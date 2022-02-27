/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_pong.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 19:54:21 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/27 09:35:27 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void		ping(int signum) {
	struct sockaddr			*targ = (struct sockaddr *)&glob.targ.in;
	static unsigned int		seq = 0;
	t_icmp_pkt				pkt = glob.pkt;
	struct timeval			now;
	unsigned int			timeout;
	unsigned int			deadline;

	(void)signum;
	gettimeofday(&now, NULL);
	if ((opt_set(O_C, T_ANY, NULL)
				&& glob.args.count-- < 1)
			|| (opt_set(O_W, T_UINT, (t_optval *)&deadline)
				&& duration(glob.start, now).tv_sec >= deadline)
			|| (glob.pngs.i.size > glob.pngs.o.size
				&& opt_set(O_UPW, T_UINT, (t_optval *)&timeout)
				&& duration(glob.lst_pong, now).tv_sec >= timeout))
		sig_int(0);
	pkt.seq = endian_sw(++seq);
	pkt.sum = checksum(&pkt, HDR_SZ + glob.args.body_sz);
	if (sendto(glob.sock, &pkt, HDR_SZ + glob.args.body_sz, 0, targ, sizeof(struct sockaddr)) < 0)
		error(E_SND, "Ping", "Can't send packet", NULL);
	new_ping(pkt);
	alarm(glob.args.inter);
}

static void		disp_err(t_bool dup, t_ip_pkt *r_pkt, t_bool quiet) {
	size_t				cmp_idx = IN_ADDR_SZ;
	uint16_t			sum;

	if (dup) {
		++glob.errors.dup;
		if (!quiet)
			printf(" (DUP!)");
	}
	sum = r_pkt->icmp_pkt.sum;
	r_pkt->icmp_pkt.sum = 0;
	if (sum != checksum(&r_pkt->icmp_pkt, HDR_SZ + glob.args.body_sz)) {
		++glob.errors.sum;
		if (!quiet)
			printf(" (BAD CHECKSUM!)");
	}
	if (!quiet && mem_cmp((void *)&glob.targ.in.sin_addr, (void *)&r_pkt->ip_src, &cmp_idx))
		printf(" (DIFFERENT ADDRESS!)");
	cmp_idx = glob.args.body_sz;
	if (!quiet && mem_cmp((void *)glob.pkt.body, (void *)r_pkt->icmp_pkt.body, &cmp_idx)) {
		printf("\nwrong data byte #%lu should be 0x%x but was 0x%x",
			cmp_idx, glob.pkt.body[cmp_idx], r_pkt->icmp_pkt.body[cmp_idx]);
		for (size_t i = 0; i < glob.args.body_sz; ++i) {
			if (!(i % 32))
				printf("\n#%lu\t", i);
			printf("%x ", r_pkt->icmp_pkt.body[i]);
		}
	}
	if (!quiet)
		printf("\n");
}

static void		check_resp(int ret_val, t_bool dup, t_ip_pkt *r_pkt, t_elem *pong) {
	char				addr[INET_ADDRSTRLEN] = { 0 };
	long long			triptime;
	t_bool				quiet = FALSE;

	if (flag_set(F_A))
		printf("\a");
	if (!(quiet = flag_set(F_Q))) {
		inet_ntop(AF_INET, &r_pkt->ip_src, addr, INET_ADDRSTRLEN);
		printf("%d bytes from %s: icmp_seq=%d ttl=%d ", ret_val - IP_HDR_SZ, addr,
			endian_sw(r_pkt->icmp_pkt.seq), r_pkt->ip_hdr[TTL_IDX]);
		triptime = time_2_us(pong->time);
		if (triptime >= 100000 - 50)
			printf("time=%lld ms", (triptime + 500) / 1000);
		else if (triptime >= 10000 - 5)
			printf("time=%lld.%01lld ms", (triptime + 50) / 1000, ((triptime + 50) % 1000) / 100);
		else if (triptime >= 1000)
			printf("time=%lld.%02lld ms", (triptime + 5) / 1000, ((triptime + 5) % 1000) / 10);
		else
			printf("time=%lld.%03lld ms", triptime / 1000, triptime % 1000);
	}
	disp_err(dup, r_pkt, quiet);
}

/*
void		print_body(t_icmp_pkt *pkt) {
	for (size_t i = 0; i < glob.args.body_sz; ++i) {
		if (!(i % 8))
			printf("\n%02ld: ", i);
		printf("%02x ", pkt->body[i]);
	}
	printf("\n");
}
*/

void		pong(void) {
	t_ip_pkt			r_pkt = { 0 };
	struct msghdr		msg = { 0 };
	t_elem				*pong = NULL;
	int					ret_val = 0;
	t_bool				dup = FALSE;
	struct iovec		io_vec =  {
		.iov_base = &r_pkt,
		.iov_len = IP_HDR_SZ + HDR_SZ + glob.args.body_sz,
	};

	msg.msg_iov = &io_vec;
	msg.msg_iovlen = 1;
	if ((ret_val = recvmsg(glob.sock, &msg, 0)) < 0)
		error(E_REC, "Ping", "Can't receive packet", NULL);
	if (r_pkt.icmp_pkt.type != ICMP_ECHOREPLY) {
		treat_error(&r_pkt);
		return;
	}
	if (r_pkt.icmp_pkt.id != glob.pkt.id)
		return;
	//print_body(&r_pkt.icmp_pkt);
	switch (ping_2_pong(r_pkt.icmp_pkt.seq, &pong)) {
		case E_NO_MATCH:
			return;
		case E_DUP:
			dup = TRUE;
			break;
		default:
			gettimeofday(&glob.lst_pong, NULL);
	}
	check_resp(ret_val, dup, &r_pkt, pong);
}
