/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/15 14:47:14 by zweng             #+#    #+#             */
/*   Updated: 2023/08/27 18:48:51 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

static void	usage()
{
	printf("\nUsage\n");
	printf("	ping [options] <destination>\n\n");
	printf("options:\n");
	printf("	<destination>		dns name or ip address\n");
	printf("	-h			print help and exit\n");
	printf("	-v			verbose output\n");
}

int	main(int ac, char **av)
{
	if (ac < 2)
		usage();
	return (0);
}
