/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 19:58:36 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/24 12:37:51 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

void		add_flag(char *arg, t_bool err) {
	if ((str_len(arg) != FLG_SZ || arg[0] != '-') && err)
		error(E_ARG, "Command line", "Unrecognized argument", arg);
	for (size_t i = 0; i < str_len(FLGS); ++i) {
		if (arg[1] == FLGS[i]) {
			if (is_set(i))
				error(E_ARG, "Command line", "Duplicated argument", arg);
			glob.flags |= 0x1 << i;
			return;
		}
	}
	if (err)
		error(E_ARG, "Command line", "Unrecognized argument", arg);
}

t_bool		is_set(t_flag flg) {
	return (glob.flags & (0x1 << flg));
}

void		disp_help(void) {
	printf("%s", HELP_TXT);
	exit(0);
}
