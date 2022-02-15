/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:05:08 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/15 19:46:39 by fcadet           ###   ########.fr       */
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
#include <math.h>

#define ICMP_ECHO				8

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
	double						val;
	double						tmp;
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

t_glob							glob = { 0 };

uint16_t						endian_sw(uint16_t src);
uint16_t						checksum(void *body, int size);
void							mem_set(void *data, size_t size, uint8_t val);
double							time_2_ms(struct timeval tv);
struct timeval					duration(struct timeval start, struct timeval end);

void							new_ping(uint16_t seq);
t_elem							*ping_2_pong(uint16_t seq);
void							free_pngs(void);

double							min_acc(t_elem *el);
double							max_acc(t_elem *el);
double							avg_acc(t_elem *el);
double							mdev_acc(t_elem *el);
double							fold_pongs(double (*acc)(t_elem *));

#endif //HEADER_H
