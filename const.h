/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   const.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:27:53 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/23 12:28:43 by fcadet           ###   ########.fr       */
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
}								t_err;

#endif //CONST_H
