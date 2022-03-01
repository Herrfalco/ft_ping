/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_pong.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 19:54:21 by fcadet            #+#    #+#             */
/*   Updated: 2022/03/01 08:25:44 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../hdrs/header.h"

static void		time_stamp(t_icmp_pkt *pkt, struct timeval *now) {
	if (glob.args.body_sz < sizeof(struct timeval)) 
		return;
	for (size_t i = 0; i < sizeof(struct timeval); ++i)
		pkt->body[i] = ((char *)now)[i];
}

void			ping(int signum) {
	struct sockaddr			*targ = (struct sockaddr *)&glob.targ.in;
	static unsigned int		seq = 0;
	static size_t			lst_pong_sz = 0;
	static struct timeval	lst_pong = { 0 };
	static struct timeval	lst_ping = { 0 };
	t_icmp_pkt				pkt = glob.pkt;
	struct timeval			now = { 0 };
	unsigned int			timeout, deadline, inter = 0;

	gettimeofday(&now, NULL);
	if (glob.pngs.i.size == glob.pngs.o.size || glob.pngs.o.size > lst_pong_sz)
		lst_pong = now;
	lst_pong_sz = glob.pngs.o.size;
	if ((opt_set(O_W, T_UINT, (t_optval *)&deadline)
				&& duration(glob.start, now).tv_sec >= deadline)
			|| (opt_set(O_UPW, T_UINT, (t_optval *)&timeout)
				&& duration(lst_pong, now).tv_sec >= timeout)
			|| ((opt_set(O_C, T_ANY, NULL) || flag_set(F_O))
				&& glob.args.count < 1))
		sig_int(0);
	if (signum && opt_set(O_I, T_UINT, (t_optval *)&inter)
			&& duration(lst_ping, now).tv_sec < inter)
		return ((void)alarm(PING_INT));
	--glob.args.count;
	lst_ping = now;
	pkt.seq = endian_sw(++seq);
	time_stamp(&pkt, &now);
	pkt.sum = checksum(&pkt, HDR_SZ + glob.args.body_sz);
	if (sendto(glob.sock, &pkt, HDR_SZ + glob.args.body_sz, 0, targ, sizeof(struct sockaddr)) < 0)
		error(E_SND, "Ping", "Can't send packet", NULL);
	new_ping(pkt, now);
	alarm(PING_INT);
}

static void		disp_err(t_bool dup, t_ip_pkt *r_pkt, struct timeval ping_time) {
	size_t				cmp_idx = IN_ADDR_SZ;
	t_icmp_pkt			pkt = glob.pkt;
	uint16_t			sum;

	if (dup) {
		++glob.errors.dup;
		if (!flag_set(F_Q))
			printf(" (DUP!)\n");
		return;
	}
	sum = r_pkt->icmp_pkt.sum;
	r_pkt->icmp_pkt.sum = 0;
	if (sum != checksum(&r_pkt->icmp_pkt, HDR_SZ + glob.args.body_sz)) {
		++glob.errors.sum;
		if (!flag_set(F_Q))
			printf(" (BAD CHECKSUM!)");
	}
	if (flag_set(F_Q))
		return;
	if (mem_cmp((void *)&glob.targ.in.sin_addr, (void *)&r_pkt->ip_src, &cmp_idx))
		printf(" (DIFFERENT ADDRESS!)");
	time_stamp(&pkt, &ping_time);
	cmp_idx = glob.args.body_sz;
	if (mem_cmp((void *)pkt.body, (void *)r_pkt->icmp_pkt.body, &cmp_idx)) {
		printf("\nwrong data byte #%lu should be 0x%x but was 0x%x",
			cmp_idx, glob.pkt.body[cmp_idx], r_pkt->icmp_pkt.body[cmp_idx]);
		for (size_t i = 0; i < glob.args.body_sz; ++i) {
			if (!(i % 32))
				printf("\n#%lu\t", i);
			printf("%x ", r_pkt->icmp_pkt.body[i]);
		}
	}
	printf("\n");
}

static void		check_resp(int ret_val, t_bool dup, t_ip_pkt *r_pkt, t_elem *pong,
	struct timeval ping_time) {
	char				addr[INET_ADDRSTRLEN] = { 0 };
	long long			triptime;

	if (!flag_set(F_Q)) {
		if (flag_set(F_A))
			printf("\a");
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
	disp_err(dup, r_pkt, ping_time);
}

void		pong(void) {
	t_ip_pkt			r_pkt = { 0 };
	struct msghdr		msg = { 0 };
	t_elem				*pong = NULL;
	int					ret_val = 0;
	t_bool				dup = FALSE;
	struct timeval		ping_time;
	struct iovec		io_vec =  {
		.iov_base = &r_pkt,
		.iov_len = IP_HDR_SZ + HDR_SZ + glob.args.body_sz,
	};

	msg.msg_iov = &io_vec;
	msg.msg_iovlen = 1;
	if ((ret_val = recvmsg(glob.sock, &msg, 0)) < 0)
		error(E_REC, "Ping", "Can't receive packet", NULL);
	if (r_pkt.icmp_pkt.type != ICMP_ECHOREPLY)
		return ((void)treat_error(&r_pkt));
	if (r_pkt.icmp_pkt.id != glob.pkt.id)
		return;
	switch (ping_2_pong(r_pkt.icmp_pkt.seq, &pong, &ping_time)) {
		case E_NO_MATCH:
			return;
		case E_DUP:
			dup = TRUE;
		default:
			break;
	}
	check_resp(ret_val, dup, &r_pkt, pong, ping_time);
}
