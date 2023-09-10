/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:47:14 by zweng             #+#    #+#             */
/*   Updated: 2023/09/05 14:29:44 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

t_prog		g_prog;

static void	usage()
{
	printf("\nUsage\n");
	printf("	ping [options] <destination>\n\n");
	printf("options:\n");
	printf("	<destination>		dns name or ip address\n");
	printf("	-h			print help and exit\n");
	printf("	-v			verbose output\n");
}

static void	parse_opt(int key, int *options)
{
	switch (key)
	{
		case 'v':
			*options |= OPT_VERBOSE;
			break ;
		case 'q':
			*options |= OPT_QUIET;
			break ;
		case 'h':
			*options |= OPT_HELP;
			break ;
		case '?':
			*options |= OPT_HELP;
			break ;
		default:
			break ;
	}
}

static void	arg_parse(char **av, int *index, int *options,
		void (*parse_opt)(int key, int *options))
{
	int	i;
	
	i = 1;
	while (av[i] && av[i][1] == '-' && ft_strlen(av[i]) > 1)
	{
		(*parse_opt)(av[i][2], options);
		i++;
	}
	*index = i;
}

int	main(int ac, char **av)
{
	int		index;
	t_ping	*ping;

	ft_memset(&g_prog, 0, sizeof(g_prog));
	g_prog.one = 1;
	g_prog.prog_name = av[0];
	arg_parse(av, &index, &g_prog.options, &parse_opt);
	if (ac < 2 || g_prog.options < 0)
		usage();
	ping = ping_init(ICMP_ECHO, getpid());
	if (ping == NULL)
		return (EXIT_FAILURE);
	ping_set_sockopt(ping, SO_BROADCAST, (char *)&g_prog.one, sizeof(g_prog.one));
	ac -= index;
	av += index;
	init_data_buffer();
	if (g_prog.data_buffer == NULL)
	{
		free(ping);
		return (EXIT_FAILURE);
	}
	g_prog.ping = ping;
	while (ac)
	{
		g_prog.status |= ping_echo(*av);
		ping_reset(ping);
		ac--;
		av++;
	}
	free(g_prog.ping);
	free(g_prog.data_buffer);
	return (g_prog.status);
}

void	sig_int(int signal)
{
	g_prog.stop = 1;
}

int	ping_run(t_ping *ping, int (*finish)(t_ping *p))
{
	int				finishing;
	size_t			i;	
	int				n;

	finishing = 0;
	signal(SIGINT, sig_int);
	while (!g_prog.stop)
	{
		if (!ping->ping_count || ping->ping_num_xmit < ping->ping_count)
		{
			send_echo(ping);
			if (!(g_prog.options & OPT_QUIET) && (g_prog.options & OPT_FLOOD))
				ft_putchar('.');
		}
		else
			break ;
		ping_recv(ping);
		usleep(ping->ping_interval * 1000);
	}
	ping_unset_data(ping);
	if (finish)
		return finish(ping);
	return (0);
}

int	send_echo(t_ping *ping)
{
	size_t			off;
	int				rc;
	struct timeval	tv;

	off = 0;
	if (PING_TIMING(g_prog.data_length))
	{
		gettimeofday(&tv, NULL);
		ping_set_data(ping, &tv, 0, sizeof(tv));
		off += sizeof(tv);
	}
	if (g_prog.data_buffer)
	{
		ping_set_data(ping, g_prog.data_buffer, off,
			g_prog.data_length > off ? g_prog.data_length - off :
			g_prog.data_length);
	}
	rc = ping_emit(ping);
	if (rc < 0)
		error(EXIT_FAILURE, errno, "sending packet");
	return (rc);
}

int	ping_finish(t_ping *ping)
{
	printf("--- %s ping statistics ---\n", ping->ping_hostname);
	printf("%zu packets transmitted, ", ping->ping_num_xmit);
	printf("%zu packets received, ", ping->ping_num_recv);
	if (ping->ping_num_rept)
		printf("+%zu duplicateds, ", ping->ping_num_rept);
	if (ping->ping_num_xmit)
	{	
		if (ping->ping_num_recv > ping->ping_num_xmit)
			printf("-- somebody is printing forged packets!");
		else
			printf("%d%% packet loss", (int)(((ping->ping_num_xmit
				- ping->ping_num_recv) * 100) / ping->ping_num_xmit));
	}
	printf("\n");
	return (0);
}
