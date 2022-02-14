/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/14 18:38:08 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//to test on localhost	sudo tcpdump -vi lo icmp and icmp[icmptype]=icmp-echo
//unordered packet		sudo tc qdisc change dev lo root netem delay 0ms 2000ms 50%
//think about stderror
//think about deleting unused var and includes
//free in general and if exit or signal
//check all possible errors and failing ways

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
#define TTL_IDX		8

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
	struct sockaddr_in			targ_in;
	char						targ_addr[INET_ADDRSTRLEN];
	char						*targ_name;
	int							sock;
	int							pid;
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

/*
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
*/

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

void	ping(int signum) {
	struct sockaddr			*targ = (struct sockaddr *)&glob.targ_in;
	static unsigned int		seq = 0;
	t_icmp_pkt				pkt = { 0 };

	(void)signum;
	pkt.type = ICMP_ECHO;
	pkt.id = endian_sw(glob.pid);
	pkt.seq = endian_sw(++seq);
	fill_body(pkt.body, 0xaabbccdd);
	pkt.sum = checksum(&pkt, sizeof(t_icmp_pkt));
	if (sendto(glob.sock, &pkt, sizeof(t_icmp_pkt), 0, targ, sizeof(struct sockaddr)) < 0 ) {
		printf("Error: Can't send ping\n");
		return;
	}
	/*
	printf("PING: type: %d, code: %d, sum: %x, id: %d, seq: %d\n",
		pkt.type, pkt.code, pkt.sum, id, seq);
		*/
	//print_body(pkt.body);
	alarm(PING_INT);
}

void	pong(void) {
		t_ip_pkt			r_pkt = { 0 };
		struct iovec		io_vec =  {
			.iov_base = &r_pkt,
			.iov_len = sizeof(t_ip_pkt),
		};
		struct msghdr		msg = { 0 };
		int					ret_val = 0;

		msg.msg_iov = &io_vec;
		msg.msg_iovlen = 1;
		if ((ret_val = recvmsg(glob.sock, &msg, 0)) < 0) {
			printf("Error: Can't receive ping\n");
			return;
		}
		if (r_pkt.icmp_pkt.id != endian_sw(glob.pid))
			return;
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=?\n",
				ret_val - IP_HDR_SZ, glob.targ_addr, endian_sw(r_pkt.icmp_pkt.seq),
				r_pkt.ip_hdr[TTL_IDX]);
		//print_body(r_pkt.icmp_pkt.body);
}

int		find_targ(char *arg) {
	struct in_addr		ip = { 0 };
	struct addrinfo		hints = create_hints();
	struct addrinfo		*inf = NULL;

	if (inet_pton(AF_INET, arg, &ip) == 1) {
		glob.targ_in.sin_family = AF_INET;
		glob.targ_in.sin_addr = ip;
		inet_ntop(AF_INET, &ip, glob.targ_addr, INET_ADDRSTRLEN);
	} else if ((getaddrinfo(arg, NULL, &hints, &inf) == 0)) {
		//check free if error
		glob.targ_in = *((struct sockaddr_in *)inf->ai_addr);
		freeaddrinfo(inf);
		inet_ntop(AF_INET,
			&glob.targ_in.sin_addr,
			glob.targ_addr, INET_ADDRSTRLEN);
		glob.targ_name = arg;
	} else  {
		printf("Error: Invalid domain\n");
		return (1);
	}
	return (0);
}

int		create_sock(void) {
	uint8_t				ttl = TTL;
	//struct timeval		timeout = { 0 };

	if ((glob.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		printf("Error: Can't create socket\n");
		return (1);
	}
	//timeout.tv_sec = TIMEOUT;
	if (setsockopt(glob.sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))) {
		printf("Error: Can't configure socket\n");
		return (2);
	}
	return (0);
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		printf("Error: Wrong number of arguments\n");
		return (1);
	} else if (getuid()) {
		printf("Error: not allowed\n");
		return (2);
	}

	int		ret_val = 0;

	if ((ret_val = find_targ(argv[1])))
		return (ret_val);
	printf("PING %s (%s) %d(%d) bytes of data.\n",
			glob.targ_name ? glob.targ_name : glob.targ_addr, glob.targ_addr,
			BODY_SZ, BODY_SZ + HDR_SZ + IP_HDR_SZ);
	if ((ret_val = create_sock()))
		return (ret_val);

	glob.pid = getpid();
	signal(SIGALRM, ping);
	ping(0);
	alarm(PING_INT);
	while (42)
		pong();
}
