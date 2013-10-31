/* (C) 2013 John Spencer. released under musl's standard MIT license. */
#undef _GNU_SOURCE
#define _GNU_SOURCE
#include <ifaddrs.h>
#include <stdlib.h>
#include <net/if.h> /* IFNAMSIZ, ifreq, ifconf */
#include <net/ethernet.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h> /* inet_pton */
#include <unistd.h>
#include <sys/ioctl.h>
#include <dirent.h>

typedef union {
	struct sockaddr_in6 v6;
	struct sockaddr_in v4;
	struct sockaddr_ll ll;
} soa;

typedef struct ifaddrs_storage {
	struct ifaddrs ifa;
	soa addr;
	soa netmask;
	soa dst;
	char name[IFNAMSIZ+1];
} stor;
#define next ifa.ifa_next

static stor* list_add(stor** list, char* ifname)
{
	stor* curr = calloc(1, sizeof(stor));
	if(curr) {
		strcpy(curr->name, ifname);
		curr->ifa.ifa_name = curr->name;
		curr->next = (struct ifaddrs *) *list;
		*list = curr;
	}
	return curr;
}

void freeifaddrs(struct ifaddrs *ifp)
{
	stor *head = (stor *) ifp;
	while(head) {
		void *p = head;
		head = (stor *) head->next;
		free(p);
	}
}

static void ipv6netmask(unsigned prefix_length, struct sockaddr_in6 *sa)
{
	unsigned char* hb = sa->sin6_addr.s6_addr;
	unsigned onebytes = prefix_length / 8;
	unsigned bits = prefix_length % 8;
	unsigned nullbytes = 16 - onebytes;
	memset(hb, -1, onebytes);
	memset(hb+onebytes, 0, nullbytes);
	if(bits) {
		unsigned char x = -1;
		x <<= 8 - bits;
		hb[onebytes] = x;
	}
}

static void dealwithipv6(stor **list)
{
	FILE* f = fopen("/proc/net/if_inet6", "r");
	/* 00000000000000000000000000000001 01 80 10 80 lo
	   A                                B  C  D  E  F
	   all numbers in hex
	   A = addr B=netlink device#, C=prefix length,
	   D = scope value (ipv6.h) E = interface flags (rnetlink.h, addrconf.c)
	   F = if name */
	char v6conv[32 + 7 + 1], *v6;
	char *line, linebuf[512];
	if(!f) return;
	while((line = fgets(linebuf, sizeof linebuf, f))) {
		v6 = v6conv;
		size_t i = 0;
		for(; i < 8; i++) {
			memcpy(v6, line, 4);
			v6+=4;
			*v6++=':';
			line+=4;
		}
		--v6; *v6 = 0;
		line++;
		unsigned b, c, d, e;
		char name[IFNAMSIZ+1];
		if(5 == sscanf(line, "%x %x %x %x %s", &b, &c, &d, &e, name)) {
			struct sockaddr_in6 sa = {0};
			if(1 == inet_pton(AF_INET6, v6conv, &sa.sin6_addr)) {
				sa.sin6_family = AF_INET6;
				stor* curr = list_add(list, name);
				if(!curr) goto out;
				curr->addr.v6 = sa;
				curr->ifa.ifa_addr = (struct sockaddr*) &curr->addr;
				ipv6netmask(c, &sa);
				curr->netmask.v6 = sa;
				curr->ifa.ifa_netmask = (struct sockaddr*) &curr->netmask;
				/* find ipv4 struct with the same interface name to copy flags */
				stor* scan = *list;
				for(;scan && strcmp(name, scan->name);scan=(stor*)scan->next);
				if(scan) curr->ifa.ifa_flags = scan->ifa.ifa_flags;
				else curr->ifa.ifa_flags = 0;
			} else errno = 0;
		}
	}
	out:
	fclose(f);
}

int getifaddrs(struct ifaddrs **ifap)
{
	DIR * dir;
	struct dirent * dirent;
	stor *list = 0, *head = 0, *addr = 0;
	int sock = socket(PF_INET, SOCK_DGRAM|SOCK_CLOEXEC, IPPROTO_IP);
	struct ifreq req;
	size_t i;

	if (sock == -1) goto err0;

	dir = opendir("/sys/class/net");
	if (!dir) goto err1;
	while ((dirent = readdir(dir))) {
		if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, ".."))
			continue;
		strncpy(req.ifr_name, dirent->d_name, sizeof req.ifr_name - 1);
		req.ifr_name[sizeof req.ifr_name - 1] = '\0';
		head = list_add(&list, req.ifr_name);
		if (!head) goto err2;
		head->addr.ll.sll_family = AF_PACKET;
		if (ioctl(sock, SIOCGIFINDEX, &req)) goto err2;
		head->addr.ll.sll_ifindex = req.ifr_ifindex;
		if (ioctl(sock, SIOCGIFHWADDR, &req)) goto err2;
		head->addr.ll.sll_hatype = req.ifr_hwaddr.sa_family;
		switch (req.ifr_hwaddr.sa_family)
		{
			case ARPHRD_ETHER:
				head->addr.ll.sll_halen = ETHER_ADDR_LEN;
				break;
			default:
				head->addr.ll.sll_halen = 0;
		}
		head->ifa.ifa_addr = (struct sockaddr *) &head->addr;
		memcpy(head->addr.ll.sll_addr, req.ifr_hwaddr.sa_data, head->addr.ll.sll_halen);
	}
	closedir(dir);

	struct ifreq reqs[32]; /* arbitrary chosen boundary */
	struct ifconf conf = {.ifc_len = sizeof reqs, .ifc_req = reqs};
	if (ioctl(sock, SIOCGIFCONF, &conf)) goto err1;
	size_t reqitems = conf.ifc_len / sizeof(struct ifreq);
	for (head = list; head; head = (stor*)head->next) {
		for(i = 0; i < reqitems; i++) {
			// get SIOCGIFADDR of active interfaces.
			if(!strcmp(reqs[i].ifr_name, head->name)) break;
		}
		if (i == reqitems) continue;

		addr = list_add(&list, reqs[i].ifr_name);
		if (!addr) goto err1;
		addr->addr.v4 = *(struct sockaddr_in *) &reqs[i].ifr_addr;
		addr->ifa.ifa_addr = (struct sockaddr *) &addr->addr;
		strcpy(req.ifr_name, addr->name);
		if (ioctl(sock, SIOCGIFFLAGS, &req)) goto err1;
		addr->ifa.ifa_flags = req.ifr_flags;
		if (addr->ifa.ifa_addr) {
			/* or'ing flags with IFF_LOWER_UP on active interfaces to mimic glibc */
			addr->ifa.ifa_flags |= IFF_LOWER_UP;
			if (ioctl(sock, SIOCGIFNETMASK, &req)) goto err1;
			addr->netmask.v4 = *(struct sockaddr_in*)&req.ifr_netmask;
			addr->ifa.ifa_netmask = (struct sockaddr*) &addr->netmask;
	
			if (addr->ifa.ifa_flags & IFF_POINTOPOINT) {
				if (ioctl(sock, SIOCGIFDSTADDR, &req)) goto err1;
				addr->dst.v4 = *(struct sockaddr_in*)&req.ifr_dstaddr;
			} else {
				if (ioctl(sock, SIOCGIFBRDADDR, &req)) goto err1;
				addr->dst.v4 = *(struct sockaddr_in*)&req.ifr_broadaddr;
			}
			addr->ifa.ifa_ifu.ifu_dstaddr = (struct sockaddr*) &addr->dst;
		}
	}
	close(sock);
	dealwithipv6(&list);
	*ifap = (struct ifaddrs*) list;
	return 0;
	err2:
	closedir(dir);
	err1:
	close(sock);
	err0:
	freeifaddrs((struct ifaddrs*) list);
	return -1;
}

