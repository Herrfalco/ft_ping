/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   const.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:27:53 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/24 12:18:52 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONST_H
#define CONST_H

#define	ICMP_ECHO				8			
#define	ICMP_ECHOREPLY			0
#define	ICMP_FILTER				1

#define HDR_SZ					8
#define BODY_SZ					56 
#define IP_HDR_SZ				20
#define TTL_IDX					8
#define IN_ADDR_SZ				4

#define TTL						64
#define PING_INT				1

#define FLGS					"vh"
#define FLG_SZ					2

#define HELP_TXT				"Usage:\n" \
								"  ft_ping [options] <destination>\n" \
								"\n" \
								"Options:\n" \
								"  <destination>      dns name or ip address\n" \
								"  -h                 display help\n" \
								"  -v                 verbose output\n"

typedef enum					e_flag {
	VERBOSE,
	HELP,
}								t_flag;

typedef enum					e_bool {
	FALSE,
	TRUE,
}								t_bool;

typedef enum					e_err {
	E_NO_FAT,
	E_ARG_NB,
	E_PERM,
	E_TARG,
	E_SCK_CRE,
	E_SCK_OPT,
	E_SND,
	E_REC,
	E_PNG_NFND,
	E_ALLOC,
	E_ARG,
}								t_err;

#endif //CONST_H
