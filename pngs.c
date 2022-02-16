/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pngs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:04:00 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/16 11:50:07 by fcadet           ###   ########.fr       */
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
