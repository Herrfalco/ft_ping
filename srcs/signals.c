/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:25:56 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/27 13:43:12 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../hdrs/header.h"

static int			get_ret_val(void) {
	unsigned int	count;

	return (!glob.pngs.o.size
			|| (opt_set(O_C, T_UINT, (t_optval *)&count)
				&& opt_set(O_W, T_ANY, NULL)
				&& glob.pngs.o.size < count));
}

void		sig_int(int signum) {
	struct timeval		now;
	long long			min, max, avg, mdev;

	(void)signum;	
	alarm(0);
	signal(SIGALRM, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	gettimeofday(&now, NULL);
	printf("\n--- %s ping statistics ---\n", glob.targ.name ? glob.targ.name : glob.targ.addr);
	printf("%ld packets transmitted, %ld received", glob.pngs.i.size, glob.pngs.o.size);
	if (glob.errors.dup)
		printf(", +%ld duplicates", glob.errors.dup);
	if (glob.errors.sum)
		printf(", +%ld corrupted", glob.errors.sum);
	if (glob.errors.err)
		printf(", +%ld errors", glob.errors.err);
	printf(", %g%% packet loss, time %lldms\n", glob.pngs.i.size ?
			100. - (glob.pngs.o.size * 100. / glob.pngs.i.size) : 0,
			time_2_us(duration(glob.time.start, now)) / 1000);
	if (glob.pngs.o.size) {
		min = fold_pongs(min_acc);
		avg = fold_pongs(avg_acc);
		max = fold_pongs(max_acc);
		mdev = fold_pongs(mdev_acc);
		printf("rtt min/avg/max/mdev = %lld.%03lld/%lld.%03lld/%lld.%03lld/%lld.%03lld ms\n",
				min / 1000, min % 1000, avg / 1000, avg % 1000,
				max / 1000, max % 1000, mdev / 1000, mdev % 1000);
	}
	free_pngs();
	exit(get_ret_val());
}

void		sig_quit(int signum) {
	struct timeval		now;
	long long			min, max, avg, ewma;

	(void)signum;	
	gettimeofday(&now, NULL);
	min = fold_pongs(min_acc);
	avg = fold_pongs(avg_acc);
	max = fold_pongs(max_acc);
	ewma = fold_pongs(ewma_acc);;
	fprintf(stderr, "\r%ld/%ld packets, %ld%% loss",
			glob.pngs.o.size, glob.pngs.i.size, glob.pngs.i.size ?
			100 - (glob.pngs.o.size * 100 / glob.pngs.i.size) : 0);
	if (glob.pngs.o.size) {
		fprintf(stderr, ", min/avg/ewma/max = %lld.%03lld/%lld.%03lld/%lld.%03lld/%lld.%03lld ms",
				min / 1000, min % 1000, avg / 1000, avg % 1000, ewma / 8000, (ewma / 8) % 1000,
				max / 1000, max % 1000);
	}
	fprintf(stderr, "\n");
}
