/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   world.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/18 12:11:30 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/24 11:28:59 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORLD_H
# define WORLD_H

# include "player.h"
# include "entity_manager.h"

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
static const int g_chunk_block_amount = CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_BREADTH;
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
static const int g_chunk_column_light_amount = CHUNK_WIDTH * CHUNK_BREADTH;
#define CHUNK_COLUMN_LIGHT_AMOUNT g_chunk_column_light_amount

typedef struct s_chunk		t_chunk;
typedef struct s_chunk_col	t_chunk_col;

typedef struct s_world
{
	unsigned int	seed;

	char		game_tick; // every time new game tick, set to 1 the frame the tick happened;

	char		block_collision_enabled;
	char		player_collision_enabled;
	char		fancy_graphics;
	char		generate_structures;
	char		light_calculation;
	char		toggle_event;
	char		generate_caves;

	int			sky_light_lvl;
	int			sky_light_lvl_prev; // previous frame light lvl; (used to decide if the sky light has changed or not);

	int			fog_max_dist;
	int			fog_min_dist;

	t_chunk		*chunks;		// size : CHUNKS_LOADED;
	t_chunk_col	*chunk_columns; // size : RENDER_DISTANCE * RENDER_DISTANCE;
	GLuint		texture; // the texture is stored here so we dont load a texture per chunk_mesh;
	int			player_chunk[3];

	/////////////////
	// ENTITY
	/////////////////
	/*

	t_bobj			*entity_objects;		// size : ENTITY_AMOUNT;
	t_model_v2		*entity_models;			// size : ENTITY_AMOUNT;

	t_entity	*entities;
	int				entity_amount_total;	// all 'entity_amount's added up;
	*/

	t_entity_manager entity_manager;

	float		spawn_point[3];

	t_camera camera; // the main camera in the world;

/*
	t_hash_item	*hash_table;
	int			hash_table_size;
	*/

	// Rendering lists;
	int				*meshes_render_indices;
	int				meshes_render_amount;

}	t_world;


#endif
