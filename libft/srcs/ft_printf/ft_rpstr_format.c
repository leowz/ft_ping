/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_rpstr_format.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <marvin@42.fr>                       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/28 20:28:11 by zweng             #+#    #+#             */
/*   Updated: 2022/12/16 19:55:55 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static void	pf_handle_sign(char *str)
{
	int		i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '+' || str[i] == '-' || str[i] == ' ' || str[i] == 'x'
			|| str[i] == 'X')
			break ;
		i++;
	}
	if ((str[i] == '+' || str[i] == '-' || str[i] == ' ') && i > 0)
	{
		str[0] = str[i];
		str[i] = '0';
	}
	if ((str[i] == 'X' || str[i] == 'x') && i > 1)
	{
		str[1] = str[i];
		str[i] = '0';
	}
}

char	*pf_padding(char *rawstr, size_t len, t_formatph forma)
{
	char	*ret;
	char	tmp;

	ret = ft_strnew(forma.fieldwidth);
	if (!ret)
		return (NULL);
	if ((forma.flags & PFFL_MI) == PFFL_MI)
	{
		ft_memset(ret, ' ', forma.fieldwidth);
		ft_memcpy(ret, rawstr, len);
	}
	else
	{
		tmp = ' ';
		if ((forma.flags & PFFL_ZR) == PFFL_ZR && (forma.precision == PRC_NO
				|| forma.type > PFTP_CX || forma.type < PFTP_D))
			tmp = '0';
		ft_memset(ret, tmp, forma.fieldwidth);
		ft_memcpy(ret + (forma.fieldwidth - len), rawstr, len);
		if (tmp == '0' && ((forma.type >= PFTP_D && forma.type <= PFTP_CX)
				|| forma.type == PFTP_P))
			pf_handle_sign(ret);
	}
	ft_strdel(&rawstr);
	return (ret);
}

char	*ft_handle_format_uint(char *rawstr, t_formatph forma)
{
	int		len;

	len = ft_strlen(rawstr);
	if ((forma.type == PFTP_X || forma.type == PFTP_CX)
		&& (forma.flags & PFFL_HT) == PFFL_HT && (ft_strcmp(rawstr, "0")
			|| forma.type == PFTP_P) && ft_strcmp(rawstr, ""))
	{
		rawstr = pf_add_prefix_str(rawstr,
				ft_ternary(forma.type == PFTP_CX, "0X", "0x"));
		len += 2;
	}
	if (forma.type == PFTP_P)
	{
		rawstr = pf_add_prefix_str(rawstr, "0x");
		len += 2;
	}
	if ((forma.type == PFTP_O || forma.type == PFTP_CO)
		&& (forma.flags & PFFL_HT) == PFFL_HT && *rawstr != '0')
	{
		rawstr = pf_add_prefix_str(rawstr, "0");
		len += 1;
	}
	if (forma.fieldwidth > len)
		return (pf_padding(rawstr, len, forma));
	return (rawstr);
}

char	*ft_handle_format_int(char *rawstr, intmax_t val, t_formatph forma)
{
	int		len;

	len = ft_strlen(rawstr);
	if ((forma.flags & PFFL_PL) == PFFL_PL)
	{
		rawstr = pf_add_prefix_str(rawstr, ft_ternary(val >= 0, "+", "-"));
		len += 1;
	}
	else if ((forma.flags & PFFL_SP) == PFFL_SP)
	{
		rawstr = pf_add_prefix_str(rawstr, ft_ternary(val >= 0, " ", "-"));
		len += 1;
	}
	else if (val < 0)
	{
		rawstr = pf_add_prefix_str(rawstr, "-");
		len += 1;
	}
	if (forma.fieldwidth > len)
		return (pf_padding(rawstr, len, forma));
	return (rawstr);
}

char	*ft_handle_format_charstr(char *rawstr, size_t len,
		t_formatph forma)
{
	if (forma.fieldwidth > (int)len)
		return (pf_padding(rawstr, len, forma));
	return (rawstr);
}
