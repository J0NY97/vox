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

# include "limits.h"
# include "pthread.h"
# include "libft.h"
# include "libpf.h"
# include "liblg.h"
# include "glad.h"
# include "glfw3.h"
# include "bmath.h"
# include "bimgf.h"
# include "bobj_loader.h"
# include "collision.h"

# define DEBUG 0

# define ROOT_PATH "./"
# define MODEL_PATH ROOT_PATH"models/"
# define SHADER_PATH ROOT_PATH"shaders/"

typedef struct	s_camera	t_camera;
typedef struct	s_model		t_model;
typedef struct	s_shader	t_shader;
typedef struct	s_fps		t_fps;
typedef struct	s_key		t_key;

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
int			create_shader(GLuint *shader, const char *content, GLenum type);
void		check_program_errors(GLuint program);

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
void		camera_print(t_camera *camera);

///////////////////
//	SKYBOX
///////////////////

typedef struct s_skybox
{
	GLuint		vao;
	GLuint		vbo_position;
	GLuint		texture;
	t_shader	shader;
	float		vertices[6 * 3 * 6];
	size_t		vertices_size;
}	t_skybox;

unsigned int	load_cube_map(char **paths, int paths_size);
t_skybox		*new_skybox(t_skybox *skybox);
float			*skybox_vertices(float *res, size_t *size);
void			render_skybox(t_skybox *skybox, t_camera *camera);

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
//	ENTITY
///////////////////

typedef struct	s_entity
{
	size_t			id;

	float			pos[VEC3_SIZE];
	float			rot_x_angle;
	float			rot_y_angle;
	float			rot_z_angle;
	float			scale_value;

	float			scale_mat[MAT4_SIZE];
	float			rot_mat[MAT4_SIZE];
	float			trans_mat[MAT4_SIZE];
	float			model_mat[MAT4_SIZE];

	int				render_aabb;
	int				collision_detection_enabled;
	int				collision_use_precise;
	t_aabb			aabb;
	float			bb_vertices[24];
	unsigned int	bb_indices[36];
	int				collision;

	int				show_normal_map;

	t_model			model;
}	t_entity;

void				new_entity(t_entity *entity);
void				entity_print(t_entity *entity);
void				update_entity(t_entity *entity);
void				render_entity(t_entity *entity, t_camera *camera, t_model *model, t_shader *shader);

/////////////////////
// ENTITY INSTANCED
/////////////////////

typedef struct s_block
{
	float	pos[VEC3_SIZE]; // in chunk coordinates, add chunk world coordinate to this to get world coordinate;
	int		texture_id;
}	t_block;

typedef struct s_chunk_info
{
	int			width; // in blocks;
	int			breadth; // in blocks;
	int			height; // in blocks;
	float		block_scale;
	float		block_size;
	float		chunk_size;

	unsigned int	seed;

	// TODO: These could be moved to 't_game_info' or something like that.
	int			chunks_loaded;
	int			render_distance;

	float		scale_matrix[MAT4_SIZE];
	//t_model		model; // at some point this;
}	t_chunk_info;

typedef struct	s_chunk
{
	t_chunk_info	*info;
	float		coordinate[VEC3_SIZE];
	float		world_coordinate[VEC3_SIZE];

	int			block_amount;
	t_block		*blocks; //x*y*z real amount should be : 20 736
	int			block_matrices_size; // sizeof (block_positions)
	float		*block_matrices;
	int			block_textures_size;
	int			*block_textures;

	t_model		model;
	GLuint		vbo_matrices;
	GLuint		vbo_texture_ids;
}	t_chunk;

void		new_chunk(t_chunk *chunk, t_chunk_info *info, float *coord);
void		gen_chunk_blocks(t_block *blocks, int *dim);
int			chunk_gen(t_chunk *chunk);
void		render_chunk(t_chunk *chunk, t_camera *camera, t_shader *shader);
void		update_chunk(t_chunk *chunk, float *coord);
float		*player_in_chunk(float *res, float *player_coord, t_chunk_info *info);
void		regenerate_chunks(int *res, t_chunk *chunks, t_chunk_info *info, float *player_chunk_v2);

///////////////////
//	NOISE
///////////////////

int			random_number(int x, int y);
float		perlin(float x, float y, unsigned int seed);
float		perlin3(float x, float y, float z, unsigned int seed);

///////////////////
//	PLAYER
///////////////////

typedef struct s_player
{
	t_camera	camera;

	float		velocity[VEC3_SIZE];
	float		gravity[VEC3_SIZE];

	int			moving;
	int			colliding;
	int			enabled_mouse;
	double		mouse_pos[2];
	double		last_mouse_pos[2];

	t_aabb		aabb;

}	t_player;

void		new_player(t_player *player);
void		player_events(t_player *player, t_key *keys, GLFWwindow *win);
void		player_movement(t_player *player, GLFWwindow *win, t_fps fps);
void		player_apply_velocity(t_player *player);
void		player_looking(t_player *player, GLFWwindow *win, t_fps fps);
void		player_entity_collision(t_player *player, t_entity *entity);
void		player_entity_collision_precise(t_player *player, t_entity *entity);
void		testing_triangle_collision(t_player *player, t_entity *entity);
void		player_print(t_player *player);

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
//	SCENE
///////////////////

/*
 * entity_allocated : size of the entities array;
 * entity_amount : amount of entities in entities array:
 * note: we can have empty spots in the entities array thats why they are separate;
*/
typedef struct	s_scene
{
	t_entity	**entities;
	size_t		entities_allocated;
	size_t		entity_amount;
}		t_scene;

void		create_scene(t_scene *scene);
void		remove_scene(t_scene *scene);
void		remove_entity_from_scene_with_index(t_scene *scene, size_t index);
void		remove_entity_from_scene(t_scene *scene, t_entity *entity);
size_t		add_entity_to_scene(t_scene *scene, t_entity *entity);
t_entity	*get_scene_entity(t_scene *scene, size_t index);

///////////////////
//	SHADERPIXEL
///////////////////

typedef struct	s_shaderpixel
{
	GLFWwindow	*win;
	int			win_w;
	int			win_h;
	int			polygon_mode;
	int			pilpalpol;
}	t_shaderpixel;

///////////////////
//	HELP
///////////////////

void		memset_pattern(void *dest, size_t dest_size, void *src, size_t src_size);
void		normalized_device_coords(float *v3, float *mouse, int w, int h);

///////////////////
//	Crosshair
///////////////////

void		new_crosshair_shader(t_shader *shader);
void		render_crosshair(void);

///////////////////
// Line Draw
///////////////////

void		render_2d_line(float *p1, float *p2, float *col);
void		render_3d_line(float *p1, float *p2, float *col, float *view_mat, float *project_mat);

///////////////////
// Box Draw
///////////////////

float			*create_bb_vertices(float *res, float *min, float *max);
unsigned int	*create_bb_indices(unsigned int *res);
void			render_box(float *vertices, unsigned int *indices, float *col, float *view, float *proj);

#endif
