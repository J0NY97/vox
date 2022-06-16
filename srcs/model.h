/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 13:23:45 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/16 14:24:02 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MODEL_H
# define MODEL_H

# include "bobj_loader.h"
# include "bobj.h"

typedef struct	s_model				t_model;
typedef struct	s_material_info		t_material_info;

typedef struct s_element_info
{
	t_element		element;
	GLuint			ebo;
	t_material_info	*material;
}	t_element_info;

typedef struct s_mesh_info
{
	t_mesh	mesh;
	GLuint	vao;
	GLuint	vbo_pos;
	GLuint	vbo_color;
	GLuint	vbo_norm;
	GLuint	vbo_tex;

	float	*colors;
	size_t	colors_size;
	size_t	colors_value_amount;
	size_t	color_amount;
	size_t	color_value_amount;

	t_element_info	*elem_info;
}	t_mesh_info;

struct s_material_info
{
	t_material	*material;
	GLuint		texture;
	int			loaded;
};

struct	s_model
{
	t_obj			*obj;

	t_mesh_info		*info;
	int				info_amount;

	t_material_info	*mat_info;
	int				mat_info_amount;
};

void		new_model(t_model *model, t_obj *obj);
void		render_model(t_model *model);

/////////////////////////
// V2
/////////////////////////

typedef struct	s_material_v2
{
	GLuint	texture;
	// GLuint bump_map;
	// GLuint bump_normalmap;
	// ...
}	t_material_v2;

/*
 * 'material_index' : from 't_model_v2->materials[x]' where 'x' is the index;
*/
typedef struct s_mesh_v2
{
	GLuint			ebo;

	int				material_index;

	unsigned int	*indices;
	int				indices_amount;
}	t_mesh_v2;

/*
 * TODO : figure out if the vertices/uvs/normals should just be pointers to the
 *		't_bobj->vertices/-...' or do we copy the over?;
 *
 * 'textures' : all the textures loaded from 't_bobj->t_bobj_material's;
*/
typedef struct	s_model_v2
{
	GLuint		vao;	
	GLuint		vbo_pos;	
	GLuint		vbo_tex;	
	GLuint		vbo_nor;	

	float		*vertices;
	float		*uvs;
	float		*normals;

	int			vertices_amount;
	int			uvs_amount;
	int			normals_amount;

	t_mesh_v2	*meshes;
	int			meshes_amount;

	t_material_v2	*materials;
	int			material_amount;
}	t_model_v2;

void	model_from_bobj(t_model_v2 *model, t_bobj *bob, int index);

#endif