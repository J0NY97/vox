/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   font_manager.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 12:55:43 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/24 12:52:26 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FONT_MANAGER_H
# define FONT_MANAGER_H

# include "bitmap.h"

typedef struct	s_font_manager
{
	FT_Library	library;

	FT_Face		*font_faces;			// array of font faces; (because for some reason FT_Face == FT_FontRec_ *);
	Uint32		font_faces_allocated;	// amount of TF_Face's allocated to 'font_faces';
	int			font_face_amount;		// amount of 'font_faces' values;
}	t_font_manager;

/////////////////////////////////
//	FREETYPE
/////////////////////////////////

void	print_face(FT_Face face);

/////////////////////////////////
//	FONT_MANAGER
/////////////////////////////////

void	font_manager_init(t_font_manager *fm);
int		font_manager_get_font(t_font_manager *fm, char *font_path, int font_size);

/////////////////////////////////
//	OTHER
/////////////////////////////////

void	fm_render_text(t_bitmap *bmp, t_font_manager *fm, int font_index, char *str, Uint32 text_color, Uint32 bg_color);

#endif
