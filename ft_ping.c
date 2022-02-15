/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/15 19:59:18 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//to test on localhost	sudo tcpdump -vi lo icmp and icmp[icmptype]=icmp-echo
//unordered packet		sudo tc qdisc change dev lo root netem delay 0ms 2000ms 50%
//think about stderror
//think about deleting unused var and includes
//free in general and if exit or signal
//free pendlist and if exit or signal
//check all possible errors and failing ways
//verify pong integrety
//check number format
//test rtt when no packets are received
//check division by 0 

#include "header.h"

t_glob		glob = { 0 };

void		quit(int signum) {
	struct timeval		now;

	(void)signum;	
	gettimeofday(&now, NULL);
	printf("\n--- %s ping statistics ---\n", glob.targ.name ? glob.targ.name : glob.targ.addr);
	printf("%ld packets transmitted, %ld received, %g%% packet loss, time %dms\n",
		glob.pngs.i.size, glob.pngs.o.size, 100. - (glob.pngs.o.size * 100. / glob.pngs.i.size),
		(unsigned int)(time_2_ms(duration(glob.start, now))));
	printf("rtt min/avg/max/mdev = %.3f/%.3f/%.3f/%.3f ms\n",
		fold_pongs(min_acc), fold_pongs(avg_acc), fold_pongs(max_acc), fold_pongs(mdev_acc));
	free_pngs();
	exit(0);
}

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

void	ping(int signum) {
	struct sockaddr			*targ = (struct sockaddr *)&glob.targ.in;
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
	new_ping(seq);
	alarm(PING_INT);
}

void	pong(void) {
	t_ip_pkt			r_pkt = { 0 };
	struct msghdr		msg = { 0 };
	t_elem				*pong;
	int					ret_val = 0;
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
	if (r_pkt.icmp_pkt.id != endian_sw(glob.pid))
		return;
	if (!(pong = ping_2_pong(endian_sw(r_pkt.icmp_pkt.seq))))
		return;
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%.15g ms\n",
			ret_val - IP_HDR_SZ, glob.targ.addr, endian_sw(r_pkt.icmp_pkt.seq),
			r_pkt.ip_hdr[TTL_IDX], time_2_ms(pong->time));
	//print_body(r_pkt.icmp_pkt.body);
}

int		find_targ(char *arg) {
	struct in_addr		ip = { 0 };
	struct addrinfo		hints = create_hints();
	struct addrinfo		*inf = NULL;

	if (inet_pton(AF_INET, arg, &ip) == 1) {
		glob.targ.in.sin_family = AF_INET;
		glob.targ.in.sin_addr = ip;
		inet_ntop(AF_INET, &ip, glob.targ.addr, INET_ADDRSTRLEN);
	} else if ((getaddrinfo(arg, NULL, &hints, &inf) == 0)) {
		//check free if error
		glob.targ.in = *((struct sockaddr_in *)inf->ai_addr);
		freeaddrinfo(inf);
		inet_ntop(AF_INET,
				&glob.targ.in.sin_addr,
				glob.targ.addr, INET_ADDRSTRLEN);
		glob.targ.name = arg;
	} else  {
		printf("Error: Invalid domain\n");
		return (1);
	}
	return (0);
}

int		create_sock(void) {
	uint8_t				ttl = TTL;

	if ((glob.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		printf("Error: Can't create socket\n");
		return (1);
	}
	if (setsockopt(glob.sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))) {
		printf("Error: Can't configure socket\n");
		return (2);
	}
	return (0);
}

int	main(int argc, char **argv) {
	int		ret_val = 0;

	if (argc != 2) {
		printf("Error: Wrong number of arguments\n");
		return (1);
	} else if (getuid()) {
		printf("Error: not allowed\n");
		return (2);
	}

	if ((ret_val = find_targ(argv[1])))
		return (ret_val);
	printf("PING %s (%s) %d(%d) bytes of data.\n",
			glob.targ.name ? glob.targ.name : glob.targ.addr, glob.targ.addr,
			BODY_SZ, BODY_SZ + HDR_SZ + IP_HDR_SZ);
	if ((ret_val = create_sock()))
		return (ret_val);

	glob.pid = getpid();
	gettimeofday(&glob.start, NULL);
	signal(SIGALRM, ping);
	signal(SIGINT, quit);
	ping(0);
	alarm(PING_INT);
	while (42)
		pong();
}
