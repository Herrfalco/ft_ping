/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   const.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:27:53 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/25 20:33:58 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONST_H
#define CONST_H

#define	ICMP_FILTER				1

#define HDR_SZ					8
#define BODY_SZ					56 
#define IP_HDR_SZ				20
#define TTL_IDX					8
#define IN_ADDR_SZ				4
#define MAX_SEQ_SZ				16

#define TTL						64
#define PING_INT				1

#define FLGS					"avh"
#define OPT_SZ					2
#define OPTS					"t"
#define OPTS_NB					1

#define HELP_TXT				"Usage:\n" \
								"  ft_ping [options] <destination>\n" \
								"\n" \
								"Options:\n" \
								"  <destination>      dns name or ip address\n" \
								"  -a				  use audible ping\n" \
								"  -h                 display help\n" \
								"  -v                 verbose output\n" \
								"  -t <ttl>           define time to live\n"

typedef enum					e_flag {
	F_A,
	F_V,
	F_H,
}								t_flag;

typedef enum					e_opt {
	O_T,
}								t_opt;

typedef enum					e_bool {
	FALSE,
	TRUE,
}								t_bool;

typedef enum					e_err {
	E_NO,
	E_ARG_NB,
	E_PERM,
	E_TARG,
	E_SCK_CRE,
	E_SCK_OPT,
	E_SND,
	E_REC,
	E_ALLOC,
	E_ARG,
	E_DUP,
	E_NO_MATCH,
	E_BCK_TIME,
}								t_err;

#endif //CONST_H
