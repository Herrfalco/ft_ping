/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/26 19:13:28 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//think about deleting unused var and includes
//put static on private functions
//verbose output...
//exit conflict with -c
//ping exit value
//check body size

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
	t_optval		ttl;
	uint32_t		filt = 1 << ICMP_ECHO;

	ttl.uint = TTL;
	if ((glob.sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
		error(E_SCK_CRE, "Socket", "Can't be created", NULL);
	opt_set(O_T, T_UINT, &ttl);
	if (ttl.uint > 255)
		error(E_SCK_OPT, "Socket", "TTL value is too high", NULL);
	if (setsockopt(glob.sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(uint8_t))
			|| setsockopt(glob.sock, SOL_RAW, ICMP_FILTER, &filt, sizeof(uint32_t)))
		error(E_SCK_OPT, "Socket", "Can't be configured", NULL);
}

void		fill_body(void) {
	for (size_t i = 0; i < glob.args.body_sz; ++i) 
		glob.pkt.body[i] = glob.args.pat.dat[i % glob.args.pat.len];
}

void		init_glob(void) {
	glob.args.inter = PING_INT;
	opt_set(O_C, T_UINT, (t_optval *)&glob.args.count);
	opt_set(O_I, T_UINT, (t_optval *)&glob.args.inter);
	glob.pkt.type = ICMP_ECHO;
	glob.pkt.id = endian_sw(getpid());
	fill_body();
	gettimeofday(&glob.start, NULL);
}

int			main(int argc, char **argv) {
	t_bool		no_addr;
	t_pat		pat = {
		.dat = { 0xaa, 0xbb, 0xcc, 0xdd },
		.len = 4,
	};

	if (argc < 2)
		error(E_ARG, "Command line", "Need argument (-h for help)", NULL);
	no_addr = parse_arg(++argv);
	if (flag_set(F_H))
		disp_help();
	if (flag_set(F_UPV)) {
		printf("ft_ping by fcadet s20220226\n");
		exit(0);
	}
	if (no_addr)
		error(E_ARG, "Command line", "No domain or address specified", NULL);
	glob.args.body_sz = BODY_SZ;
	opt_set(O_S, T_UINT, (t_optval *)&glob.args.body_sz);
	if (glob.args.body_sz > MAX_BODY_SZ)
		error(E_ARG, "Command line", "Payload is too big", NULL);
	glob.args.pat = pat;
	if (opt_set(O_P, T_PAT, (t_optval *)&glob.args.pat) && !flag_set(F_Q)) {
		printf("PATTERN: 0x");
		for (size_t i = 0; i < glob.args.pat.len; ++i)
			printf("%02x", glob.args.pat.dat[i]);
		printf("\n");
	}
	if (getuid())
		error(E_PERM, "Permissions", "Need to be run with sudo", NULL);
	find_targ(argv[argc - 2]);
	create_sock();
	printf("PING %s (%s) %d(%d) bytes of data.\n", glob.targ.name ? glob.targ.name : glob.targ.addr,
		glob.targ.addr, glob.args.body_sz, glob.args.body_sz + HDR_SZ + IP_HDR_SZ);
	init_glob();
	signal(SIGALRM, ping);
	signal(SIGINT, sig_int);
	signal(SIGQUIT, sig_quit);
	ping(0);
	alarm(glob.args.inter);
	while (TRUE)
		pong();
}
