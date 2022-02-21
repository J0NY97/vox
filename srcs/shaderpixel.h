/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 14:27:43 by jsalmi            #+#    #+#             */
/*   Updated: 2022/02/16 18:07:47by jsalmi           ###   ########.fr       */
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

# define DEBUG 1

# define ROOT_PATH "C:/Users/Jony/source/repos/shaderpixel/"
# define MODEL_PATH ROOT_PATH"models/"
# define SHADER_PATH ROOT_PATH"shaders/"

typedef struct	s_camera	t_camera;
typedef struct	s_model		t_model;
typedef struct	s_shader	t_shader;
typedef struct	s_fps		t_fps;
typedef struct	s_key		t_key;

///////////////////
//	ENTITY
///////////////////

typedef struct	s_entity
{
	float			pos[VEC3_SIZE];
	float			rot_x_angle;
	float			rot_y_angle;
	float			rot_z_angle;
	float			scale_value;

	float			scale[MAT4_SIZE];
	float			rot[MAT4_SIZE];
	float			trans[MAT4_SIZE];
	float			model[MAT4_SIZE];
}	t_entity;

void				new_entity(t_entity *entity);
void				update_entity(t_entity *entity);
void				render_entity(t_entity *entity, t_camera *camera, t_model *model, t_shader *shader);

///////////////////
//	MODEL
///////////////////

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

struct s_camera
{
	float	pos[VEC3_SIZE];
	float	front[VEC3_SIZE];
	float	up[VEC3_SIZE];
	float	yaw;
	float	pitch;
	float	fov;
	int		viewport_w;
	int		viewport_h;

	float	view[MAT4_SIZE];
	float	projection[MAT4_SIZE];
};

void		new_camera(t_camera *camera);
void		update_camera(t_camera *camera);

///////////////////
//	FRACTAL
///////////////////
typedef struct s_fractal2d
{
	GLuint	vao;
	GLuint	vbo;
	GLuint	ebo;
	GLfloat	vertices[12];
	GLuint	indices[6];
}	t_fractal2d;

void		new_fractal2d(t_fractal2d *fractal);
void		render_fractal2d(t_fractal2d *fractal, t_shader *shader);

///////////////////
//	PLAYER
///////////////////

typedef struct s_player
{
	t_camera	camera;

	float		velocity[VEC3_SIZE];
	float		gravity[VEC3_SIZE];

	int			enabled_mouse;
	double		mouse_pos[2];
	double		last_mouse_pos[2];
}	t_player;

void		new_player(t_player *player);
void		player_events(t_player *player, t_key *keys, GLFWwindow *win);
void		player_movement(t_player *player, GLFWwindow *win, t_fps fps);
void		player_looking(t_player *player, GLFWwindow *win, t_fps fps);

///////////////////
//	FPS
///////////////////

struct	s_fps
{
	float			last_time;
	float			curr_time;
	float			delta_time;
	int				fps;
	int				count;
};

void		new_fps(t_fps *fps);
void		update_fps(t_fps *fps);

///////////////////
//	KEYS
///////////////////

struct	s_key
{
	int		state;
};

void		update_all_keys(t_key *keys, GLFWwindow *win);

///////////////////
//	COLLISION
///////////////////

int			ellipsoid_collision(float *pos, float *velocity, t_mesh *mesh);

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