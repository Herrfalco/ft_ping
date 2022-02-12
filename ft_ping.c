/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/12 23:42:16 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include <errno.h>

#define ICMP_ECHO	8

#define HDR_SZ		8
#define BODY_SZ		56
#define IP_HDR_SZ	20

#define TTL			64
#define TIMEOUT		1

typedef struct		s_icmp_pkt {
	uint8_t			type;
	uint8_t			code;
	uint16_t		sum;
	uint16_t		id;
	uint16_t		seq;
	char			body[BODY_SZ];
}					t_icmp_pkt;

struct addrinfo		create_hints(void) {
	struct addrinfo		hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_flags = 0;
	return (hints);
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

int	main(int argc, char **argv) {
	if (argc != 2) {
		printf("Error: Wrong number of arguments\n");
		return (1);
	} else if (getuid()) {
		printf("Error: not allowed\n");
		return (2);
	}

	struct in_addr		ip = { 0 };
	struct addrinfo		hints = create_hints();
	struct addrinfo		*inf = NULL;
	struct sockaddr_in	soc_ad_in = { 0 };
	struct sockaddr		*soc_ad = (struct sockaddr *)&soc_ad_in;
	uint8_t				ttl = TTL;
	struct timeval		timeout = { 0 };
	char				buff[INET_ADDRSTRLEN] = { 0 };
	int					sock = 0;
	t_icmp_pkt			pkt = { 0 };

	if (inet_pton(AF_INET, argv[1], &ip) == 1) {
		soc_ad_in.sin_family = AF_INET;
		soc_ad_in.sin_addr = ip;
		inet_ntop(AF_INET, &ip, buff, INET_ADDRSTRLEN);
	} else if ((getaddrinfo(argv[1], NULL, &hints, &inf) == 0)) {
		soc_ad_in = *((struct sockaddr_in *)inf->ai_addr);
		freeaddrinfo(inf);
		inet_ntop(AF_INET,
				&soc_ad_in.sin_addr,
				buff, INET_ADDRSTRLEN);
	} else  {
		printf("Error: Invalid domain\n");
		return (1);
	}
	if ((sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		printf("Error: Can't create socket\n");
		return (3);
	}

	timeout.tv_sec = TIMEOUT;
	if (setsockopt(sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))
			|| setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval))) {
		printf("Error: Can't configure socket\n");
		return (3);
	}
	printf("PING %s (%s) %d(%d) bytes of data.\n",
			inf ? argv[1] : buff, buff, BODY_SZ, BODY_SZ + HDR_SZ + IP_HDR_SZ);

	pkt.type = ICMP_ECHO;
	pkt.code = 0;
	//	pkt.body = { 0 };
	pkt.sum = checksum(&pkt, sizeof(t_icmp_pkt));
	pkt.id = getpid();
	pkt.seq = 666;
	if (sendto(sock, &pkt, sizeof(t_icmp_pkt), 0, soc_ad, sizeof(struct sockaddr)) < 0 ) {
		printf("Error: Can't send ping\n");
		return (4);
	}

	struct msghdr	msg;
	printf("%ld\n", recvmsg(sock, &msg, 0));
	/*
	if (recvmsg(sock, &msg, 0) < 0) {
		printf("Error: Can't receive ping\n");
	}
	*/
}
