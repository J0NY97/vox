/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bitmap.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 13:13:20 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/18 13:16:22 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITMAP_H
# define BITMAP_H

#ifndef Uint32
# define Uint32 unsigned int
#endif

# include "string.h"

typedef struct	s_bitmap
{
	Uint32	rows;
	Uint32	width;
	Uint32	pitch;
	void	*pixels;
}	t_bitmap;

void	new_bitmap(t_bitmap *bmp);

#endif