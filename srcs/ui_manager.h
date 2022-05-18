/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui_manager.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/18 13:10:15 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/18 16:36:55 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UI_MANAGER_H
# define UI_MANAGER_H

# include "shader.h"
# include "bitmap.h"

typedef struct s_ui_manager
{
	t_bitmap	canvas;	
	
	// OpenGL Stuff
	GLuint	vao;
	GLuint	vbo;
	GLuint	ebo;
	GLuint	texture;

	GLfloat	*vertices;
	GLfloat	*uvs;
	GLuint	*indices;

	GLuint	vertex_amount;
	GLuint	uvs_amount;
	GLuint	index_amount;

	GLuint	shader;
}	t_ui_manager;

void	ui_manager_init(t_ui_manager *ui);
void	ui_manager_render(t_ui_manager *ui);

#endif
