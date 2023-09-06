/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:48:45 by zweng             #+#    #+#             */
/*   Updated: 2023/09/05 16:05:52 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H

# include <stdio.h>
# include <signal.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <errno.h>
# include <netinet/in_systm.h>
# include <netinet/in.h>
# include <netinet/ip.h>
# include <netinet/icmp6.h>
# include <arpa/inet.h>
# include <sys/types.h>
# include <netdb.h>
# include "libft.h"
# include "icmp.h"

# define MAXWAIT			10	/* Max seconds to wait for response.  */
# define MAXPATTERN			16	/* Maximal length of pattern.  */

# define ICMP				1	/* ICMP protocal number  */
# define ICMP_ECHO			8
# define ICMP_MINLEN		8

# define OPT_FLOOD			0x001
# define OPT_INTERVAL		0x002
# define OPT_NUMERIC		0x004
# define OPT_QUIET			0x008
# define OPT_RROUTE			0x010
# define OPT_VERBOSE		0x020
# define OPT_IPTIMESTAMP	0x040
# define OPT_FLOWINFO		0x080
# define OPT_TCLASS			0x100
# define OPT_HELP			-1

# define SOPT_TSONLY		0x001
# define SOPT_TSADDR		0x002
# define SOPT_TSPRESPEC		0x004

# define AI_CANONNAME		0x0002

struct	ping_stat
{
	double tmin;                  /* minimum round trip time */
	double tmax;                  /* maximum round trip time */
	double tsum;                  /* sum of all times, for doing average */
	double tsumsq;                /* sum of all times squared, for std. dev. */
};

# define PEV_RESPONSE		0
# define PEV_DUPLICATE		1
# define PEV_NOECHO			2

# define PING_CKTABSIZE		128

/* The rationale for not exiting after a sending N packets is that we
   want to follow the traditional behaviour of ping.  */
# define DEFAULT_PING_COUNT 0

# define IPV4_HEADER_LEN	20
# define PING_HEADER_LEN	(ICMP_MINLEN)
# define PING_TIMING(s)		((s) >= sizeof (struct timeval))
# define PING_DATALEN		(64 - PING_HEADER_LEN)  /* default data length */

# define PING_DEFAULT_INTERVAL	1000      /* Milliseconds */
# define PING_PRECISION		1000     /* Millisecond precision */

# define MAX_IPOPTLEN		40

# define _PING_BUFLEN(p) 	(MAXIPLEN + (p)->ping_datalen + ICMP_TSLEN)


typedef struct s_prog	t_prog;
typedef int (*ping_efp) (int code, void *closure, struct sockaddr_in * dest,
			struct ip * ip, icmphdr_t * icmp,
			int datalen, t_prog *p);

typedef struct s_ping
{
	int				ping_fd;
	int				useless_ident;
	int				ping_type;
	size_t			ping_count;
	struct timeval	ping_start_time;
	size_t			ping_interval;
	struct sockaddr_in	ping_dest;
	char			*ping_hostname;
	size_t			ping_datalen;
	int				ping_ident;
	ping_efp		ping_event;
	void			*ping_closure;
	
	/* Runtime info */
	int				ping_cktab_size;
	char			*ping_cktab;

	unsigned char	*ping_buffer;
	size_t			ping_num_xmit;
	size_t			ping_num_recv;
	size_t			ping_num_rept;
}			t_ping;

typedef struct s_prog
{
	char			*prog_name;
	int				options;
	int				one;
	int				status;
	unsigned char	*data_buffer;
	size_t			data_length;
	t_ping			*ping;
}					t_prog;

#define _C_BIT(p,bit)   (p)->ping_cktab[(bit)>>3]	/* byte in ck array */
#define _C_MASK(bit)    (1 << ((bit) & 0x07))
#define _C_IND(p,bit)   ((bit) % (8 * (p)->ping_cktab_size))

t_ping		*ping_init(int type, int ident);
void		ping_set_type(t_ping *p, int type);
void		ping_set_packetsize(t_ping *p, size_t size);
void		ping_set_interval(struct timeval *intvl, size_t ping_interval);
struct timeval	ping_get_resp_time(struct timeval last, struct timeval now, 
			struct timeval intvl);
void		_ping_set(t_ping *p, size_t bit);
void		_ping_clr(t_ping *p, size_t bit);
int			_ping_tst(t_ping *p, size_t bit);
int			ping_emit(t_ping *p);
int			ping_recv(t_ping *p, t_prog *prog);
void		ping_set_sockopt(t_ping *ping, int opt_name, void *val,
			int valsize);
void		init_data_buffer(t_prog *prog);
int			_ping_setbuf(t_ping *p);
int			ping_set_data(t_ping *p, void *data, size_t off, size_t len);
int			ping_echo(char *hostname, t_prog *prog);
void		ping_reset(t_ping *p);
int			send_echo(t_ping *ping, t_prog *prog);
int			ping_run(t_ping *ping, int (*finish)(t_ping *p, t_prog *g),
			t_prog *prog);
int			ping_finish(t_ping *p);
void		ping_unset_data(t_ping *p);
int			ping_set_dest(t_ping *p, const char *host);
void    	ping_set_event_handler (t_ping *p, ping_efp pf, void *closure);
int			echo_finish(t_ping *p, t_prog *prog);
void		error(int status, int errnum, const char *msg);
void		tvsub(struct timeval *out, struct timeval *in);
#endif
