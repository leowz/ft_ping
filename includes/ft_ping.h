/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ping.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:48:45 by zweng             #+#    #+#             */
/*   Updated: 2023/08/27 18:49:04 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PING_H
#define FT_PING_H

# include "libft.h"
# include <stdio.h>
# include <sys/time.h>

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
}	t_ping;


#endif
