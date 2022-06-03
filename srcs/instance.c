/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   instance.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 14:49:05 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/03 15:24:54 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include "chunk.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/
void	new_chunk(t_chunk *chunk, t_chunk_info *info, int nth)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	chunk->needs_to_update = 0;
	chunk->secondary_update = 0;
	chunk->was_updated = 0;

	chunk->event_block_amount = 0;
	chunk->event_blocks_allocated = 2048;
	chunk->event_blocks = malloc(sizeof(t_block_event) * chunk->event_blocks_allocated);

	chunk->block_palette = malloc(sizeof(int) * BLOCK_TYPE_AMOUNT);

	int	max_blocks = CHUNK_WIDTH * CHUNK_BREADTH * CHUNK_HEIGHT;

	chunk->blocks = malloc(sizeof(t_block) * (max_blocks));

	// Set INT_MAX to coordinates, so that the chunk regenerator knows to regenerate these chunks;
	for (int i = 0; i < 3; i++)
	{
		chunk->coordinate[i] = INT_MAX - (nth * 10);
		chunk->world_coordinate[i] = INT_MAX - (nth * 10);
	}

/*
	unsigned long int key = get_chunk_hash_key(chunk->coordinate);
	if (!hash_item_insert(info->hash_table, info->hash_table_size, key, nth))
		LG_WARN("Chunk data couldnt be inserted.");
		*/

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * Create chunk from noise map;
*/
int	chunk_gen(t_chunk *chunk, int *noise_map)
{
	int i = -1;

	chunk->has_blocks = 0;
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			float	block_world_z = fabs(chunk->world_coordinate[2] + z);
			// How many blocks there should be on that x/z coordinate; can be more than the chunk_height, which means it spans over multiple chunks;
			int		whatchumacallit = noise_map[x * CHUNK_HEIGHT + z] - (int)chunk->world_coordinate[1];

			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				float	block_world_y = chunk->world_coordinate[1] + y;
				float	perper = 100.0f;
				i++;
				/* Cave Gen
				if (whatchumacallit > 0)
				{
					float	pers = 0.5f;
					float	freq = 0.040f;
					perper = octave_perlin(block_world_x * freq, block_world_y * freq, block_world_z * freq, 1, pers) +
						octave_perlin(block_world_x * freq, block_world_y * freq, block_world_z * freq, 2, pers)* +
						octave_perlin(block_world_x * freq, block_world_y * freq, block_world_z * freq, 4, pers) +
						octave_perlin(block_world_x * freq, block_world_y * freq, block_world_z * freq, 8, pers);
				}
				*/
				chunk->blocks[i].visible_faces = 0;
				chunk->blocks[i].light_lvl = 15;
				chunk->blocks[i].type = GAS_AIR; // Default to air;
				if (perper > 0.9f && y <= whatchumacallit)
				{
					if (y == whatchumacallit)
						chunk->blocks[i].type = BLOCK_DIRT_GRASS;
					else if (y <= whatchumacallit - 3) // if we have 3 dirt block on top we make the rest stone blocks;
						chunk->blocks[i].type = BLOCK_STONE;
					else if (y < whatchumacallit)
						chunk->blocks[i].type = BLOCK_DIRT;
					else
					{
						if (block_world_y < 2)
							chunk->blocks[i].type = BLOCK_BEDROCK;
						else
							continue ;
					}
					chunk->has_blocks = 1;
				}
				else // everything that is over the noise_map value;
				{
					if (y >= whatchumacallit + 3 && block_world_y <= 62)
					{
						chunk->blocks[i].type = FLUID_WATER;
						chunk->has_blocks = 1;
					}
					else if (y >= whatchumacallit + 1 && y < whatchumacallit + 3 && block_world_y <= 65)
					{
						chunk->blocks[i].type = BLOCK_SAND;
						chunk->has_blocks = 1;
					}
				}
			}
		}
	}
	return (i);
}

void	get_chunk_world_pos_from_local_pos(float *res, int *local_pos)
{
	res[0] = local_pos[0] * (float)CHUNK_WIDTH;
	res[1] = local_pos[1] * (float)CHUNK_HEIGHT;
	res[2] = local_pos[2] * (float)CHUNK_BREADTH;
}

int	create_noise_map(int *map, int size_x, int size_z, int coord_x, int coord_z, int seed)
{
	int		start_y = 64;
	float	freq = 0.005f;
	float	pers = 0.5f;
	int		i = 0;
	float	final_seed = (seed % 512) * freq;
	float	chunk_world[3];

	get_chunk_world_pos_from_local_pos(chunk_world, (int []){coord_x, 0, coord_z});
	for (int x = 0; x < size_x; x++)
	{
		float	block_world_x = fabs(chunk_world[0] + x);
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < size_z; z++)
		{
			float	block_world_z = fabs(chunk_world[2] + z);
			float	to_use_z = block_world_z * freq;
			float	perper =
				octave_perlin(to_use_x, final_seed, to_use_z, 1, pers) +
				octave_perlin(to_use_x, final_seed, to_use_z, 2, pers) +
				octave_perlin(to_use_x, final_seed, to_use_z, 4, pers) +
				octave_perlin(to_use_x, final_seed, to_use_z, 8, pers);
			float	e = pers * 3;
			int		wanted_y = (start_y * (perper / e));
			map[i] = wanted_y;
			i++;
		}
	}
	return (i);
}

/*
 * Give any world position to 'world_coords' and this calculates the 
 *	chunk coordinate that you're in;
*/
int	*get_chunk_pos_from_world_pos(int *res, float *world_coords)
{
	res[0] = floor(world_coords[0] / CHUNK_SIZE_X);
	res[1] = floor(world_coords[1] / CHUNK_SIZE_Y);
	res[2] = floor(world_coords[2] / CHUNK_SIZE_Z);
	return (res);
}

void	chunk_aabb_update(t_chunk *chunk)
{
	t_aabb	*a;

	a = &chunk->aabb;
	a->min[0] = chunk->world_coordinate[0] - BLOCK_SCALE;
	a->min[1] = chunk->world_coordinate[1] - BLOCK_SCALE;
	a->min[2] = chunk->world_coordinate[2] - BLOCK_SCALE;
	a->max[0] = a->min[0] + CHUNK_SIZE_X;
	a->max[1] = a->min[1] + CHUNK_SIZE_Y;
	a->max[2] = a->min[2] + CHUNK_SIZE_Z;
}

/*
 * Returns pointer to chunk if we can find the correct one;
 * 	else NULL;
 * 
 * 'from' is the chunk we want to look in a direction from,
 * 'chunks' is all the loaded chunks,
 * 'dir' is the direction you want to look for the chunk in; (v3)
*/
t_chunk	*get_adjacent_chunk(t_chunk_info *info, t_chunk *from, float *dir)
{
	int	from_coord[3];

	from_coord[0] = from->coordinate[0] + (int)dir[0];
	from_coord[1] = from->coordinate[1] + (int)dir[1];
	from_coord[2] = from->coordinate[2] + (int)dir[2];
	for (int i = 0; i < CHUNK_COLUMNS; i++)
	{
		if (info->chunk_columns[i].coordinate[0] == from_coord[0] && 
			info->chunk_columns[i].coordinate[1] == from_coord[2])
		{
			for (int j = CHUNKS_PER_COLUMN - 1; j >= 0; j--)
			{
				if (info->chunk_columns[i].chunks[j]->coordinate[1] == from_coord[1])
					return (info->chunk_columns[i].chunks[j]);
			}
			return (NULL);
		}
	}
	return (NULL);
}

t_chunk	*get_chunk(t_chunk_info	*info, int *pos)
{
	for (int i = 0; i < CHUNKS_LOADED; i++)
	{
		if (info->chunks[i].coordinate[0] == pos[0] && 
			info->chunks[i].coordinate[1] == pos[1] && 
			info->chunks[i].coordinate[2] == pos[2])
			return (&info->chunks[i]);
	}
	return (NULL);
}

t_chunk	*get_chunk_from_world_pos(t_chunk_info *info, float *pos)
{
	int	local_pos[3];

	get_chunk_pos_from_world_pos(local_pos, pos);
	return (get_chunk(info, local_pos));
}

/*
 * From local chunk block pos to the index in the 'chunk->blocks' array;
*/
int	get_block_index(int x, int y, int z)
{
	return ((x * CHUNK_WIDTH * CHUNK_BREADTH) + (z * CHUNK_HEIGHT) + y);
}

int	*get_block_local_pos_from_world_pos(int *res, float *world)
{
	res[0] = mod(world[0], CHUNK_WIDTH);
	res[1] = mod(world[1], CHUNK_HEIGHT);
	res[2] = mod(world[2], CHUNK_BREADTH);
	return (res);
}

/*
 * Saves answer in 'res';
 * And returns it;
*/
float *get_block_world_pos(float *res, float *chunk_world_pos, int *block_local_pos)
{
	float	scale = 0.5; // t_chunk_info->block_scale;

	res[0] = chunk_world_pos[0] + (block_local_pos[0] /* scale*/);
	res[1] = chunk_world_pos[1] + (block_local_pos[1] /* scale*/);
	res[2] = chunk_world_pos[2] + (block_local_pos[2] /* scale*/);
	return (res);
}

/*
 * From 'index' in 'chunk->blocks' get the x,y,z pos in the chunk coordinates;
 * 	'max' is the max width, breadth and height of the chunk;
*/
int	*get_block_local_pos_from_index(int *res, int index)
{
	// TODO : for sure doesnt work;
	res[0] = index / (CHUNK_WIDTH * CHUNK_HEIGHT);
	res[1] = index % CHUNK_HEIGHT;
	res[2] = (index / CHUNK_WIDTH) % CHUNK_HEIGHT;
	return (res);
}

/*
 * Returns pointer to block anywhere in the world with the 'coord's given;
 * World coordinates btw;
*/
t_block	*get_block(t_chunk_info *info, float *coords)
{
	int		chunk_pos[3];
	t_chunk	*chunk;

	get_chunk_pos_from_world_pos(chunk_pos, coords);
	chunk = get_chunk(info, chunk_pos);
	if (chunk)
	{
		int	local_pos[3];
		get_block_local_pos_from_world_pos(local_pos, coords);
		int index = get_block_index(local_pos[0], local_pos[1], local_pos[2]);
		return (&chunk->blocks[index]);
	}
	return (NULL);
}

/*
 * Tries getting block from 'chunk' and then 'chunk2';
 * 'coords' : are in local position, can go over 0 and CHUNK_W/H/B, if that happesn we check the adjacent chunk;
 * 'chunk' : the chunk we think the block is in;
 * 'chunk2' : the neighboring chunk we think the block is in;
*/
t_block	*get_block_faster(t_chunk *chunk, t_chunk *chunk2, int *coords)
{
	t_block	*adj;

	adj = NULL;
	// Try to get from the same chunk first;
	if (coords[0] >= 0 && coords[0] < CHUNK_WIDTH &&
		coords[1] >= 0 && coords[1] < CHUNK_HEIGHT &&
		coords[2] >= 0 && coords[2] < CHUNK_BREADTH)
		adj = &chunk->blocks[get_block_index(coords[0], coords[1], coords[2])];
	else if (chunk2) // then check the neighbor; (the neighbor should already be the correct one)
		adj = &chunk2->blocks[get_block_index(
			mod(coords[0], CHUNK_WIDTH), mod(coords[1], CHUNK_HEIGHT), mod(coords[2], CHUNK_BREADTH))];
	return (adj);
}

t_block	*get_block_in_dir(t_chunk *chunk, t_chunk *neighbor, int *local_pos, int dir)
{
	int	coords[3];

	coords[0] = local_pos[0] + g_card_dir_int[dir][0];
	coords[1] = local_pos[1] + g_card_dir_int[dir][1];
	coords[2] = local_pos[2] + g_card_dir_int[dir][2];
	return (get_block_faster(chunk, neighbor, coords));
}

/*
 * 'block' : from which block we want to add a face to the mesh;
 * 'chunk' : the chunk in which the block recides; (and mesh);
 * 'adjacent' : the adjacent block in the direction of the face of 'block';
 * 'local_pos' : the local pos of the 'block' in 'chunk';
 * 'dir' : aka face, of the 'block' we want to add to 'chunk->meshes';
*/
void	add_block_to_correct_mesh(t_chunk *chunk, t_block *block, t_block *adjacent, int *local_pos, int dir)
{
	t_block_data	data;
	float			block_world[3];
	int				light;

	if (adjacent)
	{
		data = get_block_data(block);
		light = (int)(ft_pow(0.9f, 15 - ft_clamp(block->light_lvl, 0, 15)) * 100.0f) * (g_face_light[dir] / 100.0f);
		if (is_fluid(block) && !is_solid(adjacent) &&
			!(is_fluid(block) && is_fluid(adjacent)))
		{
			// If both blocks are fluid, we dont add face to mesh;
			float	verts[12];

			get_block_world_pos(block_world, chunk->world_coordinate, local_pos);
			flowing_water_verts(verts, dir, block, block_world, chunk->info);
			add_to_chunk_mesh_v2(&chunk->meshes, FLUID_MESH, local_pos, verts, data.texture[0], light);
			++chunk->blocks_fluid_amount;
		}
		else if (is_solid(block) && !is_solid(adjacent))
		{
			add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_MESH, local_pos, (float *)g_faces[dir], data.texture[dir], light);
			++chunk->blocks_solid_amount;
		}
		else if (is_solid_alpha(block))
		{
			if (block->type == BLOCK_ALPHA_OAK_LEAF)
				add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_ALPHA_MESH, local_pos, (float *)g_faces[dir], data.texture[dir], light);
			else if (block->type == BLOCK_ALPHA_TORCH)
				add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_ALPHA_MESH, local_pos, (float *)g_faces_torch[dir], data.texture[dir], light);
			else
				add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_ALPHA_MESH, local_pos, (float *)g_faces_cactus[dir], data.texture[dir], light);
			++chunk->blocks_solid_alpha_amount;
		}
	}
}

/*
 * 'dirs' : array of e_card_dir, only check the faces in the direction of these; last should be -1;
*/
void	helper_pelper(t_chunk *chunk, t_chunk **neighbors, int *dirs, int *pos)
{
	int		index;
	t_block	*adj;
	
	index = get_block_index(pos[0], pos[1], pos[2]);
	if (is_gas(&chunk->blocks[index])) // <-- very important, im not sure what happens if we are trying to render an air block;
		return ;
	adj = NULL;
	// TODO : this needs to be moved to the 'add_block_to_correct_mesh()';
	if (is_flora(&chunk->blocks[index]))
	{
		if (!(chunk->blocks[index].visible_faces & g_visible_faces[6]))
		{
			chunk->blocks[index].visible_faces |= g_visible_faces[6];
			int light = (int)(ft_pow(0.9f, 15 - ft_clamp(chunk->blocks[index].light_lvl, 0, 15)) * 100.0f) * (g_face_light[DIR_UP] / 100.0f);
			add_to_chunk_mesh_v2(&chunk->meshes, FLORA_MESH, pos, (float *)g_flora_faces[0], g_block_data[chunk->blocks[index].type].texture[0], light);
			add_to_chunk_mesh_v2(&chunk->meshes, FLORA_MESH, pos, (float *)g_flora_faces[1], g_block_data[chunk->blocks[index].type].texture[1], light);
			++chunk->blocks_flora_amount;
			chunk->has_visible_blocks = 1;
		}
	}
	else // these are the solid blocks;
	{
		for (int dir = 0; dirs[dir] != -1; dir++)
		{
			adj = get_block_in_dir(chunk, neighbors[dirs[dir]], pos, dirs[dir]);
			if (adj && (!is_solid(adj) || is_solid_alpha(&chunk->blocks[index]))) // add to mesh if adjacent block isnt solid;
			{
				// Dont add to mesh if face already in it;
				if (!(chunk->blocks[index].visible_faces & g_visible_faces[dirs[dir]]))
				{
					chunk->blocks[index].visible_faces |= g_visible_faces[dirs[dir]];
					add_block_to_correct_mesh(chunk, &chunk->blocks[index], adj, pos, dirs[dir]);
					chunk->has_visible_blocks = 1;
				}
			}
			else
				chunk->blocks[index].visible_faces &= ~g_visible_faces[dirs[dir]];
		}
	}
}

/*
 * For each block in chunk, check all its adjacent blocks,
 *	if touching air, add that face to the chunk's mesh.
 *
 * This func resets the mesh before doing anything;
*/
void	get_blocks_visible(t_chunk *chunk)
{
	chunk->has_visible_blocks = 0;

	chunk->blocks_solid_amount = 0;
	chunk->blocks_flora_amount = 0;
	chunk->blocks_fluid_amount = 0;
	chunk->blocks_solid_alpha_amount = 0;

	if (!chunk->has_blocks)
		return ;

	// TODO: figure out a way to remove this;
	t_chunk *neighbors[6];
	for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; ++dir, ++i)
		neighbors[i] = NULL;

	// TODO: remove this, try to integrate it to the loop under this;
	// Reset visible faces;
	for (int i = 0; i < chunk->block_amount; i++)
		chunk->blocks[i].visible_faces = 0;
	
	int	all_dirs[] = {DIR_NORTH, DIR_EAST, DIR_SOUTH, DIR_WEST, DIR_UP, DIR_DOWN, -1};

	// First check all the blocks inside the chunk, all face directions;
	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			for (int z = 0; z < CHUNK_BREADTH; z++)
			{
				helper_pelper(chunk, neighbors, all_dirs, (int []){x, y, z});
			}
		}
	}
}

/*
 * TODO: only update blocks that have not already been added to the mesh;
*/
void	update_chunk_border_visible_blocks(t_chunk *chunk)
{
	t_block	*blocks;

	if (!chunk->has_blocks)
		return ;
	blocks = chunk->blocks;

	// Get all neighbors of current chunk;
	t_chunk *neighbors[6];
	for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; ++dir, ++i)
	{
		neighbors[i] = get_adjacent_chunk(chunk->info, chunk, (float *)g_card_dir[dir]);
		if (!chunk_has_non_solid(neighbors[i]))
			neighbors[i] = NULL;
	}

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		// front && back
		for (int x = 0; x < CHUNK_WIDTH; x++)
		{
			if (neighbors[DIR_NORTH])
				helper_pelper(chunk, neighbors, (int []){DIR_NORTH, -1}, (int []){x, y, 0});
			if (neighbors[DIR_SOUTH])
				helper_pelper(chunk, neighbors, (int []){DIR_SOUTH, -1}, (int []){x, y, CHUNK_BREADTH - 1});
		}
		// left && right
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			if (neighbors[DIR_WEST])
				helper_pelper(chunk, neighbors, (int []){DIR_WEST, -1}, (int []){0, y, z});
			if (neighbors[DIR_EAST])
				helper_pelper(chunk, neighbors, (int []){DIR_EAST, -1}, (int []){CHUNK_WIDTH - 1, y, z});
		}
	}
	// down && up
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			if (neighbors[DIR_DOWN])
				helper_pelper(chunk, neighbors, (int []){DIR_DOWN, -1}, (int []){x, 0, z});
			if (neighbors[DIR_UP])
				helper_pelper(chunk, neighbors, (int []){DIR_UP, -1}, (int []){x, CHUNK_HEIGHT - 1, z});
		}
	}
}

unsigned long int	get_chunk_hash_key(int *coords)
{
	char	str[256];
	unsigned long int	hash = 5381;

	ft_b_itoa(coords[0], str);
	ft_b_itoa(coords[1], str + ft_strlen(str));
	ft_b_itoa(coords[2], str + ft_strlen(str));
	int i = -1;
	while (str[++i])
		hash = ((hash << 5) + hash) + str[i];
	return (hash);
}

void	generate_chunk(t_chunk *chunk, int *coord, int *noise_map)
{
	for (int i = 0; i < 3; i++)
		chunk->coordinate[i] = coord[i];
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * CHUNK_SIZE_X,
		chunk->coordinate[1] * CHUNK_SIZE_Y,
		chunk->coordinate[2] * CHUNK_SIZE_Z);

	chunk_aabb_update(chunk);

	// Generate Chunks	
	chunk->block_amount = chunk_gen(chunk, noise_map); // should always return max amount of blocks in a chunk;

	chunk->event_block_amount = 0;
	chunk->light_emitters = 0;

	chunk->needs_to_update = 1;

	chunk->update_water = 0;
}

/*
 * Get all coordinates in a radius around start coords;
 * On a 2d plane;
 * 
 * Returns res amount;
*/
int	get_surrounding_coords(int *res, int x, int z, int r)
{
	int	sx = x - r;
	int	sz = z - r;
	int	amount = 0;

	for (int i = 0; i <= r + r; i++)
	{
		for (int j = 0; j <= r + r; j++)
		{
			res[amount * 2 + 0] = sx + i;
			res[amount * 2 + 1] = sz + j;
			++amount;
		}
	}
	return (amount);
}

/*
 * 'player_chunk' : [3](xyz);
*/
int	get_chunk_column_to_regen(t_chunk_col *chunk_cols, int *player_chunk, int *out_col_indices, int (*out_col_coords)[2], int max_get)
{
	int	start_coord[2];
	int	reload_amount = 0;
	int	chunk_amount = 0;
	int	found;

	start_coord[0] = player_chunk[0] - (RENDER_DISTANCE / 2);
	start_coord[1] = player_chunk[2] - (RENDER_DISTANCE / 2);
	for (int x = start_coord[0], x_amount = 0; x_amount < RENDER_DISTANCE; x++, x_amount++)
	{
		for (int z = start_coord[1], z_amount = 0; z_amount < RENDER_DISTANCE; z++, z_amount++)
		{
			found = 0;
			for (int i = 0; i < RENDER_DISTANCE * RENDER_DISTANCE; i++)
			{
				if (chunk_cols[i].coordinate[0] == x &&
					chunk_cols[i].coordinate[1] == z)
				{
					found = 1;
					break ;
				}
			}
			if (!found)
			{
				out_col_coords[reload_amount][0] = x;
				out_col_coords[reload_amount][1] = z;
				++reload_amount;
			}
			if (reload_amount >= max_get)
				break;
		}
		if (reload_amount >= max_get)
			break;
	}

	for (int i = 0; i < RENDER_DISTANCE * RENDER_DISTANCE; i++)
	{
		for (int x = start_coord[0], x_amount = 0; x_amount < RENDER_DISTANCE; x++, x_amount++)
		{
			for (int z = start_coord[1], z_amount = 0; z_amount < RENDER_DISTANCE; z++, z_amount++)
			{
				found = 0;
				if (chunk_cols[i].coordinate[0] == x &&
					chunk_cols[i].coordinate[1] == z)
				{
					found = 1;
					break ;
				}
			}
			if (found)
				break ;
		}
		if (!found)
		{
			out_col_indices[chunk_amount] = i;
			++chunk_amount;
		}
		if (chunk_amount >= max_get)
			break;
	}
	return (reload_amount);
}

/*
 * Check which chunks are not going to be in the next iteration of
 *	loaded chunks, save those to 'reload_these_chunks' and when starting
 *	to update the new chunks that are going to be loaded, and put the
 *	new chunk info into those 'chunks' indices;
 * Takes 0.000000 seconds;
 * 
 * Figures out which chunks will be loaded into which chunks;
*/
int	get_chunks_to_reload_v2(int *these, int (*into_these)[2], int *start_coord, t_chunk_info *info, int *player_chunk_v3, int max_get)
{
	int	reload_amount = 0;
	int	found;

	start_coord[0] = player_chunk_v3[0] - (RENDER_DISTANCE / 2);
	start_coord[1] = 0;
	start_coord[2] = player_chunk_v3[2] - (RENDER_DISTANCE / 2);

	// Find chunk that shouldnt be loaded anymore;
	for (int i = 0; i < CHUNKS_LOADED; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < RENDER_DISTANCE; x++, x_amount++)
		{
			for (int z = start_coord[2], z_amount = 0; z_amount < RENDER_DISTANCE; z++, z_amount++)
			{
				if (info->chunks[i].coordinate[0] == x && info->chunks[i].coordinate[2] == z)
					found = 1;
				if (found)
					break;
			}
			if (found)
				break ;
		}
		if (!found)
		{
			these[reload_amount] = i;
			++reload_amount;
		}
		if (reload_amount >= max_get * CHUNKS_PER_COLUMN)
			break;
	}

	// Find coords that should be loaded and arent, and add those to the 'into_these' arr;
	int	coord_amount = 0;
	for (int x = start_coord[0], x_amount = 0; x_amount < RENDER_DISTANCE; x++, x_amount++)
	{
		for (int z = start_coord[2], z_amount = 0; z_amount < RENDER_DISTANCE; z++, z_amount++)
		{
			found = 0;
			for (int i = 0; i < CHUNKS_LOADED; i++)
			{
				if (info->chunks[i].coordinate[0] == x &&
					info->chunks[i].coordinate[2] == z)
				{
					found = 1;
					break;
				}
			}
			if (!found)
			{	
				into_these[coord_amount][0] = x;
				into_these[coord_amount][1] = z;
				coord_amount++;
			}
			if (coord_amount >= max_get)
				break;
		}
		if (coord_amount >= max_get)
			break;
	}
	return (reload_amount);
}

int	get_chunk_water_amount(t_chunk *chunk)
{
	int	total;

	total = 0;
	total += chunk->block_palette[(FLUID_WATER)];
	total += chunk->block_palette[(FLUID_WATER_1)];
	total += chunk->block_palette[(FLUID_WATER_2)];
	total += chunk->block_palette[(FLUID_WATER_3)];
	total += chunk->block_palette[(FLUID_WATER_4)];
	total += chunk->block_palette[(FLUID_WATER_5)];
	total += chunk->block_palette[(FLUID_WATER_6)];
	total += chunk->block_palette[(FLUID_WATER_7)];
	return (total);
}

/*
 * Counts the amount of every block in the chunk;
*/
void	update_chunk_block_palette(t_chunk *chunk)
{
	memset(chunk->block_palette, 0, sizeof(int) * BLOCK_TYPE_AMOUNT);
	for (int i = 0; i < chunk->block_amount; i++)
	{
		++chunk->block_palette[chunk->blocks[i].type];
	}
}

// DEBUG
void print_block_palette(t_chunk *chunk)
{
	for (int i = 0; i < BLOCK_TYPE_AMOUNT; i++)
		ft_printf("%2d : %5d [%s]\n", i, chunk->block_palette[i], get_block_data_from_type(i).name);
}

/*
int	get_chunk_skylight_amount(t_chunk *chunk)
{
	int		skylights;
	int		index;
	int		top_chunk_pos[3];
	t_chunk	*top_chunk;
	
	top_chunk = get_chunk(chunk->info,
		vec3i_add(top_chunk_pos, chunk->coordinate, g_card_dir_int[DIR_UP]));
	// TODO : For now, just return if not the top most chunk, we have to look into this later;
	if (top_chunk)
		return (0);
	skylights = 0;
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_WIDTH; z++)
		{
			index = get_block_index(x, CHUNK_HEIGHT - 1, z);
			if (is_gas(&chunk->blocks[index]))
				++skylights;
		}
	}
	return (skylights);
}
*/

void	update_chunk_event_blocks(t_chunk *chunk)
{
	t_block			*block;
	t_block_event	*event_block;
	int				local_pos[3];
	float			pos[3];

	chunk->event_block_amount = 0;
	chunk->update_water = 1;

	// Figure out how many event blocks we want;
	chunk->event_blocks_wanted = get_chunk_water_amount(chunk);
	chunk->event_blocks_wanted += chunk->block_palette[BLOCK_TNT];

	// No need doing anything if we dont have any event blocks for this chunk;
	if (chunk->event_blocks_wanted <= 0)
		return ;

	// Lets free and malloc event_blocks (we dont need to realloc, because that memcpy from the old... and we dont need that);
	if (chunk->event_blocks_wanted > chunk->event_blocks_allocated)
	{
		LG_WARN("Event blocks (%d) realloced. %d => %d", chunk->event_block_amount, chunk->event_blocks_allocated, chunk->event_blocks_wanted);
		chunk->event_blocks_allocated = chunk->event_blocks_wanted;
		free(chunk->event_blocks);
		chunk->event_blocks = malloc(sizeof(t_block_event) * chunk->event_blocks_allocated);
	}

	for (int i = 0; i < chunk->block_amount; i++)
	{
		event_block = NULL;
		block = &chunk->blocks[i];
		if ((is_water(block) && block->visible_faces) || block->type == BLOCK_TNT)
		{
			get_block_local_pos_from_index(local_pos, i);
			get_block_world_pos(pos, chunk->world_coordinate, local_pos);
			event_block = &chunk->event_blocks[chunk->event_block_amount++];
			event_block->block = block;
			vec3_assign(event_block->pos, pos);
		}
		if (is_water(block) && event_block)
		{
			// Here you should add water specific stuff;
			event_block->flow_dir = -1;
			event_block->dist_to_down = INT_MAX;
			event_block->statique = 0;
		}
		else if (block->type == BLOCK_TNT && event_block)
		{
			// Here you should add tnt specific stuff;
		}
	}
}

void	update_chunk(t_chunk *chunk)
{
	update_chunk_block_palette(chunk);
	update_chunk_visible_blocks(chunk);
	update_chunk_event_blocks(chunk);
	if (chunk->event_block_amount > 0)
		ft_printf("event : blocks(%d) / wanted(%d) / allocated(%d)\n", chunk->event_block_amount, chunk->event_blocks_wanted, chunk->event_blocks_allocated);
	chunk->was_updated = 1;
	chunk->needs_to_update = 0;
	chunk->secondary_update = 1;
}

/*
 * 'event' meaning : we are eventing the chunk;
*/
void	event_chunk(t_chunk *chunk)
{
	if (chunk->event_block_amount <= 0)
		return ;

	// water_flow() can re-enable update_water, so we are making duplicate variable here;
	int	should_we_update_water = chunk->update_water;

	chunk->update_water = 0;
	for (int j = 0; j < chunk->event_block_amount; j++)
	{
		// Water block events;
		if (should_we_update_water && is_water(chunk->event_blocks[j].block))
		{
			water_flow(chunk->info, &chunk->event_blocks[j]);
			water_remove(chunk->info, &chunk->event_blocks[j]);
			chunk->event_blocks[j].statique = 1;
		}
		else if (chunk->event_blocks[j].block->type == BLOCK_TNT)
			tnt_explosion(chunk->info, chunk, &chunk->event_blocks[j]);
	}
}

void	regenerate_chunk_column(t_chunk_col *column, int coord[2], int seed)
{
	int	noise_map[CHUNK_WIDTH * CHUNK_BREADTH];

	create_noise_map(noise_map, CHUNK_WIDTH, CHUNK_BREADTH, coord[0], coord[1], seed);
	for (int i = 0; i < CHUNKS_PER_COLUMN; i++)
		generate_chunk(column->chunks[i], (int []){coord[0], i, coord[1]}, noise_map);
	column->coordinate[0] = column->chunks[0]->coordinate[0];
	column->coordinate[1] = column->chunks[0]->coordinate[2];
	column->world_coordinate[0] = column->chunks[0]->world_coordinate[0];
	column->world_coordinate[1] = column->chunks[0]->world_coordinate[2];
	column->update_structures = 1;
}

void	show_chunk_borders(t_chunk *chunk, t_camera *camera, float *col)
{
	render_aabb(&chunk->aabb, camera, col);
}

void	render_aabb(t_aabb *a, t_camera *camera, float *col)
{
	// VERTICAL LINES
	render_3d_line(
		(float []){a->min[0], a->min[1], a->min[2]},
		(float []){a->min[0], a->max[1], a->min[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		col, camera->view, camera->projection);

	// HORIZONTAL LINES Bottom
	render_3d_line(
		(float []){a->min[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->min[1], a->min[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->min[1], a->max[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->min[1], a->max[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->min[1], a->min[2]},
		col, camera->view, camera->projection);
	// TOP
	render_3d_line(
		(float []){a->min[0], a->max[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->max[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->max[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		col, camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->max[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->min[2]},
		col, camera->view, camera->projection);
}

/*
 * I think this has to be called after all the chunk creation since we need all the adjacent
 *	chunks to already exist, and if the update_chunk is multithreaded we might not have 
 *	created the needed chunks yet;
*/
void	update_chunk_visible_blocks(t_chunk *chunk)
{
	reset_chunk_mesh_v2(&chunk->meshes);

	get_blocks_visible(chunk);
}

/*
 * Makes all the values 0, doesnt free anything;
*/
void	reset_chunk_mesh_v2(t_chunk_mesh_v2 *mesh)
{
	mesh->vertices_amount = 0;
	mesh->texture_id_amount = 0;
	mesh->index_amount = 0;
	for (int i = 0; i < mesh->amount; i++)
		mesh->indices_amount[i] = 0;
}

/*
 * Creates all the opengl stuff that needs to be created only once;
*/
void	init_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, GLuint shader, int amount)
{
	GLuint	vbo[2];
	int		error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);
	
	mesh->vertices_allocated = 162576;
	mesh->vertices = malloc(sizeof(float) * mesh->vertices_allocated);
	mesh->vertices_amount = 0;

	mesh->texture_ids_allocated = 54536; 
	mesh->texture_ids = malloc(sizeof(int) * mesh->texture_ids_allocated);
	mesh->texture_id_amount = 0;

	mesh->amount = amount;
	mesh->indices = malloc(sizeof(unsigned int *) * mesh->amount);
	mesh->indices_allocated = malloc(sizeof(int) * mesh->amount);
	mesh->indices_amount = malloc(sizeof(int) * mesh->amount);
	mesh->index_amount = 0;
	for (int i = 0; i < mesh->amount; i++)
	{
		mesh->indices_allocated[i] = 68688;
		mesh->indices[i] = malloc(sizeof(unsigned int) * mesh->indices_allocated[i]);
		mesh->indices_amount[i] = 0;
	}

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // tex

	glGenBuffers(2, vbo);
	mesh->vbo_pos = vbo[0];
	mesh->vbo_texture_ids = vbo[1];

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texture_ids);
	glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), NULL);

	mesh->ebo = malloc(sizeof(unsigned int) * mesh->amount);
	glGenBuffers(mesh->amount, mesh->ebo);

	mesh->shader = shader;
	// Get uniforms
	mesh->uniform_chunk_pos = glGetUniformLocation(mesh->shader, "chunkPos");
	mesh->uniform_view = glGetUniformLocation(mesh->shader, "view");
	mesh->uniform_proj = glGetUniformLocation(mesh->shader, "projection");
	mesh->uniform_color_tint = glGetUniformLocation(mesh->shader, "colorTint");

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * 'coordinate' world coordinate of the chunk;
*/
void	render_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int mesh_type, float *coordinate, t_camera *camera)
{
	int	error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);
	
	glUseProgram(mesh->shader);
	glUniformMatrix4fv(mesh->uniform_view, 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(mesh->uniform_proj, 1, GL_FALSE, &camera->projection[0]);

	glUniform3fv(mesh->uniform_chunk_pos, 1, &coordinate[0]);
	// Night tint 
//	glUniform3fv(mesh->uniform_color_tint, 1, (float []){0.2, 0.40, 0.6});
	// day tint
	glUniform3fv(mesh->uniform_color_tint, 1, (float []){1, 1, 1});

	glBindVertexArray(mesh->vao);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	if (mesh->indices_amount[mesh_type] > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo[mesh_type]);
		glDrawElements(GL_TRIANGLES, mesh->indices_amount[mesh_type],
			GL_UNSIGNED_INT, NULL);
	}
	else
		LG_WARN("We can\'t render mesh with no indices.");

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * Sends the mesh info to the GPU;
 *
 * You have to call this function on the main thread;
*/
void	update_chunk_mesh(t_chunk_mesh_v2 *mesh)
{
	int error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	glBindVertexArray(mesh->vao);

	// Pos
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices_amount * sizeof(float),
		&mesh->vertices[0], GL_STATIC_DRAW);

	error = glGetError();
	if (error)
	{
		LG_INFO("Vertices Amount : %d", mesh->vertices_amount);
		LG_ERROR("(%d)", error);
	}

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texture_ids);
	glBufferData(GL_ARRAY_BUFFER, mesh->texture_id_amount * sizeof(int),
		&mesh->texture_ids[0], GL_STATIC_DRAW);

	// EBO
	for (int i = 0; i < mesh->amount; i++)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			mesh->indices_amount[i] * sizeof(unsigned int),
			&mesh->indices[i][0], GL_STATIC_DRAW);
	}
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}


/*
 * 'coord' position of block in the chunk->blocks array;
 *
 * You give in the mesh you want to add the face to;
 * Otherwise same as the old version;
 * 
 * You give either solid mesh or liquid mesh to this;
*/
void	add_to_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int mesh_type, int *coord, float *face_vertices, int texture_id, int light)
{
	// Vertices and Texture
	if (mesh->vertices_allocated < mesh->vertices_amount + 12)
	{
		mesh->vertices_allocated += 2048;
		mesh->vertices = realloc(mesh->vertices, sizeof(float) * mesh->vertices_allocated);
		LG_WARN("Reallocating Vertices from %d to %d", mesh->vertices_allocated - 2048, mesh->vertices_allocated);
	}

	if (mesh->texture_ids_allocated < mesh->texture_id_amount + 4)
	{
		mesh->texture_ids_allocated += 2048;
		mesh->texture_ids = realloc(mesh->texture_ids, sizeof(int) * mesh->texture_ids_allocated);
		LG_WARN("Reallocating Texture Ids from %d to %d", mesh->texture_ids_allocated - 2048, mesh->texture_ids_allocated);
	}

	int ind = 0;
	int	tex = 0;
	for (int i = 0; i < 4; i++)
	{
		ind = 3 * i;
		mesh->vertices[mesh->vertices_amount + ind + 0] = (face_vertices[ind + 0] * BLOCK_SCALE) + coord[0];
		mesh->vertices[mesh->vertices_amount + ind + 1] = (face_vertices[ind + 1] * BLOCK_SCALE) + coord[1];
		mesh->vertices[mesh->vertices_amount + ind + 2] = (face_vertices[ind + 2] * BLOCK_SCALE) + coord[2];

		tex = texture_id | (i << 16) | (light << 20);
		mesh->texture_ids[mesh->texture_id_amount + i] = tex;
	}

	mesh->vertices_amount += 4 * 3;
	mesh->texture_id_amount += 4;

	// Indices
	if (mesh->indices_allocated[mesh_type] < mesh->indices_amount[mesh_type] + 6)
	{
		mesh->indices_allocated[mesh_type] += 2048;
		mesh->indices[mesh_type] = realloc(mesh->indices[mesh_type], sizeof(unsigned int) * mesh->indices_allocated[mesh_type]);
		LG_WARN("Reallocating Indices from %d to %d", mesh->indices_allocated[mesh_type] - 2048, mesh->indices_allocated[mesh_type]);
	}

	mesh->indices[mesh_type][mesh->indices_amount[mesh_type] + 0] = mesh->index_amount;
	mesh->indices[mesh_type][mesh->indices_amount[mesh_type] + 1] = mesh->index_amount + 1;
	mesh->indices[mesh_type][mesh->indices_amount[mesh_type] + 2] = mesh->index_amount + 2;

	mesh->indices[mesh_type][mesh->indices_amount[mesh_type] + 3] = mesh->index_amount;
	mesh->indices[mesh_type][mesh->indices_amount[mesh_type] + 4] = mesh->index_amount + 2;
	mesh->indices[mesh_type][mesh->indices_amount[mesh_type] + 5] = mesh->index_amount + 3;

	mesh->indices_amount[mesh_type] += 6;
	mesh->index_amount += 4;
}

int	is_hovering_block(float *block_pos, float *point, int *face, float (*faces)[12], int face_amount)
{
	float	p1[3];
	float	p2[3];
	float	p3[3];

	for (int f = 0; f < face_amount; f++)
	{
		p1[0] = (faces[f][0] * 0.5f) + block_pos[0];
		p1[1] = (faces[f][1] * 0.5f) + block_pos[1];
		p1[2] = (faces[f][2] * 0.5f) + block_pos[2];

		p2[0] = (faces[f][3] * 0.5f) + block_pos[0];
		p2[1] = (faces[f][4] * 0.5f) + block_pos[1];
		p2[2] = (faces[f][5] * 0.5f) + block_pos[2];

		p3[0] = (faces[f][6] * 0.5f) + block_pos[0];
		p3[1] = (faces[f][7] * 0.5f) + block_pos[1];
		p3[2] = (faces[f][8] * 0.5f) + block_pos[2];
		if (point_in_triangle(point, p1, p2, p3))
		{
			*face = f;
			return (1);
		}

		p1[0] = (faces[f][0] * 0.5f) + block_pos[0];
		p1[1] = (faces[f][1] * 0.5f) + block_pos[1];
		p1[2] = (faces[f][2] * 0.5f) + block_pos[2];

		p2[0] = (faces[f][6] * 0.5f) + block_pos[0];
		p2[1] = (faces[f][7] * 0.5f) + block_pos[1];
		p2[2] = (faces[f][8] * 0.5f) + block_pos[2];

		p3[0] = (faces[f][9] * 0.5f) + block_pos[0];
		p3[1] = (faces[f][10] * 0.5f) + block_pos[1];
		p3[2] = (faces[f][11] * 0.5f) + block_pos[2];
		if (point_in_triangle(point, p1, p2, p3))
		{
			*face = f;
			return (1);
		}
	}
	return (0);
}

/*
 * If block found : we save the position of the block in 'block_pos'
 *	(which is the world position of the block) and
 *	the face, of the block the point belongs to, in 'face' and
 *	return the pointer of the block;
*/
t_block	*get_block_from_chunk(t_chunk *chunk, float *point, float *block_pos, int *face)
{
	float	block_world[3];
	int		blocal[3];
	int		i;
	float	**faces;
	int		face_amount;

	i = 0;
	for (; i < chunk->block_amount; i++)
	{
		if (chunk->blocks[i].visible_faces == 0)
			continue ;

		// skip if gas or fluid;
		if (is_gas(&chunk->blocks[i]) || is_fluid(&chunk->blocks[i]))
			continue;

		get_block_local_pos_from_index(blocal, i);
		get_block_world_pos(block_world, chunk->world_coordinate, blocal);

		face_amount = 0;
		if (is_solid(&chunk->blocks[i]) ||
			(is_solid(&chunk->blocks[i]) && chunk->blocks[i].type == BLOCK_ALPHA_OAK_LEAF))
		{
			face_amount = 6;
			faces = g_faces;
		}
		else if (is_flora(&chunk->blocks[i]))
		{
			face_amount = 2;
			faces = g_flora_faces;
		}
		else if (is_solid_alpha(&chunk->blocks[i]))
		{
			face_amount = 6;
			faces = g_faces_cactus;
		}

		if (is_hovering_block(block_world, point, face, faces, face_amount))
		{
			vec3_assign(block_pos, block_world);
			return (&chunk->blocks[i]);
		}
	}
	return (NULL);
}

/*
 * Returns amount of collisions;
 * Result of intersection points are stored in 'intersect_point';
 * 
 * TODO: This should probably returns either an array of points being hit,
 * 	or just the closest one....?
*/
int	chunk_mesh_collision_v2(float *orig, float *dir, t_chunk_mesh_v2 *mesh, int mesh_type, float *world_coords, float reach, float intersect_point[16][3])
{
	float			*vertices;
	unsigned int	*indices;
	float			p1[3];
	float			p2[3];
	float			p3[3];
	float			norm_dir[3];
	int				collisions = 0;
	int				triangle_amount;
	float			dist;

	vec3_normalize(norm_dir, dir);
	vertices = mesh->vertices;
	indices = mesh->indices[mesh_type];
	triangle_amount = mesh->indices_amount[mesh_type] / 3;
	for (int i = 0; i < triangle_amount; i++)
	{
		int k = i * 3;
		int k0 = indices[k + 0] * 3;
		int k1 = indices[k + 1] * 3;
		int k2 = indices[k + 2] * 3;
		vec3_new(p1, vertices[k0 + 0], vertices[k0 + 1], vertices[k0 + 2]);
		vec3_new(p2, vertices[k1 + 0], vertices[k1 + 1], vertices[k1 + 2]);
		vec3_new(p3, vertices[k2 + 0], vertices[k2 + 1], vertices[k2 + 2]);
		
		// We are adding chunk woorld coordinate to points, to get the world point coordinate;
		vec3_add(p1, p1, world_coords);
		vec3_add(p2, p2, world_coords);
		vec3_add(p3, p3, world_coords);
		if (ray_triangle_intersect(orig, dir, p1, p2, p3, intersect_point[collisions], &dist))
			if (dist <= reach + EPSILON)
				collisions += 1;
	}
	return (collisions);
}

/*
 * Same as the chunk_mesh_collision, but instead of saving the intersect_points,
 *	it saves the normals of the faces it collides with;
*/
int	chunk_mesh_collision_v56(float *orig, float *dir, t_chunk *chunk, float reach, float intersect_points[16][3], float intersect_normals[16][3])
{
	float			*vertices;
	unsigned int	*indices;
	float			p1[3];
	float			p2[3];
	float			p3[3];
	float			norm_dir[3];
	int				collisions = 0;
	float			dist;

	vec3_normalize(norm_dir, dir);
	vertices = chunk->meshes.vertices;
	indices = chunk->meshes.indices[BLOCK_MESH];
	for (int i = 0; i < chunk->meshes.indices_amount[BLOCK_MESH] / 3; i++)
	{
		int k = i * 3;
		vec3_new(p1,
			vertices[indices[k + 0] * 3 + 0],
			vertices[indices[k + 0] * 3 + 1],
			vertices[indices[k + 0] * 3 + 2]);
		vec3_new(p2,
			vertices[indices[k + 1] * 3 + 0],
			vertices[indices[k + 1] * 3 + 1],
			vertices[indices[k + 1] * 3 + 2]);
		vec3_new(p3,
			vertices[indices[k + 2] * 3 + 0],
			vertices[indices[k + 2] * 3 + 1],
			vertices[indices[k + 2] * 3 + 2]);
		
		// We have to add the chunk position to the chunk mesh, since that is 
		//		what we are doing in the shader;
		vec3_add(p1, p1, chunk->world_coordinate);
		vec3_add(p2, p2, chunk->world_coordinate);
		vec3_add(p3, p3, chunk->world_coordinate);
		if (ray_triangle_intersect(orig, dir, p1, p2, p3, intersect_points[collisions], &dist))
		{
			if (dist <= reach + EPSILON)
			{
				triangle_face_normal(intersect_normals[collisions], p1, p2, p3);
				collisions += 1;
			}
		}
	}
	return (collisions);
}

void	render_block_outline(float *pos, float *color, float *view, float *projection)
{
	float	p1[3]; // top front left
	float	p2[3]; // top back left
	float	p3[3]; // top back right
	float	p4[3]; // top front right

	p1[0] = (g_faces[DIR_UP][0] * 0.5) + pos[0];
	p1[1] = (g_faces[DIR_UP][1] * 0.5) + pos[1];
	p1[2] = (g_faces[DIR_UP][2] * 0.5) + pos[2];

	p2[0] = (g_faces[DIR_UP][3] * 0.5) + pos[0];
	p2[1] = (g_faces[DIR_UP][4] * 0.5) + pos[1];
	p2[2] = (g_faces[DIR_UP][5] * 0.5) + pos[2];

	p3[0] = (g_faces[DIR_UP][6] * 0.5) + pos[0];
	p3[1] = (g_faces[DIR_UP][7] * 0.5) + pos[1];
	p3[2] = (g_faces[DIR_UP][8] * 0.5) + pos[2];

	p4[0] = (g_faces[DIR_UP][9] * 0.5) + pos[0];
	p4[1] = (g_faces[DIR_UP][10] * 0.5) + pos[1];
	p4[2] = (g_faces[DIR_UP][11] * 0.5) + pos[2];

	float	b1[3]; // bot front left
	float	b2[3]; // bot front right
	float	b3[3]; // bot back right
	float	b4[3]; // bot back left

	b1[0] = (g_faces[DIR_DOWN][0] * 0.5) + pos[0];
	b1[1] = (g_faces[DIR_DOWN][1] * 0.5) + pos[1];
	b1[2] = (g_faces[DIR_DOWN][2] * 0.5) + pos[2];

	b2[0] = (g_faces[DIR_DOWN][3] * 0.5) + pos[0];
	b2[1] = (g_faces[DIR_DOWN][4] * 0.5) + pos[1];
	b2[2] = (g_faces[DIR_DOWN][5] * 0.5) + pos[2];

	b3[0] = (g_faces[DIR_DOWN][6] * 0.5) + pos[0];
	b3[1] = (g_faces[DIR_DOWN][7] * 0.5) + pos[1];
	b3[2] = (g_faces[DIR_DOWN][8] * 0.5) + pos[2];

	b4[0] = (g_faces[DIR_DOWN][9] * 0.5) + pos[0];
	b4[1] = (g_faces[DIR_DOWN][10] * 0.5) + pos[1];
	b4[2] = (g_faces[DIR_DOWN][11] * 0.5) + pos[2];

	// TOP
	render_3d_line(p1, p2, color, view, projection);
	render_3d_line(p2, p3, color, view, projection);
	render_3d_line(p3, p4, color, view, projection);
	render_3d_line(p4, p1, color, view, projection);

	// BOT
	render_3d_line(b1, b2, color, view, projection);
	render_3d_line(b2, b3, color, view, projection);
	render_3d_line(b3, b4, color, view, projection);
	render_3d_line(b4, b1, color, view, projection);

	// BACK
	render_3d_line(p2, b4, color, view, projection);
	render_3d_line(p3, b3, color, view, projection);

	// FRONT
	render_3d_line(p1, b1, color, view, projection);
	render_3d_line(p4, b2, color, view, projection);
}

void	player_terrain_collision(float *res, float *pos, float *velocity, t_chunk_info *info)
{
	float	normed_velocity[3];
	float	player_intersect_normal[16][3];
	float	player_intersect_point[16][3];
	int		player_collision_amount = 0;
	float	velocity_dist = vec3_dist((float []){0, 0, 0}, velocity);
	int		player_chunk[3];
	float	final[3];

	vec3_assign(final, velocity);
	while (velocity_dist > EPSILON)
	{
		get_chunk_pos_from_world_pos(player_chunk, pos);
		player_collision_amount = 0;
		vec3_normalize(normed_velocity, final);
		for (int i = 0; i < CHUNKS_LOADED; i++)
		{
			if (!(info->chunks[i].blocks_solid_amount > 0 &&
				vec3i_dist(player_chunk, info->chunks[i].coordinate) < 2))
				continue ;	
			int colls = chunk_mesh_collision_v56(pos, normed_velocity, &info->chunks[i], velocity_dist, player_intersect_point + player_collision_amount, player_intersect_normal + player_collision_amount);
			player_collision_amount += colls;
		}
		if (player_collision_amount <= 0)
			break ;

		for (int i = 0; i < player_collision_amount; i++)
		{
			float	destination[3];
			vec3_add(destination, pos, final);

			float	distance = EPSILON;
			distance = vec3_dist(player_intersect_point[i], destination);	
			distance = ft_fmax(EPSILON, distance);

			float	new_destination[3];
			vec3_multiply_f(new_destination, player_intersect_normal[i], distance);
			vec3_sub(new_destination, destination, new_destination);

			float	tmp[3];
			vec3_sub(tmp, destination, new_destination);
			vec3_add(final, final, tmp);
		}
		velocity_dist = vec3_dist((float []){0, 0, 0}, final);
	}
	vec3_assign(res, final);
}

/*
 * Returns the block that was placed;
 *
 * Places block of type 'block_type' in the world position of 'world_pos' no
 *	matter what.
*/
t_block	*set_block_at_world_pos(t_chunk_info *info, float *world_pos, int block_type)
{
	int		block_local[3];
	int		chunk_pos[3];
	int		index;
	t_chunk	*chunk;

	get_chunk_pos_from_world_pos(chunk_pos, world_pos);
	chunk = get_chunk(info, chunk_pos);
	if (!chunk)
		return (NULL);
	get_block_local_pos_from_world_pos(block_local, world_pos);
	index = get_block_index(block_local[0], block_local[1], block_local[2]);
	if (chunk->blocks[index].type == block_type)
		return (NULL);
	chunk->blocks[index].type = block_type;
	chunk->needs_to_update = 1;
	if (!is_type_gas(block_type))
		chunk->has_blocks = 1;
	return (&chunk->blocks[index]);
}

void	set_block_at_world_pos_if_empty(t_chunk_info *info, float *world_pos, int block_type)
{
	int		block_local[3];
	int		chunk_pos[3];
	int		index;
	t_chunk	*chunk;

	get_chunk_pos_from_world_pos(chunk_pos, world_pos);
	chunk = get_chunk(info, chunk_pos);
	if (!chunk)
		return ;
	get_block_local_pos_from_world_pos(block_local, world_pos);
	index = get_block_index(block_local[0], block_local[1], block_local[2]);
	if (!is_gas(&chunk->blocks[index]))
		return ;
	chunk->blocks[index].type = block_type;
	chunk->needs_to_update = 1;
	chunk->has_blocks = 1;
}

int	get_block_type_at_world_pos(t_chunk_info *info, float *world_pos)
{
	float	under_block[3];
	int		block_local[3];
	int		chunk_pos[3];
	t_chunk	*chunk;
	int		index;

	vec3_new(under_block, world_pos[0], world_pos[1], world_pos[2]);
	get_chunk_pos_from_world_pos(chunk_pos, under_block);
	chunk = get_chunk(info, chunk_pos);
	if (!chunk)
		return (-1);
	get_block_local_pos_from_world_pos(block_local, under_block);
	index = get_block_index(block_local[0], block_local[1], block_local[2]);
	return (chunk->blocks[index].type);
}

/*
 * A no matter what cactus placer;
*/
void	create_cactus(t_chunk_info *info, float *world_pos)
{
	// Trunk;
	for (int i = 0; i < 3; i++)
		set_block_at_world_pos_if_empty(info,
			(float []){world_pos[0], world_pos[1] + i, world_pos[2]}, BLOCK_ALPHA_CACTUS);
}

/*
 * A no matter what tree placer;
*/
void	create_tree(t_chunk_info *info, float *world_pos)
{
	// Trunk;
	for (int i = 0; i < 6; i++)
		set_block_at_world_pos_if_empty(info,
			(float []){world_pos[0], world_pos[1] + i, world_pos[2]}, BLOCK_OAK_LOG);
	// top leaves
	for (int i = 0; i < 3; i++)
	{
		set_block_at_world_pos_if_empty(info,
			(float []){world_pos[0] - 1 + i, world_pos[1] + 6, world_pos[2]}, BLOCK_ALPHA_OAK_LEAF);
		set_block_at_world_pos_if_empty(info,
			(float []){world_pos[0], world_pos[1] + 6, world_pos[2] - 1 + i}, BLOCK_ALPHA_OAK_LEAF);
	}
	// 2nd top leaves
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (i == 1 && j == 1)
				continue ;
			set_block_at_world_pos_if_empty(info,
				(float []){world_pos[0] - 1 + i, world_pos[1] + 5, world_pos[2] - 1 + j},
				BLOCK_ALPHA_OAK_LEAF);
		}
	}
	// 3nd top leaves
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if ((i == 2 && j == 2) || // trunk in the middle
				((i == 0 || i == 4) && (j == 0 || j == 4))) // the corners of the row
				continue ;
			set_block_at_world_pos_if_empty(info,
				(float []){world_pos[0] - 2 + i, world_pos[1] + 4, world_pos[2] - 2 + j},
				BLOCK_ALPHA_OAK_LEAF);
		}
	}
	// 4nd top leaves
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if ((i == 2 && j == 2)) // trunk in the middle
				continue ;
			set_block_at_world_pos_if_empty(info,
				(float []){world_pos[0] - 2 + i, world_pos[1] + 3, world_pos[2] - 2 + j},
				BLOCK_ALPHA_OAK_LEAF);
		}
	}
	// 5nd top leaves
	// same as 3rd but lower
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if ((i == 2 && j == 2) || // trunk in the middle
				((i == 0 || i == 4) && (j == 0 || j == 4))) // the corners of the row
				continue ;
			set_block_at_world_pos_if_empty(info,
				(float []){world_pos[0] - 2 + i, world_pos[1] + 2, world_pos[2] - 2 + j},
				BLOCK_ALPHA_OAK_LEAF);
		}
	}
}

/*
 * 'world_pos' position of where tree should be placed in the world coordinate;
*/
void	tree_placer(t_chunk_info *info, float *world_pos)
{
	int	type;

	// We have to check that the block we are placing the tree on is dirt block;
	type = get_block_type_at_world_pos(info, world_pos);
	if (!is_type_gas(type))
		return ;
	type = get_block_type_at_world_pos(info,
		(float []){world_pos[0], world_pos[1] - 1.0f, world_pos[2]});
	if (type == BLOCK_DIRT || type == BLOCK_DIRT_GRASS)
		create_tree(info, world_pos);
	if (type == BLOCK_SAND)
		create_cactus(info, world_pos);
}

void	flora_placer(t_chunk_info *info, int type, float *world_pos)
{
	int	block_type;
	
// We have to check that the block we are placing on is dirt block;
	block_type = get_block_type_at_world_pos(info,
		(float []){world_pos[0], world_pos[1] - 1, world_pos[2]});
	if (block_type != BLOCK_DIRT && block_type != BLOCK_DIRT_GRASS)
		return ;
// And check that the block isnt already occupied by another block;
	block_type = get_block_type_at_world_pos(info,
		(float []){world_pos[0], world_pos[1], world_pos[2]});
	if (block_type != GAS_AIR)
		return ;

	set_block_at_world_pos(info,
		(float []){world_pos[0], world_pos[1], world_pos[2]}, type);
}

/*
 * Returns highest chunk at chunk->coordinate x and z;
*/
t_chunk *get_highest_chunk(t_chunk_info *info, int x, int z)
{
	for (int i = 0; i < CHUNK_COLUMNS; i++)
	{
		if (info->chunk_columns[i].coordinate[0] == x &&
			info->chunk_columns[i].coordinate[1] == z)
		{
			for (int j = CHUNKS_PER_COLUMN - 1; j >= 0; j--)
				if (info->chunk_columns[i].chunks[j]->has_blocks)
					return (info->chunk_columns[i].chunks[j]);
		}
	}
	return (NULL);
}

/*
 * Returns highest chunk at world coordinate  world_x / world_z;
 * Places the highest block into 'out_block' and the block's world y coordinate
 * 	into 'out_world_y';
*/
t_chunk *get_highest_chunk_with_block(t_chunk_info *info, t_block **out_block, float *out_world_y, float world_x, float world_z)
{
	int		chunk_local[3];
	int		block_local[3];
	float	block_world[3];

	get_chunk_pos_from_world_pos(chunk_local, (float []){world_x, 0, world_z});
	// first find the highets chunk with blocks;
	for (int i = 0; i < CHUNK_COLUMNS; i++)
	{
		if (info->chunk_columns[i].coordinate[0] == chunk_local[0] &&
			info->chunk_columns[i].coordinate[1] == chunk_local[2])
		{
			for (int j = CHUNKS_PER_COLUMN - 1; j >= 0; j--)
			{
				if (!info->chunk_columns[i].chunks[j]->has_blocks)
					continue ;	
				// Then loop from highest y to lowest y and double check if it has a world_x / world_z a block;
				get_block_local_pos_from_world_pos(block_local, (float []){world_x, 0, world_z});
				for (int y = CHUNK_HEIGHT - 1; y >= 0; y--)
				{
					int ind = get_block_index(block_local[0], y, block_local[2]);
					get_block_world_pos(block_world, info->chunk_columns[i].chunks[j]->world_coordinate, (int []){block_local[0], y, block_local[2]});
					if (!is_type_gas(info->chunk_columns[i].chunks[j]->blocks[ind].type))
					{
						get_block_world_pos(block_world, info->chunk_columns[i].chunks[j]->world_coordinate, (int []){block_local[0], y, block_local[2]});
						*out_block = &info->chunk_columns[i].chunks[j]->blocks[ind];
						*out_world_y = block_world[1];
						return (info->chunk_columns[i].chunks[j]);
					}
				}
			}
			return (NULL);
		}
	}
	return (NULL);
}

/*
 * Returns y of the block, saves the block in 'out_block';
 * We dont want the highest air block, since that is not an actual block;
*/
float	get_highest_block(t_chunk_info *info, t_block **out_block, float x, float z)
{
	static int count = 0;
	float	block_y;
	t_chunk	*highest_chunk;

	block_y = -1;
	highest_chunk = get_highest_chunk_with_block(info, out_block, &block_y, x, z);
	if (!highest_chunk || block_y == -1)
	{
		*out_block = NULL;
		return (-1); // error;
	}
	return (block_y);
}

/*
 * Returns highest y coordinate;
*/
float	get_highest_block_in_chunk(t_chunk *chunk, t_block **out_block, float x, float z)
{
	int		local_pos[3];
	int		index;

	for (int y = CHUNK_HEIGHT - 1; y >= 0; y--)
	{
		get_block_local_pos_from_world_pos(local_pos,
			(float []){x, y + chunk->world_coordinate[1], z});
		index = get_block_index(local_pos[0], local_pos[1], local_pos[2]);
		if (!is_type_gas(chunk->blocks[index].type))
		{
			*out_block = &chunk->blocks[index];
			return (y + chunk->world_coordinate[1]);
		}
	}
	*out_block = NULL;
	return (-1);
}

/*
 * NOTE : TODO : if you want to use this , first rewrite (im not deleting this
 *		so you can take example);
*/
float	get_highest_point_of_type(t_chunk_info *info, float x, float z, int type)
{
	int		chunk_pos[3];
	float	curr_highest = -1;
	t_chunk	*highest_chunk;

	get_chunk_pos_from_world_pos(chunk_pos, (float []){x, 0, z});
	highest_chunk = get_highest_chunk(info, chunk_pos[0], chunk_pos[2]);
	if (highest_chunk == NULL)
		return (-1); // error;
	for (int i = 0; i < CHUNK_BLOCK_AMOUNT; i++)
	{
		if (highest_chunk->blocks[i].type == type)
		{
			int		local[3];
			float	world[3];

			get_block_local_pos_from_index(local, i);
			get_block_world_pos(world, highest_chunk->world_coordinate, local);
			if (world[0] == x && world[2] == z)
				if (world[1] > curr_highest)
					curr_highest = world[1];
		}
	}
	return (curr_highest);
}

void	flora_decider(t_chunk_info *info, float chance, float *world_pos)
{
	if (chance < 1.25f)
	{
		if (chance < 1.125f)
			flora_placer(info, FLORA_FLOWER_YELLOW, world_pos);
		else
			flora_placer(info, FLORA_FLOWER_RED, world_pos);
	}
	else
		flora_placer(info, FLORA_GRASS, world_pos);
}

void	tree_gen(t_chunk_info *info, t_chunk_col *column)
{
	float	freq = 0.75f; //0.99f;
	float	pers = 0.5;
	t_block	*highest_block;
	t_chunk	*highest_chunk;

	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		float	block_world_x = fabs(column->world_coordinate[0] + x);
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			float	block_world_z = fabs(column->world_coordinate[1] + z);
			float	to_use_z = block_world_z * freq;

			highest_block = NULL;
			float block_world_y = get_highest_block(info, &highest_block, block_world_x, block_world_z);
			if (block_world_y == -1)
				continue ;
			float to_use_y = block_world_y * freq;
			float perper =
				octave_perlin(to_use_x, to_use_y, to_use_z, 1, pers) +
				octave_perlin(to_use_x, to_use_y, to_use_z, 2, pers) +
				octave_perlin(to_use_x, to_use_y, to_use_z, 4, pers) +
				octave_perlin(to_use_x, to_use_y, to_use_z, 8, pers);
			if (perper < 1.5f)
			{
				if (perper < 1.0f)
					tree_placer(info,
						(float []){block_world_x, block_world_y + 1, block_world_z});
				else
					flora_decider(info, perper,
						(float []){block_world_x, block_world_y + 1, block_world_z});
			}
		}
	}
}

/*
 * recrusion
*/
void	emit_sky_light(t_chunk_col *column, int chunk_index, int *coord, int light)
{
	t_chunk			*chunk;
	t_block			*block;
	t_block			*next_block;
	t_block_data	data;
	static int count = 0;

/*
	++count;
	if (count % 100 == 0)
		ft_printf("count : %d\n", count);
		*/
	if (light < 0 || light > 15)
		ft_printf(" [wtf! %d] ", light);
	if (light <= 0 ||
		coord[0] < 0 || coord[0] >= CHUNK_WIDTH ||
		coord[2] < 0 || coord[2] >= CHUNK_BREADTH)
		return ;
	if (coord[1] < 0)
	{
		coord[1] = CHUNK_HEIGHT - 1;
		chunk_index--;
	}
	else if (coord[1] >= CHUNK_HEIGHT)
	{
		coord[1] = 0;
		chunk_index++;
	}
	if (chunk_index < 0 || chunk_index >= CHUNKS_PER_COLUMN)
		return ;
	chunk = column->chunks[chunk_index];
	block = &chunk->blocks[get_block_index(coord[0], coord[1], coord[2])];
	if (light > block->light_lvl)
		block->light_lvl = ft_clamp(light, 0, 15);
	else // if block doesnt want more light, we can assume other blocks in that direction doesnt either;
		return ;
	chunk->needs_to_update = 1;
	if (block->light_lvl == 1)
		return ;

	data = get_block_data(block);
/*
*/
	int	new_light = ft_max(ft_clamp(block->light_lvl + data.light_emit, 0, 15) - 1, 0);
	emit_sky_light(column, chunk_index, (int []){coord[0], coord[1] + 1, coord[2]}, new_light);
	emit_sky_light(column, chunk_index, (int []){coord[0], coord[1] - 1, coord[2]}, new_light);
	emit_sky_light(column, chunk_index, (int []){coord[0] + 1, coord[1], coord[2]}, new_light);
	emit_sky_light(column, chunk_index, (int []){coord[0] - 1, coord[1], coord[2]}, new_light);
	emit_sky_light(column, chunk_index, (int []){coord[0], coord[1], coord[2] + 1}, new_light);
	emit_sky_light(column, chunk_index, (int []){coord[0], coord[1], coord[2] - 1}, new_light);
	/*
	*/
/*
*/
}

/*
 * Find height map for sky light emitters;
*/
void	update_chunk_column_light_0(t_chunk_col *column)
{
	int	light_index;
	int	block_index;
	int	curr_chunk_index;
	t_block_data	data;

	for (int x = 0; x < CHUNK_WIDTH; x++)	
	{
		for (int z = 0; z < CHUNK_BREADTH; z++)	
		{
			light_index = x * CHUNK_WIDTH + z;
			column->lights[light_index].chunk_index = -1;
			vec3i_new(column->lights[light_index].local, x, -1, z);
			for (int i = CHUNKS_PER_COLUMN - 1; i >= 0; i--)
			{
				for (int y = CHUNK_HEIGHT - 1; y >= 0; y--)
				{
					block_index = get_block_index(x, y, z);
					data = get_block_data(&column->chunks[i]->blocks[block_index]);
					column->chunks[i]->blocks[block_index].light_lvl = 0;
					// If we havent found skylight emitter for this x/z coordinate,
					//	and the block we are currently looking at, is not a gas block,
					//	we will add the previous up block to the emitter, because that is most
					//	likely the gas block we want skylight to get emitted from;
					if (column->lights[light_index].chunk_index == -1 &&
						!is_gas(&column->chunks[i]->blocks[block_index]))
					{
						if (y + 1 < CHUNK_HEIGHT)
						{
							column->lights[light_index].chunk_index = i;
							column->lights[light_index].local[1] = y + 1;
						}
						else
						{
							column->lights[light_index].chunk_index = i + 1;
							column->lights[light_index].local[1] = 0;
						}
					}
				}
			}
		}
	}
}

/*
 * Start emitting from the sky light emitters;
*/
void	update_chunk_column_light_1(t_chunk_col *column)
{
	int	skylight;
	float world[3];
	int local[3];
	skylight = column->chunks[0]->info->sky_light_lvl;
	for (int i = 0; i < CHUNK_COLUMN_LIGHT_AMOUNT; i++)
	{
		emit_sky_light(column, column->lights[i].chunk_index, column->lights[i].local, skylight);
	}

	// Torch Light
	int				block_index;
	t_block_data	data;

	for (int i = 0; i < CHUNKS_PER_COLUMN; i++)
	{
		block_index = -1;
		for (int j = 0; j < column->chunks[i]->block_palette[BLOCK_ALPHA_TORCH]; j++)
		{
			while (block_index < CHUNK_BLOCK_AMOUNT)
			{
				++block_index;
				if (column->chunks[i]->blocks[block_index].type == BLOCK_ALPHA_TORCH)
				{
					get_block_local_pos_from_index(local, block_index);
					data = get_block_data(&column->chunks[i]->blocks[block_index]);
					emit_sky_light(column, i, local, data.light_emit);
					break ;
				}
			}
		}
	}
}

/*
 * We only want to do this once per column of chunks;
 *	Give into this only the highest chunk in the column, that has blocks;
*/
void	update_chunk_column_light(t_chunk_col *column)
{
	update_chunk_column_light_0(column);
	update_chunk_column_light_1(column);
}

void	block_print(t_block *block)
{
	t_block_data	data;

	data = get_block_data(block);
	ft_printf("\n[%s] :\n", data.name);
	ft_printf("\tType : %d\n", block->type);
	ft_printf("\tLight Level : %d\n", block->light_lvl);
	ft_printf("\tVisible Faces : ");
	print_binary(block->visible_faces, 8);
	ft_printf(" (last 2 bits are not used)\n");
	ft_printf("Data :\n");
	ft_printf("\tLight Emit : %d\n", data.light_emit);
}