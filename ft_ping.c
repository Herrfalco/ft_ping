/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/24 12:48:20 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//think about deleting unused var and includes
//test rtt when no packets are received
//test all error responses
//set max readlen to avoid buffoverflow
//handle backward time
//put static on private functions
//options
//add variable field in error display

#include "header.h"

t_glob		glob = { 0 };

struct addrinfo		create_hints(void) {
	struct addrinfo		hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	return (hints);
}

void		find_targ(char *arg) {
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
		error(E_TARG, "Target search", "Can't find domain or address", arg);
	if (inf)
		freeaddrinfo(inf);
}

void		create_sock(void) {
	uint8_t		ttl = TTL;
	uint32_t	filt = 1 << ICMP_ECHO;

	if ((glob.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		error(E_SCK_CRE, "Socket", "Can't be created", NULL);
	if (setsockopt(glob.sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))
			|| setsockopt(glob.sock, SOL_RAW, ICMP_FILTER, &filt, sizeof(uint32_t)))
		error(E_SCK_OPT, "Socket", "Can't be configured", NULL);
}

void		fill_body(uint8_t *body, uint32_t pat) {
	uint32_t	*bod = (uint32_t *)body;

	for (unsigned int i = 0; i < BODY_SZ / 4; ++i)
		bod[i] = pat;
}

int			main(int argc, char **argv) {
	size_t		i;

	if (argc < 2)
		error(E_ARG, "Command line", "Need argument (-h for help)", NULL);
	for (i = 1; i < (size_t)argc - 1; ++i)
		add_flag(argv[i], TRUE);
	add_flag(argv[i], FALSE);
	if (is_set(HELP))
		disp_help();
	if (getuid())
		error(E_PERM, "Permissions", "Need to be run with sudo\n", NULL);
	find_targ(argv[i]);
	create_sock();
	printf("PING %s (%s) %d(%d) bytes of data.\n", glob.targ.name ? glob.targ.name : glob.targ.addr,
		glob.targ.addr, BODY_SZ, BODY_SZ + HDR_SZ + IP_HDR_SZ);
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
