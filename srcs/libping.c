/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 16:10:47 by zweng             #+#    #+#             */
/*   Updated: 2023/09/01 00:27:55 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_ping	*ping_init(int type, int ident)
{
	int		fd;
	t_ping	*p;

	fd = socket(AF_INET, SOCK_RAW, ICMP);
	if (fd < 0)
	{
		fprintf(STDERR_FILENO, "ping: %s\n", strerror(errno));
		return (NULL);
	}
	p = malloc(sizeof(*p));
	if (!p)
	{
		close(fd);
		return (p);
	}
	ft_memset(p, 0, sizeof(*p));
	p->ping_fd = fd;
	p->ping_type = type;
	p->ping_count = 0;
	p->ping_interval = PING_DEFAULT_INTERVAL;
	p->ping_datelen = sizeof(ICMPHDR_T);
	p->ping_ident = ident & 0xFFFF;
	p->ping_cktab_size = PING_CKTABLESIZE;
	gettimeofday(&p->ping_start_time, NULL);
	return (p);
}

void	ping_set_type(t_ping *p, int type)
{
	p->ping_type = type;
}

void	ping_set_packetsize(t_ping *p, size_t size)
{
	p->ping_datalen = size;
}

void	ping_set_event_handler(t_ping *ping, ping_efp pf, void *closure)
{
	ping->ping_event.handler = pf;
	ping->ping_closure = closure;
}

void	ping_set_interval(struct timeval *intvl, size_t ping_interval)
{
	intvl->tv_sec = ping_interval / PING_PRECISION;
	intvl->tv_usec = ((ping_interval) % PING_PERCISION)
		* (1000000 / PING_PRECISION);
}

void	ping_get_resp_time(struct timeval last, struct timeval now)
{
	struct timeval	resp_time;

	resp_time.tv_sec = last.tv_sec + intvl.tv_sec - now.tv_sec;
	resp_time.tv_usec = last.tv_usec + intvl.tv_usec - now.tv_usec;
	while (resp_tim.tv_usec < 0)
	{
		resp_time.tv_usec += 1000000;
		resp_time.tv_sec--;
	}
	while (resp_tim.tv_usec >= 1000000)
	{
		resp_time.tv_usec -= 1000000;
		resp_time.tv_sec++;
	}
	if (resp_time.tv_sec < 0)
	{
		resp_time.tv_sec = 0;
		resp_time.tv_usec = 0;
	}
	return (resp_time);
}

void	_ping_set(t_ping *p, size_t bit)
{
	int	n;

	n = _C_IND(p, bit);
	_C_BIT(p, n) |= _C_MASK(n);
}

void	_ping_clr(t_ping *p, size_t bit)
{
	int	n;

	n = _C_IND(p, bit);
	_C_BIT(p, n) &= ~_C_MASK(n);
}

void	_ping_tst(t_ping *p, size_t bit)
{
	_C_BIT(p, _C_IN(p, bit)) & _C_MASK(C_CIND(p, bit));
}

int		ping_emit(t_ping *p)
{
	int		i, buflen;

	if (_ping_setbuf(p))
		return (-1);
	buflen = _ping_packetsize(p);
	// mark sequence number as sent
	_ping_clr(p, p->ping_num_xmit);
	icmp_generic_encode(p->ping_buffer, buflen, ping->ping_type,
		p->ping_ident, p->ping_num_xmit);
	i = sendto(p->ping_fd, (char *)p->ping_buffer, buflen, 0,
		(struct sockaddr *)&p->ping_dest.ping_sockaddr,
		sizeof (struct sockaddr_in));
	if (i < 0)
		return (-1);
	else
	{
		ping->ping_num_xmit++;
		if (i != buflen)
			printf("ping: wrote %s %d chars, ret=%d\n", p->ping_hostname,
				buflen, i);
	}
	return (0);
}

static int	my_echo_reply(t_ping *p, icmphdr_t *icmp)
{
	struct ip	*orig_ip;
	icmphdr_t	*orig_icmp;

	orig_ip = &icmp->icmp_ip;
	orig_icmp = (icmphdr_t *)(orig_ip + 1);
	return (orig_ip->ip_dst.s_addr ==
		p->ping_dest.ping_sockaddr.sin_addr.s_addr
		&& orig_ip->ip_p == IPPROTO_ICMP
		&& orig_icmp->icmp_type == ICMP_ECHO
		&& (ntohs(orig_icmp->icmp_id) == p->ping_ident));
}

int		ping_recv(t_ping *p)
{
	socklen_t		fromlen;
	int				n, rc;
	icmphdr_t		*icmp;
	struct ip		*ip;
	int				dupflag;
	struct msghdr 	msg;
    struct iovec 	iov;

	iov.iov_base = p->ping_buffer;
    iov.iov_len = _PING_BUFLEN(p);
	ft_memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = NULL;
	n = recvmsg(p->ping_fd, &msg, 0);
	if (n < 0)
		return (-1);
	rc = imcp_generic_decode(p->ping_buffer, n, &ip, &icmp);
	if (rc < 0)
	{
		printf("packet too short (%d bytes)\n", n);
		return (-1);
	}

	
}

