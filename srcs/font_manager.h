/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   font_manager.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 12:55:43 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/18 13:13:49 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FONT_MANAGER_H
# define FONT_MANAGER_H

# include "bitmap.h"

typedef struct	s_font_manager
{
	FT_Library	library;
	FT_Face		*font_faces;	//	array of font faces;
}	t_font_manager;

#endif
