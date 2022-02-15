/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:43:33 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/15 18:44:35 by fcadet           ###   ########.fr       */
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

double			time_2_ms(struct timeval tv) {
	return (tv.tv_sec * 1000. + tv.tv_usec / 1000.);
}

struct timeval	duration(struct timeval start, struct timeval end) {
	struct timeval		result = { 0 };	
	int					tmp;

	result.tv_sec = end.tv_sec - start.tv_sec;
	if ((tmp = end.tv_usec - start.tv_usec) < 0) {
		--result.tv_sec;
		result.tv_usec = 1000000 + tmp;
	} else {
		result.tv_usec = tmp;
	}
	return (result);
}
