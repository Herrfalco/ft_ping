/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   struct.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:28:51 by fcadet            #+#    #+#             */
/*   Updated: 2022/03/02 08:21:10 by fcadet           ###   ########.fr       */
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
	uint16_t					sum;
	uint16_t					id;
	uint16_t					seq;
	uint8_t						body[MAX_BODY_SZ];
} __attribute__((packed))		t_icmp_pkt;

typedef struct					s_ip_pkt {
	uint8_t						ip_hdr[IP_HDR_SZ - 8];
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

typedef struct					s_pat {
	char						dat[PAT_SZ];
	size_t						len;
}								t_pat;

typedef union					u_optval {
	unsigned int				uint;
	t_pat						pat;
}								t_optval;

typedef struct					s_args {
	size_t						flags;
	size_t						opts_flags;
	t_optval					opts[OPTS_NB];		
	unsigned int				count;
	unsigned int				body_sz;
	t_pat						pat;
}								t_args;

typedef struct					s_glob {
	t_targ						targ;
	int							sock;
	struct timeval				start;
	t_icmp_pkt					pkt;
	t_pngs						pngs;
	t_acc						acc;
	t_errors					errors;
	t_args						args;
}								t_glob;

#endif //STRUCT_H
