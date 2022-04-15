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
# include "thread.h"
# include "hashtable.h"
# include "block.h"
# include "item.h"

# define DEBUG 0

# define ROOT_PATH "./"
# define MODEL_PATH ROOT_PATH"models/"
# define SHADER_PATH ROOT_PATH"shaders/"

typedef struct	s_player	t_player;
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
//	Frustum
///////////////////

typedef struct s_plane
{
	float	normal[VEC3_SIZE];
	float	dist;
}	t_plane;

typedef struct s_frustum
{
	t_plane	top_plane;
	t_plane	bot_plane;
	t_plane	right_plane;
	t_plane	left_plane;
	t_plane	far_plane;
	t_plane	near_plane;
}		t_frustum;

void		frustum_new(t_frustum *frustum, t_camera *camera);
void		frustum_print(t_frustum *frustum);
void		plane_new(t_plane *plane, float *p, float *norm);
void		plane_print(t_plane *plane, char *str);
int			aabb_in_frustum(t_aabb *a, t_frustum *f);
int			aabb_on_plane(t_aabb *a, t_plane *p);

///////////////////
//	CAMERA
///////////////////

struct s_camera
{
	float	pos[VEC3_SIZE];
	float	front[VEC3_SIZE];
	float	up[VEC3_SIZE];
	float	right[VEC3_SIZE];
	float	yaw;
	float	pitch;
	float	fov;
	float	aspect;
	float	near_plane;
	float	far_plane;
	int		viewport_w;
	int		viewport_h;

	float	view[MAT4_SIZE];
	float	projection[MAT4_SIZE];

	t_frustum	frustum;
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

typedef struct s_player_info
{
	float	reach; // how far away player can reach from player position;
	int		equipped_block; // one from 'e_block_type';
}	t_player_info;

typedef struct s_chunk		t_chunk;

typedef struct s_block
{
	int		type; // e_block_type;
}	t_block;

typedef struct s_chunk_info
{
	int			width; // in blocks;
	int			breadth; // in blocks;
	int			height; // in blocks;
	int			y_chunk_amount;
	float		block_scale;
	float		block_size;
	float		chunk_size[VEC3_SIZE];

	unsigned int	seed;

	// TODO: These could be moved to 't_game_info' or something like that.
	// 		something like t_world, so you can pass it in to the helper functions like 'get_block();'
	int			chunks_loaded; // amount of chunks currently loaded;
	int			render_distance;

	int			block_collision_enabled;
	int			player_collision_enabled;
	int			fancy_graphics;

	t_chunk		*chunks; // you should not store the chunks here mainly; its just here so you can acces from places you need, without having to pass them in the function as argumnet;
	GLuint		texture; // the texture is stored here so we dont load a texture per chunk_mesh;

/*
	t_hash_item	*hash_table;
	int			hash_table_size;
	*/
}	t_chunk_info;

/* Used for threading */
typedef struct s_chunk_args
{
	t_chunk	*chunk;
	int		coords[VEC3_SIZE];
	int		being_threaded;
}	t_chunk_args;

typedef struct s_chunk_mesh
{
	GLuint			vao;
	GLuint			vbo_pos;
	GLuint			vbo_texture_ids;
	GLuint			ebo;
	GLuint			texture; // TODO: make this a pointer so we dont have extra textures uploaded to gpu;

	// These are the values gotten from the mesh creator in code, from
	//	all visible blocks in the chunk;
	float			*vertices;
	size_t			vertices_amount;
	size_t			vertices_allocated;

	int				*texture_ids;
	size_t			texture_id_amount;
	size_t			texture_ids_allocated;

	unsigned int	*indices;
	size_t			indices_amount; // how many values in the array;
	size_t			indices_allocated;
	size_t			index_amount; // how many indices we have;
}			t_chunk_mesh;

struct	s_chunk
{
	t_chunk_info	*info;
	int				coordinate[VEC3_SIZE];
	float			world_coordinate[VEC3_SIZE];

	int				block_amount;
	t_block			*blocks; //x*y*z real amount should be : 20 736

	int				has_blocks; // 1/0 whether the chunk has other than air blocks;
	int				update_structures; // the terrain needs to be generated before the structures, thats why we have this; 

	t_chunk_mesh	mesh;
	int				blocks_solid_amount; // amount of blocks in this mesh;

	t_chunk_mesh	liquid_mesh;
	int				blocks_liquid_amount; // amount of blocks in this mesh;

	t_chunk_mesh	flora_mesh;
	int				blocks_flora_amount; // amount of blocks in this mesh;

	t_aabb			aabb;

	int				needs_to_update;

	int				render; // if we want to render the chunk this frame;

	t_chunk_args	args;
};

void		new_chunk(t_chunk *chunk, t_chunk_info *info, int nth);
int			chunk_gen(t_chunk *chunk);
void		update_chunk(t_chunk *chunk, int *coord);
void		update_chunk_visible_blocks(t_chunk *chunk);
void		chunk_aabb_update(t_chunk *chunk);
void		show_chunk_borders(t_chunk *chunk, t_camera *camera, float *col);
void		render_aabb(t_aabb *a, t_camera *camera, float *col);

int			*get_chunk_pos_from_world_pos(int *res, float *world_coords, t_chunk_info *info);
float		*block_world_pos(float *res, float *chunk_world_pos, int *block_local_pos);
t_chunk		*get_chunk(t_chunk_info *info, int *pos);
t_chunk		*get_adjacent_chunk(t_chunk *from, t_chunk *chunks, int *dir);
int			*get_block_local_pos_from_world_pos(int *res, float *world);
int			*get_block_local_pos_from_index(int *res, int index);
t_block		*get_block(t_chunk_info *info, float *coords);
int			get_block_index(t_chunk_info *info, int x, int y, int z);
float		get_highest_point(t_chunk_info *info, float x, float z);
float		get_highest_point_of_type(t_chunk_info *info, float x, float z, int type);
int			get_chunks_to_reload(int *chunks, int *start_coord, t_chunk_info *info, int *player_chunk_v3);
int			get_surrounding_coords(int *res, int x, int z, int r);

int			get_chunk_hash_key(int *coords);

int			regenerate_chunks(t_chunk *chunks, t_chunk_info *info, int *player_chunk_v2);
void		regenerate_chunks_v3(t_chunk *chunks, t_chunk_info *info, int *player_chunk_v3, t_thread_manager *tm);

void		init_chunk_mesh(t_chunk_mesh *mesh);
void		reset_chunk_mesh(t_chunk_mesh *mesh);
void		add_to_chunk_mesh(t_chunk_mesh *mesh, int *coord, float *face_vertices, int texture_id, int light);
void		update_chunk_mesh(t_chunk_mesh *chunk);
void		render_chunk_mesh(t_chunk_mesh *mesh, float *coordinate, t_camera *camera, t_shader *shader);
int			chunk_mesh_collision(float *orig, float *dir, t_chunk_mesh *mesh, float *world_coords, float reach, float intersect_point[16][3]);
int			chunk_mesh_collision_v2(float *orig, float *dir, t_chunk *chunk, float reach, float intersect_points[16][3], float intersect_normals[16][3]);
t_block		*get_block_from_chunk(t_chunk *chunk, float *point, float *block_pos, int *face);
void		render_block_outline(float *pos, float *color, float *view, float *projection);

void		set_block_at_world_pos(t_chunk_info *info, float *world_pos, int block_type);

void		player_terrain_collision(float *res, float *pos, float *velocity, t_chunk_info *info);

///////////////////
//	NOISE
///////////////////

float		perlin(float x, float y, unsigned int seed);
float		perlin3(float x, float y, float z, unsigned int seed);
float		perlin_v2(float x, float y, float z);
float		octave_perlin(float x, float y, float z, int octaves, float persistence);

///////////////////
//	PLAYER
///////////////////

struct s_player
{
	t_camera	camera;

	float		velocity[VEC3_SIZE];
	float		gravity;

	int			moving;
	int			colliding;
	int			enabled_mouse;
	double		mouse_pos[2];
	double		last_mouse_pos[2];

	t_aabb		aabb;

};

void		new_player(t_player *player);
void		player_events(t_player *player, t_key *keys, GLFWwindow *win);
void		player_movement(t_player *player, GLFWwindow *win, t_fps fps);
void		player_apply_velocity(t_player *player);
void		player_apply_gravity(t_player *player);
void		player_looking(t_player *player, GLFWwindow *win, t_fps fps);
void		player_entity_collision(t_player *player, t_entity *entity);
int			player_entity_mesh_collision(t_player *player, t_entity *entity);
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

enum e_mouse_button_state
{
	BUTTON_PRESS, // the framei t was pressed;
	BUTTON_HOLD, // if held for more than a frame;
	BUTTON_RELEASE, // the frame it was released;
	BUTTON_NONE // if not one of the above (aka not pressed?);
};

struct	s_key
{
	int		state;
};

void		update_all_keys(t_key *keys, t_key *mouse, GLFWwindow *win);

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
void		render_3d_point(float *p1, float *col, float *view_mat, float *project_mat);

///////////////////
// Box Draw
///////////////////

float			*create_bb_vertices(float *res, float *min, float *max);
unsigned int	*create_bb_indices(unsigned int *res);
void			render_box(float *vertices, unsigned int *indices, float *col, float *view, float *proj);

#endif
