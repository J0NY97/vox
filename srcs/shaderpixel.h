/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 14:27:43 by jsalmi            #+#    #+#             */
/*   Updated: 2022/02/11 14:27:43 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPIXEL_H
# define SHADERPIXEL_H

# include "libft.h"
# include "libpf.h"
# include "liblg.h"
# include "glad.h"
# include "glfw3.h"
# include "bmath.h"
# include "bimgf.h"
# include "bobj_loader.h"

# define ROOT_PATH "C:/Users/Jony/source/repos/shaderpixel/"
# define MODEL_PATH ROOT_PATH"models/"
# define SHADER_PATH ROOT_PATH"shaders/"

typedef struct	s_model		t_model;
typedef struct	s_shader	t_shader;

///////////////////
//	ENTITY
///////////////////

typedef struct	s_entity
{
	float			pos[VEC3_SIZE];
	float			rot[VEC3_SIZE];
	float			scale[VEC3_SIZE];
	float			rot_x_angle;
	float			rot_y_angle;
	float			rot_z_angle;
	float			scale_value;

	float			orig_pos[VEC3_SIZE];
	float			orig_rot[VEC3_SIZE];
	float			orig_scale[VEC3_SIZE];
	float			orig_rot_x_angle;
	float			orig_rot_y_angle;
	float			orig_rot_z_angle;
	float			orig_scale_value;
}	t_entity;

void				new_entity(t_entity *entity);
void				render_entity(t_entity *entity, t_model *model, t_shader *shader);

///////////////////
//	MODEL
///////////////////

typedef struct s_element_info
{
	t_element	element;
	GLuint		ebo;
	GLuint		texture;
	int			has_texture;
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

typedef struct s_material_info
{
	t_material	*material;
	GLuint		texture;
	int			loaded;
}	t_material_info;

struct	s_model
{
	t_obj			*obj;

	t_mesh_info		*info;
	int				info_amount;

	t_material_info	*mat_info;
	int				mat_info_amount;
};

void		new_model(t_model *model, t_obj *obj);

///////////////////
//	SHADER
///////////////////

struct	s_shader
{
	unsigned int	program;
};

void		new_shader(t_shader *shader, const char *vs, const char *fs);

///////////////////
//	TEXTURE
///////////////////

int			new_texture(GLuint *texture, const char *image_file);

///////////////////
//	CAMERA
///////////////////

typedef struct s_camera
{
	float	pos[VEC3_SIZE];
	float	front[VEC3_SIZE];
	float	up[VEC3_SIZE];
	float	yaw;
	float	pitch;
	float	fov;
}	t_camera;

///////////////////
//	SHADERPIXEL
///////////////////

typedef struct	s_shaderpixel
{
	GLFWwindow	*win;
	int			win_w;
	int			win_h;
}	t_shaderpixel;

#endif