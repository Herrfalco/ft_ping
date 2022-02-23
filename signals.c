/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:25:56 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/22 16:33:25 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void		sig_int(int signum) {
	struct timeval		now;
	long long			min, max, avg, mdev;

	(void)signum;	
	gettimeofday(&now, NULL);
	printf("\n--- %s ping statistics ---\n", glob.targ.name ? glob.targ.name : glob.targ.addr);
	printf("%ld packets transmitted, %ld received", glob.pngs.i.size, glob.pngs.o.size);
	if (glob.errors.dup)
		printf(", +%ld duplicates", glob.errors.dup);
	if (glob.errors.sum)
		printf(", +%ld corrupted", glob.errors.sum);
	if (glob.errors.err)
		printf(", +%ld errors", glob.errors.err);
	printf(", %g%% packet loss, time %lldms\n", 100. - (glob.pngs.o.size * 100. / glob.pngs.i.size),
		time_2_us(duration(glob.start, now)) / 1000);
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
	exit(0);
}

void		sig_quit(int signum) {
	struct timeval		now;
	long long			min, max, avg, ewma;

	(void)signum;	
	gettimeofday(&now, NULL);
	if (glob.pngs.o.size) {
		min = fold_pongs(min_acc);
		avg = fold_pongs(avg_acc);
		max = fold_pongs(max_acc);
		ewma = fold_pongs(ewma_acc);;
		fprintf(stderr, "\r%ld/%ld packets, %ld%% loss, ",
				glob.pngs.o.size, glob.pngs.i.size,
				100 - (glob.pngs.o.size * 100 / glob.pngs.i.size));
		fprintf(stderr, "min/avg/ewma/max = %lld.%03lld/%lld.%03lld/%lld.%03lld/%lld.%03lld ms\n",
				min / 1000, min % 1000, avg / 1000, avg % 1000, ewma / 8000, (ewma / 8) % 1000,
				max / 1000, max % 1000);
	}
}
