/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   world.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/18 12:11:30 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/21 11:32:43 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WORLD_H
# define WORLD_H

# include "player.h"

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
typedef struct s_vox_entity	t_vox_entity;

typedef struct s_world
{
	unsigned int	seed;

	char		game_tick; // every time new game tick, set to 1 the frame the tick happened;

	int			block_collision_enabled;
	int			player_collision_enabled;
	int			fancy_graphics;
	int			generate_structures;
	int			light_calculation;
	int			toggle_ui;
	int			toggle_event;

	int			sky_light_lvl;
	int			sky_light_lvl_prev; // previous frame light lvl; (used to decide if the sky light has changed or not);
	
	int			fog_max_dist;
	int			fog_min_dist;

	t_chunk		*chunks; // you should not store the chunks here mainly; its just here so you can acces from places you need, without having to pass them in the function as argumnet;
	t_chunk_col	*chunk_columns; // render_distance * render_distance;
	GLuint		texture; // the texture is stored here so we dont load a texture per chunk_mesh;

	t_vox_entity	*entities;
	int				*entity_amount; // arr size: ENTITY_AMOUNT (e_entity_types); each index corresponds to the 'type' in 't_vox_entity';
	int				entity_amount_total; // all 'entity_amount's added up;

	// Player
	t_player	*player;

/*
	t_hash_item	*hash_table;
	int			hash_table_size;
	*/

	// Rendering lists;
	int				*meshes_render_indices;
	int				meshes_render_amount;

}	t_world;

#endif