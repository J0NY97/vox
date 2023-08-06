/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   chunk.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/27 10:06:36 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 12:15:14 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHUNK_H
# define CHUNK_H

# include "world.h"
# include "block.h"
# include "noise.h"
# include "collision.h"
# include "camera.h"

typedef struct s_chunk		t_chunk;

enum e_mesh_types
{
	BLOCK_MESH = 0,		// ex. dirt
	FLUID_MESH,			// ex. water
	MESH_TYPE_AMOUNT
};

/*
 * All different type of meshes are combined into this one
*/
typedef struct s_chunk_mesh
{
	GLuint			vao;
	GLuint			vbo_pos;
	GLuint			vbo_texture_ids;
	GLuint			*ebo; // same amount of as 'amount';
	GLuint			texture;

	int				amount; // amount of meshes in this mesh;

	float			*vertices;
	int				vertices_amount;
	int				vertices_allocated;

	int				*texture_ids;
	int				texture_id_amount;
	int				texture_ids_allocated;

	unsigned int	**indices; // 'amount' amount of pointers;
	int				*indices_allocated; // how many indices allocated;
	int				*indices_amount; // how many values in the array; (one face has 6 indices);

	int				index_amount; // how many indices we have; (one face has 4 indices);

	GLuint			shader;

	GLuint			uniform_chunk_pos;
	GLuint			uniform_view;
	GLuint			uniform_proj;
	GLuint			uniform_color_tint;
}	t_chunk_mesh;

/*
 * 'visible_faces' : has 'e_faces' 'OR':ed into it, IF the face has been added
 *		to it's chunk's mesh;
*/
typedef struct s_block
{
	int		type; // e_block_type;
	char	light_lvl; // the current light lvl of the block; 0 - 15
	char	visible_faces; // any of 'e_faces', can have multiple 'OR':ed;
}	t_block;

void		block_print(t_block *block);

typedef struct s_block_event
{
	t_block	*block; // pointer to the actual block;
	int		local[3];
	float	pos[3]; // world position of the block;

	// WATER
	int		flow_dir;		// e_card_dir;									(can be changed to 'char');
	int		dist_to_down;	// shortest distance to down block;				(can be changed to 'char');
	int		statique;		// if we are done event handling this block;	(can be changed to 'char');

	// TNT
}	t_block_event;

typedef struct s_light
{
	int		chunk_index;
	int		local[3];
}	t_light;

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

	// Some bools
	char	update_structures;
	char	chunk_needs_update; // if a chunk in this column needs and update this frame; set / reset every frame

	t_light	*lights; // def CHUNK_COLUMN_LIGHT_AMOUNT; (this is only skylights);

	t_noise	height_map;

	t_world	*world;
	int		wanted_coord[2];
	char	being_threaded;
}	t_chunk_col;

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
t_block_info	*get_block_info_pos(t_block_info *res, t_world *info, float *pos);

/* Used for threading */
typedef struct s_chunk_args
{
	t_chunk	*chunk;
	int		coords[V3_SIZE];
	int		*noise_map;
	int		being_threaded;
}	t_chunk_args;

struct	s_chunk
{
	t_world	*world;
	int				coordinate[V3_SIZE];
	float			world_coordinate[V3_SIZE];

	int				block_amount;
	t_block			*blocks; //x*y*z real amount should be : CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH;

	int				*block_palette; // amount of all different types of blocks; index is the type, value is the amount;

	int				event_blocks_wanted;
	int				event_block_amount;
	int				event_blocks_allocated;
	t_block_event	*event_blocks; //

	int				light_emitters; // NOT USED YET

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
	t_chunk_mesh	meshes; // TODO : rename to mesh;

	t_aabb			aabb;

	int				needs_to_update;
	int				secondary_update;
	int				was_updated;

	t_chunk_args	args;
};

void		new_chunk(t_chunk *chunk, t_world *info, int nth);
void		generate_chunk(t_chunk *chunk, int *coord, t_noise *noise);
void		update_chunk_visible_blocks(t_chunk *chunk);
void		update_chunk_event_blocks(t_chunk *chunk);
void		update_chunk_block_palette(t_chunk *chunk);
void		update_chunk_border_visible_blocks(t_chunk *chunk);
void		chunk_aabb_update(t_chunk *chunk);
void		show_chunk_borders(t_chunk *chunk, t_camera *camera, float *col);
void		render_aabb(t_aabb *a, t_camera *camera, float *col);

int			*get_chunk_pos_from_world_pos(int *res, float *world_coords);
float		*get_block_world_pos(float *res, float *chunk_world_pos, int *block_local_pos);
t_chunk		*get_chunk(t_world *info, int *pos);
t_chunk		*get_chunk_from_column(t_chunk_col *col, int y);
t_chunk_col	*get_chunk_column(t_world *info, int *pos_v2);
t_chunk		*get_chunk_from_world_pos(t_world *info, float *pos);
t_chunk		*get_adjacent_chunk(t_world *info, t_chunk *from, float *dir);
int			*get_block_local_pos_from_world_pos(int *res, float *world);
int			*get_block_local_pos_from_index(int *res, int index);
t_block		*get_block(t_world *info, float *coords);
int			get_block_index(int x, int y, int z);
t_chunk		*get_highest_chunk(t_world *info, int x, int z);
float		get_highest_block_in_chunk(t_chunk *chunk, t_block **out_block, float x, float z);
float		get_highest_point(t_world *info, float x, float z);
float		get_highest_point_of_type(t_world *info, float x, float z, int type);
int			get_chunks_to_reload_v2(int *these, int (*into_these)[2], int *start_coord, t_world *info, int *player_chunk_v3, int max_get);
int			get_chunk_column_to_regen(t_world *info, int *player_chunk, int *out_col_indices, int (*out_col_coords)[2], int max_get);
int			get_surrounding_coords(int *res, int x, int z, int r);
int			get_block_type_at_world_pos(t_world *info, float *world_pos);

t_block_data	get_block_data(t_block *block);
t_block_data	get_block_data_from_type(int type);

void		update_chunk_column_light(t_chunk_col *column);

unsigned long int	get_chunk_hash_key(int *coords);

typedef struct s_regen_args
{
	int			*reload_these_chunks;
	int			*into_these_coords; // [2]
	t_world	*chunk_info;
}	t_regen_args;

void		regen_column_thread(void *args);
int			regenerate_chunks(int *these, int coord[2], t_world *info);
int			regenerate_chunks_threading(int *these, int coord[2], t_world *info);
void		regenerate_chunk_column(t_chunk_col *column, int coord[2], int seed);

void		update_chunk(t_chunk *chunk);

void		init_chunk_mesh(t_chunk_mesh *mesh, GLuint shader, int amount);
void		reset_chunk_mesh(t_chunk_mesh *mesh);
void		add_to_chunk_mesh(t_chunk_mesh *mesh, int mesh_type, int *coord, float *face_vertices, int texture_id, int light);
void		update_chunk_mesh(t_chunk_mesh *mesh);
void		render_chunk_mesh(t_chunk_mesh *mesh, int mesh_type, float *coordinate, t_camera *camera);
int			chunk_mesh_collision_v2(float *orig, float *dir, t_chunk_mesh *mesh, int mesh_type, float *world_coords, float reach, float intersect_point[16][3]);

// This is used in the terrain collision; rename when you come up with better;
int			chunk_mesh_collision_v56(float *orig, float *dir, t_chunk *chunk, float reach, float intersect_points[16][3], float intersect_normals[16][3]);
// a

t_block		*get_block_from_chunk(t_chunk *chunk, float *point, float *block_pos, int *face);
t_block		*get_block_from_chunk_local(t_chunk *chunk, int *local_pos);
void		render_block_outline(float *pos, float *color, float *view, float *projection);

void		update_block_visibility(t_chunk *chunk, int *block_pos, int _cardDir);
void		update_block_visibility_v3(t_chunk *chunk, int *block_pos, int _cardDir, int (*cardinalDirs)[3], t_block_data *blockDatas);

t_block		*set_block_at_world_pos(t_world *info, float *world_pos, int block_type);

void		player_terrain_collision(float *res, float *pos, float *velocity, t_world *info);

void		tree_gen(t_world *info, t_chunk_col *column);

void		tree_placer(t_world *info, float *world_pos);
int			water_placer(t_world *info, float *world_pos, int nth_from_source);
void		water_flow(t_world *info, t_block_event *water);
int			water_remove(t_world *info, t_block_event *water);

void		event_chunk(t_chunk *chunk);

// WATER
void		flowing_water_verts(float *verts, int face, t_block *block, float *block_world, t_world *info);
void		chunk_water_flower(t_world *info, t_chunk *chunk);
void		chunk_water_remover(t_world *info, t_chunk *chunk);

// TNT
void		tnt_explosion(t_world *info, t_block_event *event_block);

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
