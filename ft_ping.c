/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcadet <fcadet@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 13:52:47 by fcadet            #+#    #+#             */
/*   Updated: 2022/02/12 13:57:18 by fcadet           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

struct addrinfo		create_hints(void) {
	struct addrinfo		hints = { 0 };

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_RAW;
	hints.ai_protocol = IPPROTO_ICMP;
	hints.ai_flags = 0;
	return (hints);
}

int	main(int argc, char **argv) {
	if (argc != 2) {
		printf("Error: Wrong number of arguments\n");
		return (1);
	} else if (getuid()) {
		printf("Error: not allowed\n");
		return (2);
	}

	struct in_addr		ip = { 0 };
	struct addrinfo		hints = create_hints();
	struct addrinfo		*inf = NULL;
	struct sockaddr_in	soc_ad_in = { 0 };
	struct sockaddr		*soc_ad = (struct sockaddr *)&soc_ad_in;
	char				buff[INET_ADDRSTRLEN] = { 0 };
	int					sock = 0;

	if (inet_pton(AF_INET, argv[1], &ip) == 1) {
		soc_ad_in.sin_family = AF_INET;
		soc_ad_in.sin_addr = ip;
		inet_ntop(AF_INET, &ip, buff, INET_ADDRSTRLEN);
	} else if ((getaddrinfo(argv[1], NULL, &hints, &inf) == 0)) {
		soc_ad_in = *((struct sockaddr_in *)inf->ai_addr);
		freeaddrinfo(inf);
		inet_ntop(AF_INET,
				&soc_ad_in.sin_addr,
				buff, INET_ADDRSTRLEN);
	} else  {
		printf("Error: Invalid domain\n");
		return (1);
	}
	if ((sock = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
		printf("Error: Can't create socket\n");
		return (3);
	}
	printf("PING %s (%s) X(X) bytes of data.\n",
			inf ? argv[1] : buff, buff);

	sendto(sock, "bonjour", 8, 0, soc_ad, sizeof(struct sockaddr));
}
