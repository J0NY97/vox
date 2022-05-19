/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_manager.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 13:10:15 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/19 12:56:37 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UI_MANAGER_H
# define UI_MANAGER_H

# include "shader.h"
# include "bitmap.h"

typedef struct s_ui_vertex
{
	float	pos[2];
	float	uv[2];
	Uint8	col[4];
}	t_ui_vertex;

typedef struct s_ui_manager
{
	t_bitmap	bitmap;

	// OpenGL Stuff
	GLuint	vao;
	GLuint	vbo;
	GLuint	ebo;
	GLuint	texture;

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

// Shapes
void	ui_draw_rect(t_ui_manager *ui, float *pos, Uint8 *color);

#endif
