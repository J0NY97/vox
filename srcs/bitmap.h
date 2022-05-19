/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bitmap.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 13:13:20 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/19 12:21:10 by jsalmi           ###   ########.fr       */
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
	Uint32	rows;
	Uint32	width;
	Uint32	pitch;
	void	*pixels;
}	t_bitmap;

void	new_bitmap(t_bitmap *bmp, int w, int h);

#endif