/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   const.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:27:53 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/26 19:39:16 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONST_H
#define CONST_H

#define	ICMP_FILTER			1

#define HDR_SZ				8
#define MAX_BODY_SZ			65507
#define BODY_SZ				56 
#define IP_HDR_SZ			20
#define TTL_IDX				8
#define IN_ADDR_SZ			4
#define MAX_SEQ_SZ			16

#define TTL					64
#define PAT_SZ				16
#define PING_INT			1

#define FLGS				"avhqV"
#define OPT_SZ				2
#define OPTS				"tcipswW"
#define OPTS_NB				7

#define HELP_TXT			"Usage:\n" \
							"  ft_ping [options] <destination>\n\n" \
							"Options:\n" \
							"  <destination>      dns name or ip address\n" \
							"  -a				  use audible ping\n" \
							"  -c <count>         stop after <count> replies" \
							"  -h                 display help\n" \
							"  -i <interval>      seconds between sending each packet\n" \
							"  -p <pattern>       contents of padding byte\n" \
							"  -q                 quiet output\n" \
							"  -s <size>          use <size> as number of data bytes to be sent\n" \
							"  -t <ttl>           define time to live\n" \
							"  -v                 verbose output\n" \
							"  -V                 print version and exit\n" \
							"  -w <deadline>      reply wait <deadline> in seconds\n" \
							"  -W <timeout>       time to wait for response\n"

typedef enum				e_flag {
	F_A,
	F_V,
	F_H,
	F_Q,
	F_UPV,
}							t_flag;

typedef enum				e_opt {
	O_T,
	O_C,
	O_I,
	O_P,
	O_S,
	O_W,
	O_UPW,
}							t_opt;

typedef enum				e_optype {
	T_ANY,
	T_UINT,
	T_PAT,
}							t_optype;

typedef enum				e_bool {
	FALSE,
	TRUE,
}							t_bool;

typedef enum				e_err {
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
}							t_err;

#endif //CONST_H
