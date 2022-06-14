/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/14 12:37:39 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VOX_ENTITY_H
# define VOX_ENTITY_H

# include "stdlib.h"
# include "glad.h"
# include "glfw3.h"
# include "camera.h"

enum e_entity_types
{
	ENTITY_MELON_GOLEM = 0,
	ENTITY_AMOUNT
};

typedef struct s_vox_entity_mesh
{
	GLuint			vao;
	GLuint			vbo_pos;
	GLuint			vbo_tex;
	GLuint			ebo;
	GLuint			texture;
	
	float			*vertices;
	int				vertices_amount;
	int				vertices_allocated;

	float			*uvs;
	int				uvs_amount;
	int				uvs_allocated;
	
	unsigned int	*indices;
	int				indices_amount;
	int				indices_allocated;

	GLuint			shader;

	/* Uniforms */
	GLuint			uniform_view;
	GLuint			uniform_proj;
}		t_vox_entity_mesh;

typedef struct s_vox_entity_data
{
	char		*name;
	char		*readable_name;
	char		type;			// 'e_entity_types';
}		t_vox_entity_data;

static const t_vox_entity_data	g_entity_data[] = {
	{
		"ENTITY_MELON_GOLEM",
		"Melon Golem",
		ENTITY_MELON_GOLEM	
	}
};

typedef struct s_vox_entity
{
	float	pos[3];
	float	rot[3];

	char	type; // 'e_entity_types';
}		t_vox_entity;

#endif