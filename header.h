/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:05:08 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/17 13:32:02 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>

#define ICMP_ECHO				8
#define ICMP_ERESP				0

#define HDR_SZ					8
#define BODY_SZ					56 // Must be divisible by 4
#define IP_HDR_SZ				20
#define TTL_IDX					8

#define TTL						64
#define PING_INT				1

typedef enum					e_bool {
	FALSE,
	TRUE,
}								t_bool;

typedef struct					s_targ {
	struct sockaddr_in			in;
	char						addr[INET_ADDRSTRLEN];
	char						*name;
}								t_targ;

typedef struct					s_icmp_pkt {
	uint8_t						type;
	uint8_t						code;
	int16_t						sum;
	uint16_t					id;
	uint16_t					seq;
	uint8_t						body[BODY_SZ];
} __attribute__((packed))		t_icmp_pkt;

typedef struct					s_ip_pkt {
	uint8_t						ip_hdr[IP_HDR_SZ];
	t_icmp_pkt					icmp_pkt;
} __attribute__((packed))		t_ip_pkt;

typedef struct					s_elem {
	struct timeval				time;
	uint16_t					seq;
	struct s_elem				*next;
}								t_elem;

typedef struct					s_elem_lst {
	t_elem						*head;
	size_t						size;
}								t_elem_lst;

typedef struct					s_pngs {
	t_elem_lst					i;
	t_elem_lst					o;
}								t_pngs;

typedef struct					s_acc {
	long long					val;
	long long					tmp;
	t_bool						flag;
}								t_acc;

typedef struct					s_glob {
	t_targ						targ;
	int							sock;
	int							pid;
	struct timeval				start;
	t_pngs						pngs;
	t_acc						acc;
}								t_glob;

extern t_glob					glob;

uint16_t						endian_sw(uint16_t src);
uint16_t						checksum(void *body, int size);
void							mem_set(void *data, size_t size, uint8_t val);
long long						time_2_us(struct timeval tv);
struct timeval					duration(struct timeval start, struct timeval end);
long							llsqrt(long long nb);

void							new_ping(uint16_t seq);
t_elem							*ping_2_pong(uint16_t seq);
void							free_pngs(void);

long long						min_acc(t_elem *el);
long long						max_acc(t_elem *el);
long long						avg_acc(t_elem *el);
long long						mdev_acc(t_elem *el);
long long						ewma_acc(t_elem *el);
long long						fold_pongs(long long (*acc)(t_elem *));

#endif //HEADER_H
