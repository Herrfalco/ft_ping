/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pngs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:04:00 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/15 18:43:00 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

static void		push_elem(t_elem_lst *lst, t_elem *el) {
	el->next = lst->head;
	lst->head = el;
	++lst->size;
}

void		new_ping(uint16_t seq) {
	t_elem		*new = malloc(sizeof(t_elem));

	if (!new) {
		printf("Error: Can't allocate enough ressources\n");
		exit(6);
	}
	gettimeofday(&new->time, NULL);
	new->seq = seq;
	push_elem(&glob.pngs.i, new);
}

t_elem		*ping_2_pong(uint16_t seq) {
	t_elem			*prev = NULL;
	t_elem			*elem;
	struct timeval	now;

	for (elem = glob.pngs.i.head; elem && elem->seq != seq; elem = elem->next)
		prev = elem;	
	if (!elem)
		return (NULL);
	if (prev)
		prev->next = elem->next;
	else
		glob.pngs.i.head = elem->next;
	gettimeofday(&now, NULL);
	elem->time = duration(elem->time, now);
	push_elem(&glob.pngs.o, elem);
	return (elem);
}

static void		rec_free_lst(t_elem *elem) {
	if (!elem)
		return;
	rec_free_lst(elem->next);
	free(elem);
}

void		free_pngs(void) {
	rec_free_lst(glob.pngs.i.head);
	rec_free_lst(glob.pngs.o.head);
}

double		min_acc(t_elem *el) {
	if (el && (!glob.acc.flag || glob.acc.val > time_2_ms(el->time))) {
		glob.acc.flag = TRUE;	
		glob.acc.val = time_2_ms(el->time);
	}
	return (glob.acc.val);
}

double		max_acc(t_elem *el) {
	if (el && (!glob.acc.flag || glob.acc.val < time_2_ms(el->time))) {
		glob.acc.flag = TRUE;	
		glob.acc.val = time_2_ms(el->time);
	}
	return (glob.acc.val);
}

double		avg_acc(t_elem *el) {
	if (!el)
		return (glob.acc.val / glob.pngs.o.size);
	glob.acc.val += time_2_ms(el->time);
	return (glob.acc.val);
}

double		mdev_acc(t_elem *el) {
	if (el) {
		if (!glob.acc.flag) {
			glob.acc.flag = TRUE;	
			glob.acc.tmp = fold_pongs(avg_acc);
		}
		glob.acc.val += (time_2_ms(el->time) - glob.acc.tmp)
			* (time_2_ms(el->time) - glob.acc.tmp);
	} else {
		return (sqrtl(glob.acc.val / (glob.pngs.o.size - 1)));
	}
	return (glob.acc.val);
}

double		fold_pongs(double (*acc)(t_elem *)) {
	t_elem		*pong = glob.pngs.o.head;

	mem_set(&glob.acc, sizeof(t_acc), 0);
	for (; pong; pong = pong->next)
		acc(pong);
	return (acc(NULL));
}
