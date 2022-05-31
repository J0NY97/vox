/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 10:06:36 by jsalmi            #+#    #+#             */
/*   Updated: 2022/05/31 13:38:51 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHUNK_H
# define CHUNK_H

typedef struct s_player_info
{
	float	reach; // how far away player can reach from player position;
	int		equipped_block;
	int		equipped_hotbar; // 0 - 8; one of 'hotbar_item_ids' indices;
	int		hotbar_item_ids[9];
}	t_player_info;

/*
 * All dimensions should be '320 % x == 0'
*/
#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define CHUNK_BREADTH 32
static const int g_chunks_per_column = 256 / CHUNK_HEIGHT;
#define CHUNKS_PER_COLUMN g_chunks_per_column
#define BLOCK_SCALE 0.5f
#define BLOCK_SIZE BLOCK_SCALE * 2
static const float g_chunk_size_x = CHUNK_WIDTH * BLOCK_SIZE;
static const float g_chunk_size_y = CHUNK_HEIGHT * BLOCK_SIZE;
static const float g_chunk_size_z = CHUNK_BREADTH * BLOCK_SIZE;
static const float g_chunk_block_amount = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH;
#define CHUNK_BLOCK_AMOUNT g_chunk_block_amount
#define CHUNK_SIZE_X g_chunk_size_x
#define CHUNK_SIZE_Y g_chunk_size_y
#define CHUNK_SIZE_Z g_chunk_size_z
static const int g_render_distance = 320 / CHUNK_WIDTH;
#define RENDER_DISTANCE 9 /*g_render_distance*/
static const int g_chunks_loaded = CHUNKS_PER_COLUMN * RENDER_DISTANCE * RENDER_DISTANCE;
#define CHUNKS_LOADED g_chunks_loaded
static const int g_chunk_columns = RENDER_DISTANCE * RENDER_DISTANCE;
#define CHUNK_COLUMNS g_chunk_columns

typedef struct s_chunk		t_chunk;

enum e_mesh_types
{
	BLOCK_MESH = 0,		// ex. dirt
	FLUID_MESH,			// ex. water
	FLORA_MESH,			// ex. grass
	BLOCK_ALPHA_MESH,	// ex. cactus
	MESH_TYPE_AMOUNT
};

/*
 * All different type of meshes are combined into this one
*/
typedef struct s_chunk_mesh_v2
{
	GLuint			vao;
	GLuint			vbo_pos;
	GLuint			vbo_texture_ids;
	GLuint			*ebo; // same amount of as 'amount';
	GLuint			texture;

	int				amount; // amount of meshes in this mesh;

	float			*vertices;
	size_t			vertices_amount;
	size_t			vertices_allocated;

	int				*texture_ids;
	size_t			texture_id_amount;
	size_t			texture_ids_allocated;

	unsigned int	**indices; // 'amount' amount of pointers;
	size_t			*indices_allocated; // how many indices allocated;
	size_t			*indices_amount; // how many values in the array; (one face has 6 indices);

	size_t			index_amount; // how many indices we have; (one face has 4 indices);
}	t_chunk_mesh_v2;

/*
 * 'visible_faces' : has 'e_faces' 'OR':ed into it, IF the face has been added
 *		to it's chunk's mesh;
*/
typedef struct s_block
{
	int		type; // e_block_type;
	char	is_emit; // is the block currently emitting light; 0 - 1
	char	light_lvl; // the current light lvl of the block; 0 - 15
	char	visible_faces; // any of 'e_faces', can have multiple 'OR':ed;
}	t_block;

void		block_print(t_block *block);

typedef struct s_block_event
{
	t_block	*block; // pointer to the actual block;
	float	pos[3]; // world position of the block;

	// WATER
	int		flow_dir;		// e_card_dir;									(can be changed to 'char');
	int		dist_to_down;	// shortest distance to down block;				(can be changed to 'char');
	int		statique;		// if we are done event handling this block;	(can be changed to 'char');

	// TNT
}	t_block_event;

/*
 * One column of chunks at local chunk 'coordinate';
 * 
 * 'coordinate' : [0](x), [1](z);
 * 'chunks' : amount of 'CHUNKS_PER_COLUMN'; pointer to the chunk_info->chunks[x];
*/
typedef struct s_chunk_col
{
	int		coordinate[2];
	float	world_coordinate[2];
	t_chunk	**chunks;
	int		update_structures;
}	t_chunk_col;

typedef struct s_chunk_info
{
	unsigned int	seed;

	int			block_collision_enabled;
	int			player_collision_enabled;
	int			fancy_graphics;
	int			generate_structures;
	int			light_calculation;
	int			toggle_ui;
	int			toggle_event;

	int			sky_light_lvl;
	int			sky_light_lvl_prev; // previous frame light lvl; (used to decide if the sky light has changed or not);
	int			sky_light_changed; // if the light has changed this frame;

	t_chunk		*chunks; // you should not store the chunks here mainly; its just here so you can acces from places you need, without having to pass them in the function as argumnet;
	t_chunk_col	*chunk_columns; // render_distance
	GLuint		texture; // the texture is stored here so we dont load a texture per chunk_mesh;

	t_hash_item	*hash_table;
	int			hash_table_size;

	// Rendering lists;
	int				*meshes_render_indices;
	int				meshes_render_amount;

}	t_chunk_info;

typedef struct s_block_info
{
	t_chunk	*chunk; // the chunk the block is a part of;
	t_block	*block; // the actual block;
	int		local[3]; // local position of the block;
	float	pos[3]; // world position of the block;
	int		index;	// the index of the block in the chunk blocks arr;
}	t_block_info;

/*
t_block_info	*get_block_info(t_block_info *info, t_block *block);
t_block_info	*get_block_info_index(t_block_info *info, int index);
*/
t_block_info	*get_block_info_pos(t_block_info *res, t_chunk_info *info, float *pos);

/* Used for threading */
typedef struct s_chunk_args
{
	t_chunk	*chunk;
	int		coords[VEC3_SIZE];
	int		*noise_map;
	int		being_threaded;
}	t_chunk_args;

struct	s_chunk
{
	t_chunk_info	*info;
	int				coordinate[VEC3_SIZE];
	float			world_coordinate[VEC3_SIZE];

	int				block_amount;
	t_block			*blocks; //x*y*z real amount should be : CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH;

	int				*block_palette; // amount of all different types of blocks; index is the type, value is the amount;

	int				event_blocks_wanted;
	int				event_block_amount;
	int				event_blocks_allocated;
	t_block_event	*event_blocks; // 

	int				has_blocks; // 1/0 whether the chunk has other than air blocks;
	int				has_visible_blocks; // 1 / 0 whether chunk has visible blocks;
	int				update_water;

	int				blocks_solid_amount; // amount of blocks in this mesh;
	int				blocks_fluid_amount; // amount of blocks in this mesh;
	int				blocks_flora_amount; // amount of blocks in this mesh;
	int				blocks_solid_alpha_amount; // amount of blocks in this mesh;

	/* Mesh Types:
	 * 0 : no alpha, collision, hitbox;		(solid)
	 * 1 : alpha, no collision, no hitbox;	(fluid)
	 * 2 : alpha, no collision, hitbox;		(flora)
	 * 3 : alpha, collision, hitbox;		(solid alpha)
	*/
	t_chunk_mesh_v2	meshes; // TODO : rename to mesh;
	int				mesh_amount; // same amount as the mesh types described above;

	t_aabb			aabb;

	int				needs_to_update;
	int				secondary_update;
	int				was_updated;

	t_chunk_args	args;
};

void		new_chunk(t_chunk *chunk, t_chunk_info *info, int nth);
int			chunk_gen(t_chunk *chunk, int *noise_map);
void		generate_chunk(t_chunk *chunk, int *coord, int *noise_map);
void		update_chunk_visible_blocks(t_chunk *chunk);
void		update_chunk_border_visible_blocks(t_chunk *chunk);
void		chunk_aabb_update(t_chunk *chunk);
void		show_chunk_borders(t_chunk *chunk, t_camera *camera, float *col);
void		render_aabb(t_aabb *a, t_camera *camera, float *col);

int			*get_chunk_pos_from_world_pos(int *res, float *world_coords);
float		*get_block_world_pos(float *res, float *chunk_world_pos, int *block_local_pos);
t_chunk		*get_chunk(t_chunk_info *info, int *pos);
t_chunk		*get_chunk_from_world_pos(t_chunk_info *info, float *pos);
t_chunk		*get_adjacent_chunk(t_chunk_info *info, t_chunk *from, float *dir);
int			*get_block_local_pos_from_world_pos(int *res, float *world);
int			*get_block_local_pos_from_index(int *res, int index);
t_block		*get_block(t_chunk_info *info, float *coords);
int			get_block_index(t_chunk_info *info, int x, int y, int z);
t_chunk		*get_highest_chunk(t_chunk_info *info, int x, int z);
float		get_highest_block_in_chunk(t_chunk *chunk, t_block **out_block, float x, float z);
float		get_highest_point(t_chunk_info *info, float x, float z);
float		get_highest_point_of_type(t_chunk_info *info, float x, float z, int type);
int			get_chunks_to_reload_v2(int *these, int (*into_these)[2], int *start_coord, t_chunk_info *info, int *player_chunk_v3, int max_get);
int			get_chunk_column_to_regen(t_chunk_col *chunk_cols, int *player_chunk, int *out_col_indices, int (*out_col_coords)[2], int max_get);
int			get_surrounding_coords(int *res, int x, int z, int r);
int			get_block_type_at_world_pos(t_chunk_info *info, float *world_pos);

t_block_data	get_block_data(t_block *block);
t_block_data	get_block_data_from_type(int type);

void		update_chunk_column_light(t_chunk_col *column);

unsigned long int	get_chunk_hash_key(int *coords);

typedef struct s_regen_args
{
	int				*reload_these_chunks;
	int				*into_these_coords; // [2]
	t_chunk_info	*chunk_info;
}	t_regen_args;

void		*regen_thread_func(void *args);
int			regenerate_chunks(int *these, int coord[2], t_chunk_info *info);
int			regenerate_chunks_threading(int *these, int coord[2], t_chunk_info *info);
void		regenerate_chunk_column(t_chunk_col *column, int coord[2], int seed);

void		update_chunk(t_chunk *chunk);

void		init_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int amount);
void		reset_chunk_mesh_v2(t_chunk_mesh_v2 *mesh);
void		add_to_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int mesh_type, int *coord, float *face_vertices, int texture_id, int light);
void		update_chunk_mesh(t_chunk_mesh_v2 *mesh);
void		render_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int mesh_type, float *coordinate, t_camera *camera, GLuint shader);
int			chunk_mesh_collision_v2(float *orig, float *dir, t_chunk_mesh_v2 *mesh, int mesh_type, float *world_coords, float reach, float intersect_point[16][3]);

// This is used in the terrain collision; rename when you come it better;
int			chunk_mesh_collision_v56(float *orig, float *dir, t_chunk *chunk, float reach, float intersect_points[16][3], float intersect_normals[16][3]);
// a

t_block		*get_block_from_chunk(t_chunk *chunk, float *point, float *block_pos, int *face);
void		render_block_outline(float *pos, float *color, float *view, float *projection);

t_block		*set_block_at_world_pos(t_chunk_info *info, float *world_pos, int block_type);

void		player_terrain_collision(float *res, float *pos, float *velocity, t_chunk_info *info);

void		tree_gen(t_chunk_info *info, t_chunk_col *column);

void		tree_placer(t_chunk_info *info, float *world_pos);
int			water_placer(t_chunk_info *info, float *world_pos, int nth_from_source);

void		event_chunk(t_chunk *chunk);

// WATER
void		flowing_water_verts(float *verts, int face, t_block *block, float *block_world, t_chunk_info *info);
void		chunk_water_flower(t_chunk_info *info, t_chunk *chunk);
void		chunk_water_remover(t_chunk_info *info, t_chunk *chunk);

// TNT
void		tnt_explosion(t_chunk_info *info, t_chunk *chunk, t_block_event *event_block);

int			is_type_gas(int type);
int			is_type_solid(int type);
int			is_type_flora(int type);
int			is_type_fluid(int type);
int			is_type_solid_alpha(int type);
int			is_type_item(int type);

int			is_gas(t_block *block);
int			is_fluid(t_block *block);
int			is_solid(t_block *block);
int			is_flora(t_block *block);
int			is_solid_alpha(t_block *block);

int			is_water(t_block *block);

int			chunk_has_non_solid(t_chunk *chunk);

#endif