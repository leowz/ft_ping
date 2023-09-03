/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 18:23:29 by zweng             #+#    #+#             */
/*   Updated: 2023/09/03 17:59:54 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int	ping_echo(char *hostname, t_prog *prog)
{
	char		rspace[MAX_IPOPTLEN];
	int			status;
	t_ping		*ping;

	ping = prog->ping;
	ping_set_type(ping, ICMP_ECHO);
	ping_set_packetsize(ping, prog->data_length);
//	ping_set_event_handler(ping, handler, &ping_stat);
	if (ping_set_dest(ping, hostname))
		error(EXIT_FAILURE, 0, "unknown, host");
	printf("PING %s (%s): %zu bytes of data.", ping->ping_hostname,
			inet_ntoa(ping->ping_dest.sin_addr), prog->data_length);
	if (prog->options & OPT_VERBOSE)
		printf(", id 0x%04x = %u", ping->ping_ident, ping->ping_ident);
	printf("\n");
	status = ping_run(ping, echo_finish, prog);
	free(ping->ping_hostname);
	return (status);
}

/*int	handler(int code, void *closure, struct sockaddr_in *dest,
		struct sockaddr_in *from, struct ip *ip, icmphdr_t *icmp, int datalen)
{
}*/

int	echo_finish(t_ping *p, t_prog *prog)
{
	struct ping_stat	*ping_stat;
	double				total, avg, vari;

	ping_finish(p);
	if (p->ping_num_recv && PING_TIMING(prog->data_length))
	{
		ping_stat = (struct ping_stat *)p->ping_closure;
		total = p->ping_num_recv + p->ping_num_rept;
		avg = ping_stat->tsum / total;
		vari = ping_stat->tsumsq / total - avg * avg;
		printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
				ping_stat->tmin, avg, ping_stat->tmax, vari);
	}
	return (p->ping_num_recv == 0);
}

