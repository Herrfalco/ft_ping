/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   arg.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/23 19:58:36 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/27 13:42:56 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../hdrs/header.h"

static t_bool		add_flag(char *arg) {
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

static t_bool		add_opt(char ***arg) {
	t_bool		err = FALSE;

	if (str_len(**arg) != OPT_SZ)
		return (FALSE);
	for (size_t i = 0; i < str_len(OPTS); ++i) {
		if ((**arg)[1] == OPTS[i]) {
			if (opt_set(i, T_ANY, NULL))
				error(E_ARG, "Command line", "Duplicated argument", **arg);
			if (!*(*arg + 1))
				error(E_ARG, "Command line", "Need value for option", **arg);
			switch (i) {
				case O_P:
					err = str_2_pat(*(*arg + 1), &glob.args.opts[i].pat);
					break;
				default:
					err = str_2_uint(*(*arg + 1), &glob.args.opts[i].uint);
			}
			if (err)
				error(E_ARG, "Command line", "Bad value for option", **arg);
			glob.args.opts_flags |= 0x1 << i;
			++(*arg);
			return (TRUE);
		}
	}
	return (FALSE);
}

t_bool		parse_arg(char **arg) {
	for (; *arg; ++arg) {
		if (**arg != '-') {
			if (*(arg + 1))
				error(E_ARG, "Command line", "Unrecognized argument", *arg);
			return (FALSE);
		}
		if (str_len(*arg) < OPT_SZ || (!add_flag(*arg) && !add_opt(&arg)))
			error(E_ARG, "Command line", "Unrecognized argument", *arg);
	}
	return (TRUE);
}

t_bool		flag_set(t_flag flg) {
	return (!!(glob.args.flags & (0x1 << flg)));
}

t_bool		opt_set(t_flag flg, t_optype typ, t_optval *val) {
	if (glob.args.opts_flags & (0x1 << flg)) {
		switch (typ) {
			case T_UINT:
				val->uint = glob.args.opts[flg].uint;
				break;
			case T_PAT:
				val->pat = glob.args.opts[flg].pat;
				break;
			default:
				break;
		}
		return (TRUE);
	}
	return (FALSE);
}
