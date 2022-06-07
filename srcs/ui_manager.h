/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_manager.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 13:10:15 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/07 15:03:22 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UI_MANAGER_H
# define UI_MANAGER_H

# include "shader.h"
# include "bitmap.h"
//# include "font_manager.h"

typedef struct s_ui_vertex
{
	float	pos[2];
	float	uv[2];
	Uint8	col[4];
}	t_ui_vertex;

typedef struct s_ui_element
{
	GLuint	draw_type;
	GLuint	texture;
	int		index_start;
	int		index_amount;
}	t_ui_element;

typedef struct s_ui_manager
{
	t_font_manager	font_manager;
	t_bitmap		bitmap;

	t_ui_element	*elements;
	Uint32			elements_allocated;
	Uint32			element_amount;

	// OpenGL Stuff
	GLuint	vao;
	GLuint	vbo;
	GLuint	ebo;
	GLuint	texture; // default texture;

	GLuint	*all_textures;
	Uint32	textures_generated;
	Uint32	textures_in_use;

	t_ui_vertex	*vertices;
	Uint32		*indices;

	Uint32	vertices_allocated;
	Uint32	indices_allocated;
	GLuint	vertex_amount;
	GLuint	index_amount;

	GLuint	shader;

	GLint	attrib_pos;
	GLint	attrib_uv;
	GLint	attrib_col;
	GLint	uniform_tex;
	GLint	uniform_proj;
}	t_ui_manager;

void	ui_manager_init(t_ui_manager *ui);
void	ui_manager_start(t_ui_manager *ui);
void	ui_manager_end(t_ui_manager *ui);
void	ui_manager_render(t_ui_manager *ui, int width, int height);

// HELP
GLuint	ui_new_texture(t_ui_manager *ui, t_bitmap *bmp);

// Shapes
void	ui_draw_filled_rect_multi_color(t_ui_manager *ui, float *pos, Uint32 *color);
void	ui_draw_filled_rect(t_ui_manager *ui, float *pos, Uint8 *color);
void	ui_draw_rect(t_ui_manager *ui, float *pos, Uint8 *color);
void	ui_draw_bitmap(t_ui_manager *ui, float *pos, t_bitmap *bmp);

#endif
