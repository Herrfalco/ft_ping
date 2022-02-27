/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   proto.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/22 16:30:37 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/27 09:34:18 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PROTO_H
#define PROTO_H

uint16_t						endian_sw(uint16_t src);
uint16_t						checksum(void *body, int size);
void							mem_set(void *data, size_t size, uint8_t val);
long long						time_2_us(struct timeval tv);
struct timeval					duration(struct timeval start, struct timeval end);
long							llsqrt(long long nb);
void							error(t_err ret, char *fnc, char *msg, char *quote);
t_bool							mem_cmp(void *m1, void *m2, size_t *size);
size_t							str_len(char *str);
t_bool							str_2_uint(char *str, unsigned int *result);
t_bool							str_2_pat(char *str, t_pat *pat);

void							new_ping(t_icmp_pkt pkt);
t_err							ping_2_pong(uint16_t seq, t_elem **pong);
void							free_pngs(void);

long long						min_acc(t_elem *el);
long long						max_acc(t_elem *el);
long long						avg_acc(t_elem *el);
long long						mdev_acc(t_elem *el);
long long						ewma_acc(t_elem *el);
long long						fold_pongs(long long (*acc)(t_elem *));

void							sig_int(int signum);
void							sig_quit(int signum);

void							ping(int signum);
void							pong(void);

void							treat_error(t_ip_pkt *pkt);

t_bool							parse_arg(char **arg);
t_bool							flag_set(t_flag flg);
t_bool							opt_set(t_flag flg, t_optype typ, t_optval *val);

#endif //PROTO_H
