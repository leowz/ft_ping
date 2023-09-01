/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:47:14 by zweng             #+#    #+#             */
/*   Updated: 2023/09/01 18:52:51 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

int	stop = 0;

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
	t_prog	prog;
	int		index;
	t_ping	*ping;

	ft_memset(&prog, 0, sizeof(prog));
	prog.one = 1;
	prog.prog_name = av[0];
	arg_parse(av, &index, &prog.options, &parse_opt);
	if (ac < 2 || prog.options < 0)
		usage();
	ping = ping_init(ICMP_ECHO, getpid());
	// set broadcast option at socket level at value 1
	ping_set_sockopt(ping, SO_BROADCAST, (char *)&prog.one, sizeof(prog.one));
	ac -= index;
	av += index;
	init_data_buffer(&prog);
	if (ping == NULL || prog.data_buffer == NULL)
		return (EXIT_FAILURE);
	prog.ping = ping;
	while (ac)
	{
		prog.status |= ping_echo(*av, &prog);
		ping_reset(ping);
		ac--;
		av++;
	}
	free(ping);
	free(prog.data_buffer);
	return (prog.status);
}

void	sig_int(int signal)
{
	stop = 1;
}

int	ping_run(t_ping *ping, int (*finish)(t_ping *p), t_prog *prog)
{
	struct timeval	resp_time, last, intvl, now;
	struct timeval	*t;
	int				finishing;
	size_t			nresp, i;	
	int				n;

	t = NULL;
	finishing = 0;
	nresp = 0;
	signal(SIGINT, sig_int);
	ft_memset(&resp_time, 0, sizeof(resp_time));
	ft_memset(&intvl, 0, sizeof(intvl));
	ft_memset(&now, 0, sizeof(now));
	if (prog->options & OPT_FLOOD)
	{
		intvl.tv_sec = 0;
		intvl.tv_usec = 10000;
	}
	else
		ping_set_interval(&intvl, ping->ping_interval);
	gettimeofday(&last, NULL);
	send_echo(ping, prog);
	while (!stop)
	{
		gettimeofday(&now, NULL);
		resp_time = ping_get_resp_time(last, now, intvl);
		//block to receive
		if (ping_recv(ping) == 0)
			nresp++;
		if (t == 0)
		{
			gettimeofday(&now, NULL);
			t = &now;
		}
		// check -w waittime, -c count
		if (!ping->ping_count || ping->ping_num_xmit < ping->ping_count)
		{
			// block to send
			send_echo(ping, prog);
			if (!(prog->options & OPT_QUIET) && (prog->options & OPT_FLOOD))
				ft_putchar('.');
			// check -w waittime
		}
		else
			break ;
		gettimeofday(&last, NULL);
	}
	ping_unset_data(ping);
	if (finish)
		return finish(ping);
	return (0);
}

int	send_echo(t_ping *ping, t_prog *prog)
{
	size_t			off;
	int				rc;
	struct timeval	tv;

	off = 0;
	if (PING_TIMING(prog->data_length))
	{
		gettimeofday(&tv, NULL);
		ping_set_data(ping, &tv, 0, sizeof(tv));
		off += sizeof(tv);
	}
	if (prog->data_buffer)
	{
		ping_set_data(ping, prog->data_buffer, off,
			prog->data_length > off ? prog->data_length - off :
			prog->data_length);
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
