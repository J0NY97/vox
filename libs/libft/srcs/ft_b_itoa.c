/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_b_itoa.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/10/22 13:18:53 by nneronin          #+#    #+#             */
/*   Updated: 2022/05/24 12:07:17 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

/*
 * NOTE : only positive numbers;
*/
static char	*nbr_to_str(char *buffer, int nbr)
{
	int	i;

	i = 0;
	if (nbr == 0)
		buffer[i++] = '0';
	while (nbr != 0)
	{
		buffer[i++] = nbr % 10 + '0';
		nbr = nbr / 10;
	}
	buffer[i] = 0;
	return (buffer);
}

/*
 * NOTE : we assume the 'nbr' will fit in the 'buffer';
 *
 * 'nbr' : number you want to stringify;
 * 'buffer' : should already be allocated;
 *
 * Return : 'buffer';
*/
char	*ft_b_itoa(int nbr, char *buffer)
{
	int		i;
	char	temp[16];

	if (nbr == -2147483648)
	{
		ft_strncpy(buffer, "-2147483648", 11);
		return (buffer);
	}
	i = 0;
	if (nbr < 0)
	{
		buffer[0] = '-';
		i = 1;
		nbr = -nbr;
	}
	nbr_to_str(temp, nbr);
	ft_b_reverse(temp);
	ft_strcpy(buffer + i, temp);
	return (buffer);
}
