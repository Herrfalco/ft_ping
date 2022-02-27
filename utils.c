/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:43:33 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/27 09:16:25 by fcadet           ###   ########.fr       */
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

	if (start.tv_sec > end.tv_sec)
		error(E_BCK_TIME, "Clock", "Time goes backward !!!", NULL);
	result.tv_sec = end.tv_sec - start.tv_sec;
	if ((tmp = end.tv_usec - start.tv_usec) < 0) {
		if (result.tv_sec < 1)
			error(E_BCK_TIME, "Clock", "Time goes backward !!!", NULL);
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

void			error(t_err ret, char *fnc, char *msg, char *quote) {
	(void)ret;
	fprintf(stderr, "Error: ");	
	if (fnc)
		fprintf(stderr, "%s: ", fnc);	
	fprintf(stderr, "%s", msg);
	if (quote)
		fprintf(stderr, " \"%s\"", quote);
	fprintf(stderr, "\n");
	if (ret) {
		free_pngs();	
		exit(2);
	}
}

t_bool			mem_cmp(void *m1, void *m2, size_t *size) {
	while ((*size)-- > 0) {
		if (((char *)m1)[*size] != ((char *)m2)[*size])
			return (TRUE);
	}
	return (FALSE);
}

size_t			str_len(char *str) {
	size_t		len;

	for (len = 0; str[len]; ++len);
	return (len);
}

t_bool			str_2_uint(char *str, unsigned int *result) {
	long		res = 0;

	for (; *str; ++str) {
		if (*str < '0' || *str > '9')
			return (TRUE);
		res *= 10;
		res += *str - '0';
		if (res > UINT_MAX)
			return (TRUE);
	}
	*result = res;
	return (FALSE);
}

t_bool			str_2_pat(char *str, t_pat *pat) {
	char		byte;
	t_bool		ho = FALSE;

	for (; *str && pat->len < PAT_SZ; ++str, ho = ho ? FALSE : TRUE) {
		if (*str >= '0' && *str <= '9') {
			byte = *str - '0';
		} else if (*str >= 'a' && *str <= 'f') {
			byte = *str - 'a' + 10;
		} else if (*str >= 'A' && *str <= 'F') {
			byte = *str - 'A' + 10;
		} else
			return (TRUE);
		if (ho)
			pat->dat[pat->len++] |= byte;
		else
			pat->dat[pat->len] = byte << 4;
	}
	if (ho)
		pat->dat[pat->len++] >>= 4;
	return (FALSE);
}
