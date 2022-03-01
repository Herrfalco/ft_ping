/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   err_resp.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 19:46:41 by fcadet            #+#    #+#             */
/*   Updated: 2022/03/01 08:17:02 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../hdrs/header.h"

void		treat_error(t_ip_pkt *pkt) {
	char			addr[INET_ADDRSTRLEN] = { 0 };

	++glob.errors.err;
	if (flag_set(F_Q) || !flag_set(F_V))
		return;
	inet_ntop(AF_INET, &pkt->ip_src, addr, INET_ADDRSTRLEN);
	printf("From %s icmp_seq=%d ", addr,
		endian_sw(((t_icmp_pkt *)(pkt->icmp_pkt.body + IP_HDR_SZ))->seq));
	switch (pkt->icmp_pkt.type) {
		case ICMP_DEST_UNREACH:
			switch (pkt->icmp_pkt.code) {
				case ICMP_NET_UNREACH:
					printf("Destination Net Unreachable\n");
					break;
				case ICMP_HOST_UNREACH:
					printf("Destination Host Unreachable\n");
					break;
				case ICMP_PROT_UNREACH:
					printf("Destination Protocol Unreachable\n");
					break;
				case ICMP_PORT_UNREACH:
					printf("Destination Port Unreachable\n");
					break;
				case ICMP_FRAG_NEEDED:
					printf("Frag needed and DF set (mtu = %u)\n", pkt->icmp_pkt.seq);
					break;
				case ICMP_SR_FAILED:
					printf("Source Route Failed\n");
					break;
				case ICMP_NET_UNKNOWN:
					printf("Destination Net Unknown\n");
					break;
				case ICMP_HOST_UNKNOWN:
					printf("Destination Host Unknown\n");
					break;
				case ICMP_HOST_ISOLATED:
					printf("Source Host Isolated\n");
					break;
				case ICMP_NET_ANO:
					printf("Destination Net Prohibited\n");
					break;
				case ICMP_HOST_ANO:
					printf("Destination Host Prohibited\n");
					break;
				case ICMP_NET_UNR_TOS:
					printf("Destination Net Unreachable for Type of Service\n");
					break;
				case ICMP_HOST_UNR_TOS:
					printf("Destination Host Unreachable for Type of Service\n");
					break;
				case ICMP_PKT_FILTERED:
					printf("Packet filtered\n");
					break;
				case ICMP_PREC_VIOLATION:
					printf("Precedence Violation\n");
					break;
				case ICMP_PREC_CUTOFF:
					printf("Precedence Cutoff\n");
					break;
				default:
					printf("Dest Unreachable, Bad Code: %d\n", pkt->icmp_pkt.code);
			}
			break;
		case ICMP_SOURCE_QUENCH:
			printf("Source Quench\n");
			break;
		case ICMP_REDIRECT:
			switch (pkt->icmp_pkt.code) {
				case ICMP_REDIR_NET:
					printf("Redirect Network\n");
					break;
				case ICMP_REDIR_HOST:
					printf("Redirect Host\n");
					break;
				case ICMP_REDIR_NETTOS:
					printf("Redirect Type of Service and Network\n");
					break;
				case ICMP_REDIR_HOSTTOS:
					printf("Redirect Type of Service and Host\n");
					break;
				default:
					printf("Redirect, Bad Code: %d\n", pkt->icmp_pkt.code);
			}
			break;
		case ICMP_TIME_EXCEEDED:
			switch(pkt->icmp_pkt.code) {
				case ICMP_EXC_TTL:
					printf("Time to live exceeded\n");
					break;
				case ICMP_EXC_FRAGTIME:
					printf("Frag reassembly time exceeded\n");
					break;
				default:
					printf("Time exceeded, Bad Code: %d\n", pkt->icmp_pkt.code);
			}
			break;
		case ICMP_PARAMETERPROB:
			printf("Parameter problem: pointer = %u\n", *((uint8_t *)&pkt->icmp_pkt + 4));
			break;
		case ICMP_TIMESTAMP:
			printf("Timestamp\n");
			break;
		case ICMP_TIMESTAMPREPLY:
			printf("Timestamp Reply\n");
			break;
		case ICMP_INFO_REQUEST:
			printf("Information Request\n");
			break;
		case ICMP_INFO_REPLY:
			printf("Information Reply\n");
			break;
		case ICMP_ADDRESS:
			printf("Address Mask Request\n");
			break;
		case ICMP_ADDRESSREPLY:
			printf("Address Mask Reply\n");
			break;
		default:
			printf("Bad ICMP type: %d\n", pkt->icmp_pkt.type);
	}
}
