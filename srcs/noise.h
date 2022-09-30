/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   noise.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 13:12:33 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/07 13:24:01 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NOISE_H
# define NOISE_H

# include "stdlib.h"
# include "perlin.h"
# include "simplex.h"

typedef struct	s_noise
{
	float	*map;
	int		width;
	int		height;
}		t_noise;

void	noise_create(t_noise *noise, int w, int h, int x_offset, int y_offset, int seed);
void	noise_free(t_noise *noise);

float	noise_get_value(t_noise *noise, int x, int y);

#endif