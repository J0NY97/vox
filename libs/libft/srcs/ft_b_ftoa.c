/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_b_ftoa.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/09/19 15:54:23 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/24 12:35:31 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "stdio.h"
#include "math.h"

/*
 * 'str' : should already be allocated;
*/
char	*special_itoa(char *str, int nbr, int n)
{
	int		i;

	i = 0;
	while (nbr)
	{
		str[n - (++i)] = (nbr % 10) + '0';
		nbr = nbr / 10;
	}
	while (i < n)
		str[n - (++i)] = '0';
	return (str);
}

char	*ft_b_ftoa(double d, int precision, char *buffer)
{
	int		i;
	int		ipart;
	double	fpart;
	char	temp[16];

	i = 0;
	ipart = abs((int)d);
	fpart = fabs(d - (double)ipart);
	if (d < 0)
		buffer[i++] = '-';
	ft_b_itoa(ipart, temp);
	ft_strcpy(buffer + i, temp);
	i += ft_strlen(temp);
	if (precision > 0)
	{
		temp[precision] = 0;
		buffer[i++] = '.';
		special_itoa(temp, (int)(fpart * ft_pow(10, precision)), precision);
		ft_strncpy(buffer + i, temp, precision);
	}
	buffer[i + precision] = 0;
	return (buffer);
}
