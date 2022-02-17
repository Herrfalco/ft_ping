/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   stats.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/16 11:49:44 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/17 11:38:42 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

long long		min_acc(t_elem *el) {
	if (el && (!glob.acc.flag || glob.acc.val > time_2_us(el->time))) {
		glob.acc.flag = TRUE;	
		glob.acc.val = time_2_us(el->time);
	}
	return (glob.acc.val);
}

long long		max_acc(t_elem *el) {
	if (el && (!glob.acc.flag || glob.acc.val < time_2_us(el->time))) {
		glob.acc.flag = TRUE;	
		glob.acc.val = time_2_us(el->time);
	}
	return (glob.acc.val);
}

long long		avg_acc(t_elem *el) {
	if (!el)
		return (glob.acc.val / glob.pngs.o.size);
	glob.acc.val += time_2_us(el->time);
	return (glob.acc.val);
}

long long		mdev_acc(t_elem *el) {
	if (el) {
		glob.acc.val += time_2_us(el->time);
		glob.acc.tmp += time_2_us(el->time) * time_2_us(el->time);
	} else {
		if (glob.acc.val < INT_MAX)
			glob.acc.val = (glob.acc.tmp - ((glob.acc.val * glob.acc.val) / glob.pngs.o.size))
				/ glob.pngs.o.size;
		else {
			glob.acc.val = (glob.acc.tmp / glob.pngs.o.size) - ((glob.acc.val / glob.pngs.o.size)
				* (glob.acc.val / glob.pngs.o.size));
		}
		return (llsqrt(glob.acc.val));
	}
	return (glob.acc.val);
}

long long		ewma_acc(t_elem *el) {
	if (el) {
		if (!glob.acc.val)
			glob.acc.val = time_2_us(el->time) * 8;
		else
			glob.acc.val += time_2_us(el->time) - glob.acc.val / 8;
	}
	return (glob.acc.val);
}

long long		fold_pongs(long long (*acc)(t_elem *)) {
	t_elem		*pong = glob.pngs.o.head;

	mem_set(&glob.acc, sizeof(t_acc), 0);
	for (; pong; pong = pong->next)
		acc(pong);
	return (acc(NULL));
}
