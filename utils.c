/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:43:33 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/21 19:59:51 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

uint16_t	endian_sw(uint16_t src) {
	return ((src >> 8) | (src << 8));
}

uint16_t	checksum(void *body, int size) {
	uint16_t	*data = body;
	uint32_t	result = 0;

	for (; size > 1; size -= 2)
		result += *(data++);		
	if (size)
		result += *((uint8_t *)data);
	while (result >> 16)
		result = (result & 0xffff) + (result >> 16);
	return (~result);
}

void			mem_set(void *data, size_t size, uint8_t val) {
	while (size-- > 0)
		((uint8_t *)data)[size] = val;
}

long long		time_2_us(struct timeval tv) {
	return (tv.tv_sec * 1000000 + tv.tv_usec);
}

struct timeval	duration(struct timeval start, struct timeval end) {
	struct timeval		result = { 0 };	
	long				tmp;

	result.tv_sec = end.tv_sec - start.tv_sec;
	if ((tmp = end.tv_usec - start.tv_usec) < 0) {
		--result.tv_sec;
		result.tv_usec = 1000000 + tmp;
	} else {
		result.tv_usec = tmp;
	}
	return (result);
}

long			 llsqrt(long long nb) {
	long long prev = LLONG_MAX;
	long long x = nb;

	if (x > 0) {
		while (x < prev) {
			prev = x;
			x = (x + (nb / x)) / 2;
		}
	}
	return x;
}

void			error(int ret, char *fnc, char *msg) {
	fprintf(stderr, "Error: ");	
	if (fnc)
		fprintf(stderr, "%s: ", fnc);	
	fprintf(stderr, "%s\n", msg);
	if (ret)
		exit(ret);
}

t_bool			mem_cmp(void *m1, void *m2, size_t *size) {
	while ((*size)-- > 0) {
		if (((char *)m1)[*size] != ((char *)m2)[*size])
			return (TRUE);
	}
	return (FALSE);
}
