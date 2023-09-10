/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 18:23:29 by zweng             #+#    #+#             */
/*   Updated: 2023/09/05 17:31:52 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static int print_echo(int dupflag, struct ping_stat *ping_stat,
					  struct sockaddr_in *dest, struct ip *ip,
					  icmphdr_t *icmp, int datalen);
static int handler(int code, void *closure, struct sockaddr_in *dest,
				   struct ip *ip, icmphdr_t *icmp, int datalen);
static void print_ip_opt(struct ip *ip, int hlen);

int ping_echo(char *hostname)
{
	int					status;
	struct ping_stat	ping_stat;
	t_ping				*ping;
	char				ip_readable[INET_ADDRSTRLEN];

	ping = g_prog.ping;
	ping_stat.tmin = 999999999.0;
	ping_stat.tmax = 0.0;
	ping->ping_type = ICMP_ECHO;
	ping->ping_datalen = g_prog.data_length;
	ping->ping_event = handler;
	ping->ping_closure = &ping_stat;
	if (ping_set_dest(ping, hostname))
		error(EXIT_FAILURE, 0, "unknown host");
	printf("PING %s (%s) %zu(%zu) bytes of data.", ping->ping_hostname,
		   inet_ntop(AF_INET, &ping->ping_dest.sin_addr, ip_readable,
					 sizeof(ip_readable)),
		   g_prog.data_length,
		   g_prog.data_length + PING_HEADER_LEN + IPV4_HEADER_LEN);
	if (g_prog.options & OPT_VERBOSE)
		printf(", id 0x%04x = %u", ping->ping_ident, ping->ping_ident);
	printf("\n");
	status = ping_run(ping, echo_finish);
	free(ping->ping_hostname);
	return (status);
}

int handler(int code, void *closure, struct sockaddr_in *dest,
			struct ip *ip, icmphdr_t *icmp, int datalen)
{
	if (code == PEV_RESPONSE || code == PEV_DUPLICATE)
	{
		print_echo(code == PEV_DUPLICATE, (struct ping_stat *)closure,
				   dest, ip, icmp, datalen);
	}
	return (0);
}

int print_echo(int dupflag, struct ping_stat *ping_stat,
			   struct sockaddr_in *dest, struct ip *ip,
			   icmphdr_t *icmp, int datalen)
{
	int 			hlen;
	struct timeval	tv;
	int 			timing;
	double			triptime;
	char			ip_readable[INET_ADDRSTRLEN];
	struct timeval	tv1, *tp;

	timing = 0;
	triptime = 0.0;
	gettimeofday(&tv, NULL);
	hlen = ip->ip_hl << 2;
	datalen -= hlen;
	if (PING_TIMING(datalen - PING_HEADER_LEN))
	{

		timing++;
		tp = (struct timeval *)icmp->icmp_data;
		ft_memcpy(&tv1, tp, sizeof(tv1));
		tvsub(&tv, &tv1);
		triptime = ((double)tv.tv_sec) * 1000.0 +
				   ((double)tv.tv_usec) / 1000.0;
		ping_stat->tsum += triptime;
		ping_stat->tsumsq += triptime * triptime;
		if (triptime < ping_stat->tmin)
			ping_stat->tmin = triptime;
		if (triptime > ping_stat->tmax)
			ping_stat->tmax = triptime;
	}
	if (g_prog.options & OPT_QUIET)
		return (0);
	if (g_prog.options & OPT_FLOOD)
	{
		ft_putchar('\b');
		return (0);
	}
	printf("%d bytes from %s: icmp_seq=%u", datalen,
		   inet_ntop(AF_INET, &dest->sin_addr.s_addr,
					 ip_readable, sizeof(ip_readable)),
		   ntohs(icmp->icmp_seq));
	printf(" ttl=%d", ip->ip_ttl);
	if (timing)
		printf(" time=%.3f ms", triptime);
	if (dupflag)
		printf(" (DUP!)");
	printf("\n");
	return (0);
}

int echo_finish(t_ping *p)
{
	struct ping_stat	*ping_stat;
	double				total, avg, vari;

	ping_finish(p);
	if (p->ping_num_recv && PING_TIMING(g_prog.data_length))
	{
		ping_stat = (struct ping_stat *)p->ping_closure;
		total = p->ping_num_recv + p->ping_num_rept;
		avg = ping_stat->tsum / total;
		vari = ping_stat->tsumsq / total - avg * avg;
		printf("rtt min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n",
			   ping_stat->tmin, avg, ping_stat->tmax, vari);
	}
	return (p->ping_num_recv == 0);
}
