/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/23 12:40:46 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//think about deleting unused var and includes
//free in general and if exit or signal
//free pendlist and if exit or signal
//check all possible errors and failing ways
//test rtt when no packets are received
//check division by 0 
//test all error responses
//set max readlen to avoid buffoverflow
//handle backward time
//put static on private functions
//options

#include "header.h"

t_glob		glob = { 0 };

void		fill_body(uint8_t *body, uint32_t pat) {
	uint32_t	*bod = (uint32_t *)body;

	for (unsigned int i = 0; i < BODY_SZ / 4; ++i)
		bod[i] = pat;
}

void	ping(int signum) {
	struct sockaddr			*targ = (struct sockaddr *)&glob.targ.in;
	static unsigned int		seq = 0;
	t_icmp_pkt				pkt = glob.pkt;

	(void)signum;
	pkt.seq = endian_sw(++seq);
	pkt.sum = checksum(&pkt, sizeof(t_icmp_pkt));
	if (sendto(glob.sock, &pkt, sizeof(t_icmp_pkt), 0, targ, sizeof(struct sockaddr)) < 0 ) {
		printf("Error: Can't send ping\n");
		return;
	}
	new_ping(pkt);
	alarm(PING_INT);
}

void	check_resp(int ret_val, int error, t_ip_pkt *r_pkt, t_elem *pong) {
	long long			triptime;
	uint16_t			sum;
	size_t				cmp_idx = IN_ADDR_SZ;

	printf("%d bytes from %s: icmp_seq=%d ttl=%d ",
			ret_val - IP_HDR_SZ, glob.targ.addr, endian_sw(r_pkt->icmp_pkt.seq),
			r_pkt->ip_hdr[TTL_IDX]);
	triptime = time_2_us(pong->time);
	if (triptime >= 100000 - 50)
		printf("time=%lld ms", (triptime + 500) / 1000);
	else if (triptime >= 10000 - 5)
		printf("time=%lld.%01lld ms", (triptime + 50) / 1000, ((triptime + 50) % 1000) / 100);
	else if (triptime >= 1000)
		printf("time=%lld.%02lld ms", (triptime + 5) / 1000, ((triptime + 5) % 1000) / 10);
	else
		printf("time=%lld.%03lld ms", triptime / 1000, triptime % 1000);
	if (error == 2) {
		++glob.errors.dup;
		printf(" (DUP!)");
	}
	sum = r_pkt->icmp_pkt.sum;
	r_pkt->icmp_pkt.sum = 0;
	if (sum != checksum(&r_pkt->icmp_pkt, sizeof(t_icmp_pkt))) {
		++glob.errors.sum;
		printf(" (BAD CHECKSUM!)");
	}
	if (mem_cmp((void *)&glob.targ.in.sin_addr, (void *)&r_pkt->ip_src, &cmp_idx))
		printf(" (DIFFERENT ADDRESS!)");
	cmp_idx = BODY_SZ;
	if (mem_cmp((void *)glob.pkt.body, (void *)r_pkt->icmp_pkt.body, &cmp_idx)) {
		printf("\nwrong data byte #%lu should be 0x%x but was 0x%x",
			cmp_idx, glob.pkt.body[cmp_idx], r_pkt->icmp_pkt.body[cmp_idx]);
		for (size_t i = 0; i < BODY_SZ; ++i) {
			if (!(i % 32))
				printf("\n#%lu\t", i);
			printf("%x ", r_pkt->icmp_pkt.body[i]);
		}
	}
	printf("\n");
}

void	pong(void) {
	t_ip_pkt			r_pkt = { 0 };
	struct msghdr		msg = { 0 };
	t_elem				*pong = NULL;
	int					ret_val = 0;
	int					error;
	struct iovec		io_vec =  {
		.iov_base = &r_pkt,
		.iov_len = sizeof(t_ip_pkt),
	};

	msg.msg_iov = &io_vec;
	msg.msg_iovlen = 1;
	if ((ret_val = recvmsg(glob.sock, &msg, 0)) < 0) {
		printf("Error: Can't receive ping\n");
		return;
	}
	if (r_pkt.icmp_pkt.id != glob.pkt.id)
		return;
	if (r_pkt.icmp_pkt.type != ICMP_ECHOREPLY) {
		++glob.errors.err;
		return;
	}
	if ((error = ping_2_pong(r_pkt.icmp_pkt.seq, &pong)) == 1)
		return;
	check_resp(ret_val, error, &r_pkt, pong);
}

struct addrinfo		create_hints(void) {
	struct addrinfo		hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	return (hints);
}

void	find_targ(char *arg) {
	struct in_addr		ip = { 0 };
	struct addrinfo		hints = create_hints();
	struct addrinfo		*inf = NULL;

	if (inet_pton(AF_INET, arg, &ip) == 1) {
		glob.targ.in.sin_family = AF_INET;
		glob.targ.in.sin_addr = ip;
		inet_ntop(AF_INET, &ip, glob.targ.addr, INET_ADDRSTRLEN);
	} else if ((getaddrinfo(arg, NULL, &hints, &inf) == 0)) {
		glob.targ.in = *((struct sockaddr_in *)inf->ai_addr);
		inet_ntop(AF_INET,
				&glob.targ.in.sin_addr,
				glob.targ.addr, INET_ADDRSTRLEN);
		glob.targ.name = arg;
	} else 
		error(E_TARG, "Target search", "Can't find domain or address");
	if (inf)
		freeaddrinfo(inf);
}

void	create_sock(void) {
	uint8_t		ttl = TTL;
	uint32_t	filt = 1 << ICMP_ECHO;

	if ((glob.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		error(E_SCK_CRE, "Socket", "Can't be created");
	if (setsockopt(glob.sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))
			|| setsockopt(glob.sock, SOL_RAW, ICMP_FILTER, &filt, sizeof(uint32_t)))
		error(E_SCK_OPT, "Socket", "Can't be configured");
}

int		main(int argc, char **argv) {
	if (argc != 2)
		error(E_ARG_NB, "Command line", "Wrong number of arguments\n");
	else if (getuid())
		error(E_PERM, "Permissions", "Need to be run with sudo\n");
	find_targ(argv[1]);
	create_sock();
	printf("PING %s (%s) %d(%d) bytes of data.\n",
			glob.targ.name ? glob.targ.name : glob.targ.addr, glob.targ.addr,
			BODY_SZ, BODY_SZ + HDR_SZ + IP_HDR_SZ);
	glob.pkt.type = ICMP_ECHO;
	glob.pkt.id = endian_sw(getpid());
	fill_body(glob.pkt.body, 0xaabbccdd);
	gettimeofday(&glob.start, NULL);
	signal(SIGALRM, ping);
	signal(SIGINT, sig_int);
	signal(SIGQUIT, sig_quit);
	ping(0);
	alarm(PING_INT);
	while (TRUE)
		pong();
}
