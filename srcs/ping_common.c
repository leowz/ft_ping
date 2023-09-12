/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ping_common.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 17:10:07 by zweng             #+#    #+#             */
/*   Updated: 2023/09/04 18:24:42 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	ping_set_sockopt(t_ping *ping, int opt_name, void *val, int valsize)
{
	setsockopt(ping->ping_fd, SOL_SOCKET, opt_name, (char *)&val, valsize);
}

void	init_data_buffer()
{
	size_t			i;

	g_prog.data_length = PING_DATALEN;
	g_prog.data_buffer = malloc(g_prog.data_length);
	if (!g_prog.data_buffer)
	{
		printf("%s\n", strerror(errno));
		return ;
	}
	i = 0;
	while (i < g_prog.data_length)
	{
		g_prog.data_buffer[i] = i;
		i++;
	}
}

int		_ping_setbuf(t_ping *p)
{
	if (!p->ping_buffer)
	{	
		p->ping_buffer = malloc(_PING_BUFLEN(p));
		if (!p->ping_buffer)
			return (-1);
	}
	if (!p->ping_cktab)
	{	
		p->ping_cktab = malloc(p->ping_cktab_size);
		if (!p->ping_cktab)
			return (-1);
		ft_memset(p->ping_cktab, 0, p->ping_cktab_size);
	}
	return (0);
}

int		ping_set_data(t_ping *p, void *data, size_t off, size_t len)
{
	icmphdr_t	*icmp;

	if (_ping_setbuf(p))
		return (-1);
	if (p->ping_datalen < off + len)
		return (-1);
	icmp = (icmphdr_t *)p->ping_buffer;
	ft_memcpy(icmp->icmp_data + off, data, len);
	return (0);
}

void	_ping_freebuf(t_ping *p)
{
	if (p->ping_buffer)
	{
		free(p->ping_buffer);
		p->ping_buffer = NULL;
	}
	if (p->ping_cktab)
	{
		free(p->ping_cktab);
		p->ping_cktab = NULL;
	}
}

void	ping_unset_data(t_ping *p)
{
	_ping_freebuf(p);
}

void tvsub (struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

double nabs (double a)
{
  return (a < 0) ? -a : a;
}

double nsqrt (double a, double prec)
{
  double x0, x1;

  if (a < 0)
    return 0;
  if (a < prec)
    return 0;
  x1 = a / 2;
  do
    {
      x0 = x1;
      x1 = (x0 + a / x0) / 2;
    }
  while (nabs (x1 - x0) > prec);

  return x1;
}