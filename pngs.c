/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pngs.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:04:00 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/24 12:41:47 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

static void		push_elem(t_elem_lst *lst, t_elem *el) {
	el->next = lst->head;
	lst->head = el;
	++lst->size;
}

void		new_ping(t_icmp_pkt pkt) {
	t_elem		*new = malloc(sizeof(t_elem));

	if (!new)
		error(E_ALLOC, "Memory", "Can't allocate enough ressources", NULL);
	gettimeofday(&new->time, NULL);
	new->pkt = pkt;
	push_elem(&glob.pngs.i, new);
}

t_bool		ping_2_pong(uint16_t seq, t_elem **pong) {
	t_elem			*prev = NULL;
	struct timeval	now;

	for (*pong = glob.pngs.i.head; *pong && (*pong)->pkt.seq != seq; *pong = (*pong)->next)
		prev = *pong;
	if (!*pong) {
		for	(*pong = glob.pngs.o.head; *pong && (*pong)->pkt.seq != seq; *pong = (*pong)->next);
		if (!*pong)
			error(E_PNG_NFND, "Pong", "Can't match reponse with any request", NULL);
		return (TRUE);
	}
	if (prev)
		prev->next = (*pong)->next;
	else
		glob.pngs.i.head = (*pong)->next;
	gettimeofday(&now, NULL);
	(*pong)->time = duration((*pong)->time, now);
	push_elem(&glob.pngs.o, *pong);
	return (FALSE);
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
