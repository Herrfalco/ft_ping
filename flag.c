/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   flag.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 19:58:36 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/25 13:39:43 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

t_bool		add_flag(char *arg) {
	t_bool		found;
	size_t		flgs = 0;

	while (*++arg) {
		found = FALSE;
		for (size_t i = 0; i < str_len(FLGS); ++i) {
			if (*arg == FLGS[i]) {
				if (flag_set(i))
					error(E_ARG, "Command line", "Duplicated argument", arg);
				flgs |= 0x1 << i;
				found = TRUE;
				break;
			}
		}
		if (!found)
			return (FALSE);
	}
	glob.args.flags |= flgs;
	return (TRUE);
}

t_bool		add_opt(char ***arg) {
	unsigned int		val;

	if (str_len(**arg) != OPT_SZ)
		return (FALSE);
	for (size_t i = 0; i < str_len(OPTS); ++i) {
		if ((**arg)[1] == OPTS[i]) {
			if (opt_set(i, NULL))
				error(E_ARG, "Command line", "Duplicated argument", **arg);
			if (!*(*arg + 1))
				error(E_ARG, "Command line", "Need value for option", **arg);
			if (str_2_uint(*(*arg + 1), &val))
				error(E_ARG, "Command line", "Bad value for option", **arg);
			++(*arg);
			glob.args.opts_flags |= 0x1 << i;
			glob.args.opts[i] = val;
			return (TRUE);
		}
	}
	return (FALSE);
}

void		parse_arg(char **arg) {
	for (; *arg; ++arg) {
		if (**arg != '-') {
			if (*(arg + 1))
				error(E_ARG, "Command line", "Unrecognized argument", *arg);
			return;
		}
		if (str_len(*arg) < OPT_SZ || (!add_flag(*arg) && !add_opt(&arg)))
			error(E_ARG, "Command line", "Unrecognized argument", *arg);
	}
}

t_bool		flag_set(t_flag flg) {
	return (!!(glob.args.flags & (0x1 << flg)));
}

t_bool		opt_set(t_flag flg, unsigned int *val) {
	if (glob.args.opts_flags & (0x1 << flg)) {
		if (val)
			*val = glob.args.opts[flg];
		return (TRUE);
	}
	return (FALSE);
}

void		disp_help(void) {
	fprintf(stderr, "%s", HELP_TXT);
	exit(0);
}
