/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libping.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 16:10:47 by zweng             #+#    #+#             */
/*   Updated: 2023/09/05 14:21:23 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

size_t	_ping_packetsize (t_ping *p)
{
  return PING_HEADER_LEN + p->ping_datalen;
}

t_ping	*ping_init(int type, int ident)
{
	int		fd;
	t_ping	*p;
	int		useless_ident;

	useless_ident = 0;
	fd = socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK, ICMP);
	if (fd < 0)
	{
		fd = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, ICMP);
		if (fd < 0)
		{
			printf("ping: %s\n", strerror(errno));
			return (NULL);
		}
		useless_ident++;
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
	p->ping_datalen = sizeof(icmphdr_t);
	p->ping_ident = ident & 0xFFFF;
	p->useless_ident = useless_ident;
	p->ping_cktab_size = PING_CKTABSIZE;
	gettimeofday(&p->ping_start_time, NULL);
	return (p);
}

void	ping_set_interval(struct timeval *intvl, size_t ping_interval)
{
	intvl->tv_sec = ping_interval / PING_PRECISION;
	intvl->tv_usec = ((ping_interval) % PING_PRECISION)
		* (1000000 / PING_PRECISION);
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

int		_ping_tst(t_ping *p, size_t bit)
{
	return (_C_BIT(p, _C_IND(p, bit)) & _C_MASK(_C_IND(p, bit)));
}

int		ping_emit(t_ping *p)
{
	int		i, buflen;

	if (_ping_setbuf(p))
		return (-1);
	buflen = _ping_packetsize(p);
	// mark sequence number as sent
	_ping_clr(p, p->ping_num_xmit);
	icmp_generic_encode(p->ping_buffer, buflen, p->ping_type,
		p->ping_ident, p->ping_num_xmit);
	i = sendto(p->ping_fd, (char *)p->ping_buffer, buflen, 0,
		(struct sockaddr *)&p->ping_dest,
		sizeof (struct sockaddr_in));
	if (i < 0)
		return (-1);
	else
	{
		p->ping_num_xmit++;
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
		p->ping_dest.sin_addr.s_addr
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
	rc = icmp_generic_decode(p->ping_buffer, n, &ip, &icmp);
	if (rc < 0)
	{
		printf("packet too short (%d bytes)\n", n);
		return (-1);
	}
	if (icmp->icmp_type == ICMP_ECHOREPLY)
	{
		if (ntohs(icmp->icmp_id) != p->ping_ident && p->useless_ident == 0)
			return (-1);
		if (rc)
			printf("checksum mismatch\n");
		p->ping_num_recv++;
		if (_ping_tst(p, ntohs(icmp->icmp_seq)))
		{
			p->ping_num_rept++;
			p->ping_num_recv--;
			dupflag = 1;
		}
		else
		{
			_ping_set(p, ntohs(icmp->icmp_seq));
			dupflag = 0;
		}
		if (p->ping_event)
			p->ping_event(dupflag ? PEV_DUPLICATE : PEV_RESPONSE,
				p->ping_closure, &p->ping_dest, ip, icmp, n);
	}
	else if (icmp->icmp_type == ICMP_ECHO)
		return (-1);
	else
	{
		if (!my_echo_reply(p, icmp))
			return (-1);
	}
	return (0);
}

void	ping_reset(t_ping *p)
{
  p->ping_num_xmit = 0;
  p->ping_num_recv = 0;
  p->ping_num_rept = 0;
}

int		ping_set_dest(t_ping *p, const char *host)
{
	int rc;
	struct addrinfo hints, *res;

	ft_memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_flags = AI_CANONNAME;
	rc = getaddrinfo(host, NULL, &hints, &res);
	if (rc)
		return (1);
	ft_memcpy(&p->ping_dest, res->ai_addr, res->ai_addrlen);
	if (res->ai_canonname)
		p->ping_hostname = ft_strdup(res->ai_canonname);
	else
		p->ping_hostname = ft_strdup(host);
	freeaddrinfo(res);
	return (0);
}
