/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   icmp_echo.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zweng <zweng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/03 15:26:05 by zweng             #+#    #+#             */
/*   Updated: 2023/09/04 18:04:16 by zweng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "icmp.h"

int	icmp_generic_encode (unsigned char * buffer, size_t bufsize, int type,
		int ident, int seqno)
{
	icmphdr_t *icmp;
	if (bufsize < ICMP_MINLEN)
		return -1;
	icmp = (icmphdr_t *) buffer;
	icmp->icmp_type = type;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = htons (seqno);
	icmp->icmp_id = htons (ident);
	icmp->icmp_cksum = icmp_cksum (buffer, bufsize);
	return 0;
}

int	icmp_generic_decode (unsigned char * buffer, size_t bufsize,
		struct ip **ipp, icmphdr_t ** icmpp)
{
	size_t hlen;
	unsigned short cksum;
	struct ip *ip;
	icmphdr_t *icmp;

  /* IP header */
	ip = (struct ip *) buffer;
	hlen = ip->ip_hl << 2;
	if (bufsize < hlen + ICMP_MINLEN)
		return -1;
  /* ICMP header */
	icmp = (icmphdr_t *) (buffer + hlen);
  /* Prepare return values */
	*ipp = ip;
	*icmpp = icmp;
  /* Recompute checksum */
	cksum = icmp->icmp_cksum;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = icmp_cksum ((unsigned char *) icmp, bufsize - hlen);
	if (icmp->icmp_cksum != cksum)
		return 1;
	return 0;
}

int	icmp_echo_encode (unsigned char * buffer, size_t bufsize, int ident,
		int seqno)
{
	return icmp_generic_encode (buffer, bufsize, ICMP_ECHO, ident, seqno);
}

int	icmp_echo_decode (unsigned char * buffer, size_t bufsize,
		struct ip **ipp, icmphdr_t ** icmpp)
{
	return icmp_generic_decode (buffer, bufsize, ipp, icmpp);
}
