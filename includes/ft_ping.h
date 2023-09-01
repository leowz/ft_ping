/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:48:45 by zweng             #+#    #+#             */
/*   Updated: 2023/09/01 00:16:44 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H

# include "libft.h"
# include <stdio.h>
# include <sys/time.h>
# include <sys/socket.h>
# include <errno.h>

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

typedef struct	s_ping_stat
{
	double tmin;                  /* minimum round trip time */
	double tmax;                  /* maximum round trip time */
	double tsum;                  /* sum of all times, for doing average */
	double tsumsq;                /* sum of all times squared, for std. dev. */
}	t_ping_stat;

# define PEV_RESPONSE		0
# define PEV_DUPLICATE		1
# define PEV_NOECHO			2

# define PING_CKTABSIZE		128

/* The rationale for not exiting after a sending N packets is that we
   want to follow the traditional behaviour of ping.  */
# define DEFAULT_PING_COUNT 0

# define PING_HEADER_LEN	(ICMP_MINLEN)
# define PING_TIMING(s)		((s) >= sizeof (struct timeval))
# define PING_DATALEN		(64 - PING_HEADER_LEN)  /* default data length */

# define PING_DEFAULT_INTERVAL	1000      /* Milliseconds */
# define PING_PRECISION		1000     /* Millisecond precision */

# define MAX_IPOPTLEN		40

# define _PING_BUFLEN(p) 	(MAXIPLEN + (p)->ping_datalen + ICMP_TSLEN)

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

typedef struct s_ping
{
	int		ping_fd;
	int		ping_type;
	size_t	ping_count;
	struct	timeval ping_start_time;
	size_t	ping_interval;
	// ping_address to
	char	*ping_hostname;
	size_t	ping_datalen;
	int		ping_ident;
	// ping event;
	
	/* Runtime info */
	int		ping_cktab_size;
	char	*ping_cktab;

	unsigned char	*ping_buffer;
	// ping address from
	size_t	ping_num_xmit;
	size_t	ping_num_recv;
	size_t	ping_num_rept;
}			t_ping;

#define _C_BIT(p,bit)   (p)->ping_cktab[(bit)>>3]	/* byte in ck array */
#define _C_MASK(bit)    (1 << ((bit) & 0x07))
#define _C_IND(p,bit)   ((bit) % (8 * (p)->ping_cktab_size))

#endif
