/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/15 18:05:08 by fcadet            #+#    #+#             */
/*   Updated: 2022/08/31 09:05:45 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>

#include "const.h"
#include "struct.h"
#include "proto.h"
#include "icmp.h"

extern t_glob					glob;

#endif //HEADER_H
