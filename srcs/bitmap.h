/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bitmap.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 13:13:20 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/19 13:07:16 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BITMAP_H
# define BITMAP_H

#ifndef Uint32
# define Uint32 unsigned int
#endif
#ifndef Uint8
# define Uint8 unsigned char
#endif

#include "string.h"
#include "stdlib.h"

typedef struct	s_bitmap
{
	Uint32	height;
	Uint32	width;
	Uint32	pitch;
	Uint32	pixel_amount;
	void	*pixels;
}	t_bitmap;

void	bitmap_new(t_bitmap *bmp, int w, int h);
void	bitmap_fill(t_bitmap *bmp, Uint32 col);

#endif