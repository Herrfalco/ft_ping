/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:28:51 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/25 21:17:54 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef STRUCT_H
#define STRUCT_H

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
	uint8_t						ip_hdr[IP_HDR_SZ - 2 * IN_ADDR_SZ];
	uint32_t					ip_src;
	uint32_t					ip_dst;
	t_icmp_pkt					icmp_pkt;
} __attribute__((packed))		t_ip_pkt;

typedef struct					s_elem {
	struct timeval				time;
	t_icmp_pkt					pkt;
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

typedef struct					s_errors {
	size_t						sum;
	size_t						dup;
	size_t						err;
}								t_errors;

typedef struct					s_args {
	size_t						flags;
	size_t						opts_flags;
	unsigned int				opts[OPTS_NB];		
	unsigned int				count;
	unsigned int				inter;
}								t_args;

typedef struct					s_glob {
	t_targ						targ;
	int							sock;
	t_icmp_pkt					pkt;
	struct timeval				start;
	t_pngs						pngs;
	t_acc						acc;
	t_errors					errors;
	t_args						args;
}								t_glob;

#endif //STRUCT_H
