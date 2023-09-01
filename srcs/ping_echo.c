/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 18:23:29 by zweng             #+#    #+#             */
/*   Updated: 2023/08/31 17:21:14 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int	ping_echo(char *hostname, t_prog *prog)
{
	char		rspace[MAX_IPOPTLEN];
	t_ping_stat	ping_stat;
	int			status;
	t_ping		*ping;

	ping = prog->ping;
	ft_memset(&ping_stat, 0, sizeof(ping_stat));
	ping_stat.tmin = 999999999.0;
	ping_set_type(ping, ICMP_ECHO);
	ping_set_packetsize(ping, date_length);
//	ping_set_event_handler(ping, handler, &ping_stat);
	if (ping_set_dest(ping, hostname))
		error(EXIT_FAILURE, 0, "unknown, host");
	printf("PING %s (%s): %zu data bytes", ping->ping_hostname, 
			inet_ntoa(ping->ping_dest.ping_sockaddr.sin_addr),
			prog->data_length);
	if (prog->options & OPT_VERBOSE)
		printf(", id 0x%04x = %u", ping->ping_ident, ping->pint_ident);
	printf("\n");
	status = ping_run(ping, echo_finish, prog);
	free(ping->ping_hostname);
	return (status);
}

/*int	handler(int code, void *closure, struct sockaddr_in *dest,
		struct sockaddr_in *from, struct ip *ip, icmphdr_t *icmp, int datalen)
{


}*/

