/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/14 16:49:31 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//to test on localhost	sudo tcpdump -vi lo icmp and icmp[icmptype]=icmp-echo
//unordered packet		sudo tc qdisc change dev lo root netem delay 0ms 2000ms 50%
//think about stderror
//think about deleting unused var and includes

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define ICMP_ECHO	8

#define HDR_SZ		8
#define BODY_SZ		56 // Must be divisible by 4
#define IP_HDR_SZ	20

#define TTL			64
//#define TIMEOUT		1
#define PING_INT	1

typedef struct					s_icmp_pkt {
	uint8_t						type;
	uint8_t						code;
	int16_t						sum;
	uint16_t					id;
	uint16_t					seq;
	uint8_t						body[BODY_SZ];
} __attribute__((packed))		t_icmp_pkt;

typedef struct					s_ip_pkt {
	uint8_t						ip_hdr[IP_HDR_SZ];
	t_icmp_pkt					icmp_pkt;
} __attribute__((packed))		t_ip_pkt;

typedef struct					s_glob {
	struct sockaddr_in			soc_ad_in;
	int							soc;
}								t_glob;

t_glob							glob = { 0 };

struct addrinfo		create_hints(void) {
	struct addrinfo		hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	return (hints);
}

void		fill_body(uint8_t *body, uint32_t pat) {
	uint32_t	*bod = (uint32_t *)body;

	for (unsigned int i = 0; i < BODY_SZ / 4; ++i)
		bod[i] = pat;
}

//debug purpose
void		print_body(uint8_t *body) {
	uint32_t	*bod = (uint32_t *)body;

	for (unsigned int i = 0; i < BODY_SZ / 4; ++i) {
		printf("%02x %02x %02x %02x\n",
				bod[i] >> 0x18,
				(bod[i] >> 0x10)  & 0xff,
				(bod[i] >> 0x8)  & 0xff,
				bod[i] & 0xff);
	}
}

uint16_t	endian_sw(uint16_t src) {
	return ((src >> 8) | (src << 8));
}

uint16_t	checksum(void *body, int size) {
	uint16_t	*data = body;
	uint32_t	result = 0;

	for (; size > 1; size -= 2)
		result += *(data++);		
	if (size)
		result += *((uint8_t *)data);
	while (result >> 16)
		result = (result & 0xffff) + (result >> 16);
	return (~result);
}

void	send_ping(int signum) {
	// Send ping
	struct sockaddr			*soc_ad = (struct sockaddr *)&glob.soc_ad_in;
	t_icmp_pkt				pkt = { 0 };
	static unsigned int		seq = 0;
	unsigned int			id = getpid();

	pkt.type = ICMP_ECHO;
	pkt.id = endian_sw(id);
	pkt.seq = endian_sw(++seq);
	fill_body(pkt.body, 0xaabbccdd);
	pkt.sum = checksum(&pkt, sizeof(t_icmp_pkt));
	if (sendto(glob.soc, &pkt, sizeof(t_icmp_pkt), 0, soc_ad, sizeof(struct sockaddr)) < 0 ) {
		printf("Error: Can't send ping\n");
		return;
	}
	printf("PING: type: %d, code: %d, sum: %x, id: %d, seq: %d\n",
		pkt.type, pkt.code, pkt.sum, id, seq);
	alarm(PING_INT);
}

int	main(int argc, char **argv) {
	// Check perms
	if (argc != 2) {
		printf("Error: Wrong number of arguments\n");
		return (1);
	} else if (getuid()) {
		printf("Error: not allowed\n");
		return (2);
	}

	// Find target address
	struct in_addr		ip = { 0 };
	struct addrinfo		hints = create_hints();
	struct addrinfo		*inf = NULL;
	char				addr_str[INET_ADDRSTRLEN] = "";

	if (inet_pton(AF_INET, argv[1], &ip) == 1) {
		glob.soc_ad_in.sin_family = AF_INET;
		glob.soc_ad_in.sin_addr = ip;
		inet_ntop(AF_INET, &ip, addr_str, INET_ADDRSTRLEN);
	} else if ((getaddrinfo(argv[1], NULL, &hints, &inf) == 0)) {
		glob.soc_ad_in = *((struct sockaddr_in *)inf->ai_addr);
		freeaddrinfo(inf);
		inet_ntop(AF_INET,
				&glob.soc_ad_in.sin_addr,
				addr_str, INET_ADDRSTRLEN);
	} else  {
		printf("Error: Invalid domain\n");
		return (1);
	}
	printf("PING %s (%s) %d(%d) bytes of data.\n",
			inf ? argv[1] : addr_str, addr_str, BODY_SZ, BODY_SZ + HDR_SZ + IP_HDR_SZ);

	// Create and configure socket
	//int					sock = 0;
	uint8_t				ttl = TTL;
	//struct timeval		timeout = { 0 };

	if ((glob.soc = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		printf("Error: Can't create socket\n");
		return (3);
	}
	//timeout.tv_sec = TIMEOUT;
	if (setsockopt(glob.soc, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))) {
		printf("Error: Can't configure socket\n");
		return (3);
	}

	signal(SIGALRM, send_ping);
	send_ping(0);
	alarm(PING_INT);

	// Get pong
	while (42) {
		struct msghdr		msg = { 0 };
		t_ip_pkt			r_pkt = { 0 };
		struct iovec		io_vec =  {
			.iov_base = &r_pkt,
			.iov_len = sizeof(t_ip_pkt),
		};

		msg.msg_iov = &io_vec;
		msg.msg_iovlen = 1;
		if (recvmsg(glob.soc, &msg, 0) < 0) {
			printf("Error: Can't receive ping\n");
			return (5);
		}

		printf("  PONG: type: %d, code: %d, sum: %x, id: %d, seq: %d\n",
				r_pkt.icmp_pkt.type,
				r_pkt.icmp_pkt.code,
				r_pkt.icmp_pkt.sum,
				endian_sw(r_pkt.icmp_pkt.id),
				endian_sw(r_pkt.icmp_pkt.seq));
		//		print_body(r_pkt.icmp_pkt.body);
	}
}
