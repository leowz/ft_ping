/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/30 16:13:51 by zweng             #+#    #+#             */
/*   Updated: 2023/09/01 18:11:53 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ping.h"

void	error(int status, int errnum, const char *msg)
{
	printf("%s\n", msg);
	if (errnum)
		printf("ft_ping: %s", strerror(errnum));
	if (status)
		exit(status);
}
