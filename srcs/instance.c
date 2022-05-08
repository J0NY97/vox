#include "shaderpixel.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/
void	new_chunk(t_chunk *chunk, t_chunk_info *info, int nth)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	chunk->needs_to_update = 0;
	chunk->secondary_update = 0;

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

	init_chunk_mesh_v2(&chunk->meshes, MESH_TYPE_AMOUNT);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * Create chunk from noise map;
*/
int	chunk_gen_v2(t_chunk *chunk, int *noise_map)
{
	int i = 0;

	chunk->has_blocks = 0;
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			float	block_world_z = fabs(chunk->world_coordinate[2] + z);
			int		whatchumacallit = noise_map[x * CHUNK_HEIGHT + z] - (int)chunk->world_coordinate[1];

			for (int y = 0; y < CHUNK_HEIGHT; y++)
			{
				float	block_world_y = chunk->world_coordinate[1] + y;
				float	perper = 100.0f;
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
				if (chunk->info->light_calculation)
					chunk->blocks[i].light_lvl = 0;
				else
					chunk->blocks[i].light_lvl = 15;
				if (perper > 0.9f && y <= whatchumacallit)
				{
					if (y <= whatchumacallit - 1) // if we have 3 dirt block on top we make the rest stone blocks;
						chunk->blocks[i].type = BLOCK_STONE;
					else
					{
						if ((block_world_y <= 67 && block_world_y >= 65) ||
							block_world_y + 1 <= 65)
							chunk->blocks[i].type = BLOCK_SAND;
						else if (block_world_y < 2)
							chunk->blocks[i].type = BLOCK_BEDROCK;
						else
							chunk->blocks[i].type = BLOCK_DIRT;
					}
					chunk->has_blocks = 1;
				}
				else
				{
					if (block_world_y <= 65)
					{
						chunk->blocks[i].type = FLUID_WATER;
						chunk->has_blocks = 1;
					}
					else
						chunk->blocks[i].type = GAS_AIR;
				}
				i++;
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

int	create_noise_map(int *map, int size_x, int size_z, int coord_x, int coord_z)
{
	int		start_y = 64;
	float	freq = 0.005f;
	float	pers = 0.5f;
	int		i = 0;
	float	seed = (896868766 % 512) * freq;
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
				octave_perlin(to_use_x, seed, to_use_z, 1, pers) +
				octave_perlin(to_use_x, seed, to_use_z, 2, pers) +
				octave_perlin(to_use_x, seed, to_use_z, 4, pers) +
				octave_perlin(to_use_x, seed, to_use_z, 8, pers);
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
	for (int i = 0; i < CHUNKS_LOADED; i++)
	{
		if (info->chunks[i].coordinate[0] == from_coord[0] &&
			info->chunks[i].coordinate[1] == from_coord[1] &&
			info->chunks[i].coordinate[2] == from_coord[2])
			return (&info->chunks[i]);
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

/*
 * From local chunk block pos to the index in the 'chunk->blocks' array;
*/
int	get_block_index(t_chunk_info *info, int x, int y, int z)
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
float *block_world_pos(float *res, float *chunk_world_pos, int *block_local_pos)
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
		int index = get_block_index(info, local_pos[0], local_pos[1], local_pos[2]);
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
		adj = &chunk->blocks[get_block_index(chunk->info, coords[0], coords[1], coords[2])];
	else if (chunk2) // then check the neighbor; (the neighbor should already be the correct one)
		adj = &chunk2->blocks[get_block_index(chunk->info,
			mod(coords[0], CHUNK_WIDTH), mod(coords[1], CHUNK_HEIGHT), mod(coords[2], CHUNK_BREADTH))];
	return (adj);
}

t_block	*get_block_in_dir(t_chunk *chunk, t_chunk *neighbor, int *local_pos, int dir)
{
	int				coords[3];

	coords[0] = local_pos[0] + g_card_dir[dir][0];
	coords[1] = local_pos[1] + g_card_dir[dir][1];
	coords[2] = local_pos[2] + g_card_dir[dir][2];
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
	float			*block_world;
	int				light;

	if (adjacent)
	{
		data = get_block_data(block);
		light = (100 / 15) * ft_clamp(block->light_lvl, 0, 15);//(int)g_face_light[face];
		if (is_fluid(block) && !is_solid(adjacent) &&
			!(is_fluid(block) && is_fluid(adjacent)))
		{
			// If both blocks are fluid, we dont add face to mesh;
			float	verts[12];
			int		type = -1;

			block_world_pos(block_world, chunk->world_coordinate, local_pos);
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
			else
				add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_ALPHA_MESH, local_pos, (float *)g_faces_cactus[dir], data.texture[dir], light);
			++chunk->blocks_solid_alpha_amount;
		}
	}
}

void	helper_pelper(t_chunk *chunk, t_chunk **neighbors, int *pos)
{
	int index;
	
	index = get_block_index(chunk->info, pos[0], pos[1], pos[2]);
	if (chunk->blocks[index].type == GAS_AIR) // <-- very important, im not sure what happens if we are trying to render an air block;
		return ;

	if (is_flora(&chunk->blocks[index]))
	{
		chunk->blocks[index].visible_faces |= g_visible_faces[6];
		add_to_chunk_mesh_v2(&chunk->meshes, FLORA_MESH, pos, (float *)g_flora_faces[0], g_flora_data[chunk->blocks[index].type - FLORA_FIRST - 1].texture[0], 100);
		add_to_chunk_mesh_v2(&chunk->meshes, FLORA_MESH, pos, (float *)g_flora_faces[1], g_flora_data[chunk->blocks[index].type - FLORA_FIRST - 1].texture[1], 100);
		++chunk->blocks_flora_amount;
	}
	else // these are the solid blocks;
	{
		t_block	*adj = NULL;
		for (int dir = 0; dir <= DIR_DOWN; dir++)
		{
			adj = get_block_in_dir(chunk, neighbors[dir], pos, dir);
			if (adj && !is_solid(adj)) // add to mesh if adjacent block isnt solid;
			{
				// Dont add to mesh if face already in it;
				if (!(chunk->blocks[index].visible_faces & g_visible_faces[dir]))
				{
					chunk->blocks[index].visible_faces |= g_visible_faces[dir];
					add_block_to_correct_mesh(chunk, &chunk->blocks[index], adj, pos, dir);
				}
			}
			else
				chunk->blocks[index].visible_faces &= ~g_visible_faces[dir];
		}
	}
	/*
	chunk->blocks[index].visible_faces = g_visible_faces[6];
	print_binary(chunk->blocks[index].visible_faces, 8);
	ft_putstr("\n");
	chunk->blocks[index].visible_faces |= g_visible_faces[0];
	print_binary(chunk->blocks[index].visible_faces, 8);
	ft_putstr("\n");
	chunk->blocks[index].visible_faces &= ~g_visible_faces[0];
	print_binary(chunk->blocks[index].visible_faces, 8);
	ft_putstr("\n");
	exit(0);
	*/
}

/*
 * For each block in chunk, check all its adjacent blocks,
 *	if touching air, add that face to the chunk's mesh.
 *
 * This func resets the mesh before doing anything;
*/
void	get_blocks_visible(t_chunk *chunk)
{
	chunk->blocks_solid_amount = 0;
	chunk->blocks_flora_amount = 0;
	chunk->blocks_fluid_amount = 0;
	chunk->blocks_solid_alpha_amount = 0;

	if (!chunk->has_blocks)
		return ;

	// Get all neighbors of current chunk;
	t_chunk *neighbors[6];
	for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; ++dir, ++i)
		neighbors[i] = get_adjacent_chunk(chunk->info, chunk, (float *)g_card_dir[dir]);

	// TODO: remove this, try to integrate it to the loop under this;
	// Reset visible faces;
	for (int i = 0; i < chunk->block_amount; i++)
		chunk->blocks[i].visible_faces = 0;

	for (int y = 1; y < CHUNK_HEIGHT - 1; y++)
	{
		for (int x = 1; x < CHUNK_WIDTH - 1; x++)
		{
			for (int z = 1; z < CHUNK_BREADTH - 1; z++)
			{
				helper_pelper(chunk, neighbors, (int []){x, y, z});
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
		neighbors[i] = get_adjacent_chunk(chunk->info, chunk, (float *)g_card_dir[dir]);

	for (int y = 0; y <= CHUNK_HEIGHT - 1; y++)
	{
		for (int x = 0; x <= CHUNK_WIDTH - 1; x++)
		{
			helper_pelper(chunk, neighbors, (int []){x, y, 0});
			helper_pelper(chunk, neighbors, (int []){x, y, CHUNK_BREADTH - 1});
		}
		for (int z = 0; z <= CHUNK_BREADTH - 1; z++)
		{
			helper_pelper(chunk, neighbors, (int []){0, y, z});
			helper_pelper(chunk, neighbors, (int []){CHUNK_WIDTH - 1, y, z});
		}
	}
	for (int x = 0; x <= CHUNK_WIDTH - 1; x++)
	{
		for (int z = 0; z <= CHUNK_BREADTH - 1; z++)
		{
			helper_pelper(chunk, neighbors, (int []){x, 0, z});
			helper_pelper(chunk, neighbors, (int []){x, CHUNK_HEIGHT - 1, z});
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

void	update_chunk_v2(t_chunk *chunk, int *coord, int *noise_map)
{
	for (int i = 0; i < 3; i++)
		chunk->coordinate[i] = coord[i];
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * CHUNK_SIZE_X,
		chunk->coordinate[1] * CHUNK_SIZE_Y,
		chunk->coordinate[2] * CHUNK_SIZE_Z);

	// Generate Chunks	
	chunk->block_amount = chunk_gen_v2(chunk, noise_map); // should always return max amount of blocks in a chunk;

	chunk->update_structures = 1;
	chunk->needs_to_update = 1;
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
 * Check which chunks are not going to be in the next iteration of
 *	loaded chunks, save those to 'reload_these_chunks' and when starting
 *	to update the new chunks that are going to be loaded, and put the
 *	new chunk info into those 'chunks' indices;
 * Takes 0.000000 seconds;
*/
int	get_chunks_to_reload(int *chunks, int *start_coord, t_chunk_info *info, int *player_chunk_v3)
{
	int	reload_amount = 0;
	int	found = 0;

	start_coord[0] = player_chunk_v3[0] - (RENDER_DISTANCE / 2);
	start_coord[1] = 0;
	start_coord[2] = player_chunk_v3[2] - (RENDER_DISTANCE / 2);
	for (int i = 0; i < CHUNKS_LOADED; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < RENDER_DISTANCE; x++, x_amount++)
		{
			for (int z = start_coord[2], z_amount = 0; z_amount < RENDER_DISTANCE; z++, z_amount++)
			{
				if (info->chunks[i].coordinate[0] == x && info->chunks[i].coordinate[2] == z)
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
			chunks[reload_amount] = i;
			reload_amount++;
		}
	}
	return (reload_amount);
}

/*
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

void	*chunk_thread_func(void *args)
{
	t_chunk_args	*info;

	info = args;
	update_chunk_v2(info->chunk, info->coords, info->noise_map);
	update_chunk_visible_blocks(info->chunk);
	chunk_aabb_update(info->chunk);
	return (NULL);
}

int	regenerate_chunks_threading(int *these, int coord[2], t_chunk_info *info)
{
	int				ind;
	int				nth_chunk = 0;
	int				noise_map[CHUNK_WIDTH * CHUNK_BREADTH];
	t_chunk_args	args[CHUNKS_PER_COLUMN];
	pthread_t		threads[CHUNKS_PER_COLUMN];

	create_noise_map(noise_map, CHUNK_WIDTH, CHUNK_BREADTH, coord[0], coord[1]);
	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		if (nth_chunk >= CHUNKS_PER_COLUMN || nth_chunk >= CHUNKS_LOADED) 
			break ;
		ind = these[nth_chunk];
		args[nth_chunk].chunk = &info->chunks[ind];
		args[nth_chunk].coords[0] = coord[0];
		args[nth_chunk].coords[1] = y;
		args[nth_chunk].coords[2] = coord[1];
		args[nth_chunk].noise_map = noise_map;
		pthread_create(&threads[nth_chunk], NULL, chunk_thread_func, &args[nth_chunk]);
		nth_chunk++;
	}
	for (int i = 0; i < nth_chunk; i++)
		pthread_join(threads[i], NULL);
	for (int i = 0; i < nth_chunk; i++)
		update_chunk_mesh(&info->chunks[these[i]].meshes);
	return (nth_chunk);
}

/*
 * Returns amount of chunks that still need to get generated;
*/
int	regenerate_chunks(int *these, int coord[2], t_chunk_info *info)
{
	int				ind;
	int				nth_chunk = 0;
	int				noise_map[CHUNK_WIDTH * CHUNK_BREADTH];

	create_noise_map(noise_map, CHUNK_WIDTH, CHUNK_BREADTH, coord[0], coord[1]);
	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		if (nth_chunk >= CHUNKS_PER_COLUMN || nth_chunk >= CHUNKS_LOADED) 
			break ;
		ind = these[nth_chunk];
		update_chunk_v2(&info->chunks[ind], (int []){coord[0], y, coord[1]}, noise_map);
		update_chunk_visible_blocks(&info->chunks[ind]);
		update_chunk_mesh(&info->chunks[ind].meshes);
		chunk_aabb_update(&info->chunks[ind]);
		info->chunks[ind].needs_to_update = 0;
		nth_chunk++;
	}
	return (nth_chunk);
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
void	init_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int amount)
{
	GLuint	vbo[2];
	int		error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	mesh->vertices_allocated = 17964;
	mesh->vertices = malloc(sizeof(float) * mesh->vertices_allocated);
	mesh->vertices_amount = 0;

	mesh->texture_ids_allocated = 5988; 
	mesh->texture_ids = malloc(sizeof(int) * mesh->texture_ids_allocated);
	mesh->texture_id_amount = 0;

	mesh->amount = amount;
	mesh->indices = malloc(sizeof(unsigned int *) * mesh->amount);
	mesh->indices_allocated = malloc(sizeof(int) * mesh->amount);
	mesh->indices_amount = malloc(sizeof(int) * mesh->amount);
	mesh->index_amount = 0;
	for (int i = 0; i < mesh->amount; i++)
	{
		mesh->indices_allocated[i] = 8466;
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
void	render_chunk_mesh_v2(t_chunk_mesh_v2 *mesh, int mesh_type, float *coordinate, t_camera *camera, t_shader *shader)
{
	int	error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);
	
	glUseProgram(shader->program);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, &camera->projection[0]);

	glUniform3fv(glGetUniformLocation(shader->program, "chunkPos"), 1, &coordinate[0]);

	glBindVertexArray(mesh->vao);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	if (mesh->indices_amount[mesh_type] > 0)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo[mesh_type]);
		glDrawElements(GL_TRIANGLES, mesh->indices_amount[mesh_type],
			GL_UNSIGNED_INT, NULL);
	}
	else
		LG_WARN("We can\'t render mesh with no indices.");

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * Sends the mesh info to the GPU;
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
	// @Modulate (aka make modular)
	float	block_scale = 0.5f;

	// Vertices and Texture
	if (mesh->vertices_allocated < mesh->vertices_amount + 12)
	{
		mesh->vertices_allocated += 256;
		mesh->vertices = realloc(mesh->vertices, sizeof(float) * mesh->vertices_allocated);
		LG_WARN("Reallocating Vertices from %d to %d", mesh->vertices_allocated - 256, mesh->vertices_allocated);
	}

	if (mesh->texture_ids_allocated < mesh->texture_id_amount + 4)
	{
		mesh->texture_ids_allocated += 256;
		mesh->texture_ids = realloc(mesh->texture_ids, sizeof(int) * mesh->texture_ids_allocated);
		LG_WARN("Reallocating Texture Ids from %d to %d", mesh->texture_ids_allocated - 256, mesh->texture_ids_allocated);
	}

	int ind = 0;
	int	tex = 0;
	for (int i = 0; i < 4; i++)
	{
		ind = 3 * i;
		mesh->vertices[mesh->vertices_amount + ind + 0] = (face_vertices[ind + 0] * block_scale) + coord[0];
		mesh->vertices[mesh->vertices_amount + ind + 1] = (face_vertices[ind + 1] * block_scale) + coord[1];
		mesh->vertices[mesh->vertices_amount + ind + 2] = (face_vertices[ind + 2] * block_scale) + coord[2];

		tex = texture_id | (i << 16) | (light << 20);
		mesh->texture_ids[mesh->texture_id_amount + i] = tex;
	}

	mesh->vertices_amount += 4 * 3;
	mesh->texture_id_amount += 4;

	// Indices
	if (mesh->indices_allocated[mesh_type] < mesh->indices_amount[mesh_type] + 6)
	{
		mesh->indices_allocated[mesh_type] += 256;
		mesh->indices[mesh_type] = realloc(mesh->indices[mesh_type], sizeof(unsigned int) * mesh->indices_allocated[mesh_type]);
		LG_WARN("Reallocating Indices from %d to %d", mesh->indices_allocated[mesh_type] - 256, mesh->indices_allocated[mesh_type]);
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

/*
 * Returns true if hovering over;
 * Saves which face was hovered, in 'face';
*/
int	is_hovering_solid_block(float *block_pos, float *point, int *face)
{
	float	p1[3];
	float	p2[3];
	float	p3[3];

	for (int f = DIR_NORTH; f <= DIR_DOWN; ++f)
	{
		p1[0] = (g_faces[f][0] * 0.5f) + block_pos[0];
		p1[1] = (g_faces[f][1] * 0.5f) + block_pos[1];
		p1[2] = (g_faces[f][2] * 0.5f) + block_pos[2];

		p2[0] = (g_faces[f][3] * 0.5f) + block_pos[0];
		p2[1] = (g_faces[f][4] * 0.5f) + block_pos[1];
		p2[2] = (g_faces[f][5] * 0.5f) + block_pos[2];

		p3[0] = (g_faces[f][6] * 0.5f) + block_pos[0];
		p3[1] = (g_faces[f][7] * 0.5f) + block_pos[1];
		p3[2] = (g_faces[f][8] * 0.5f) + block_pos[2];
		if (point_in_triangle(point, p1, p2, p3))
		{
			*face = f;
			return (1);
		}

		p1[0] = (g_faces[f][0] * 0.5f) + block_pos[0];
		p1[1] = (g_faces[f][1] * 0.5f) + block_pos[1];
		p1[2] = (g_faces[f][2] * 0.5f) + block_pos[2];

		p2[0] = (g_faces[f][6] * 0.5f) + block_pos[0];
		p2[1] = (g_faces[f][7] * 0.5f) + block_pos[1];
		p2[2] = (g_faces[f][8] * 0.5f) + block_pos[2];

		p3[0] = (g_faces[f][9] * 0.5f) + block_pos[0];
		p3[1] = (g_faces[f][10] * 0.5f) + block_pos[1];
		p3[2] = (g_faces[f][11] * 0.5f) + block_pos[2];
		if (point_in_triangle(point, p1, p2, p3))
		{
			*face = f;
			return (1);
		}
	}
	return (0);
}

/* BUG : Whatever is being saved in 'face' is incorrect */
int	is_hovering_flora_block(float *block_pos, float *point, int *face)
{
	float	p1[3];
	float	p2[3];
	float	p3[3];

	for (int f = 0; f < FACE_FLORA_AMOUNT; f++)
	{
		p1[0] = (g_flora_faces[f][0] * 0.5f) + block_pos[0];
		p1[1] = (g_flora_faces[f][1] * 0.5f) + block_pos[1];
		p1[2] = (g_flora_faces[f][2] * 0.5f) + block_pos[2];

		p2[0] = (g_flora_faces[f][3] * 0.5f) + block_pos[0];
		p2[1] = (g_flora_faces[f][4] * 0.5f) + block_pos[1];
		p2[2] = (g_flora_faces[f][5] * 0.5f) + block_pos[2];

		p3[0] = (g_flora_faces[f][6] * 0.5f) + block_pos[0];
		p3[1] = (g_flora_faces[f][7] * 0.5f) + block_pos[1];
		p3[2] = (g_flora_faces[f][8] * 0.5f) + block_pos[2];
		if (point_in_triangle(point, p1, p2, p3))
		{
			*face = f;
			return (1);
		}

		p1[0] = (g_flora_faces[f][0] * 0.5f) + block_pos[0];
		p1[1] = (g_flora_faces[f][1] * 0.5f) + block_pos[1];
		p1[2] = (g_flora_faces[f][2] * 0.5f) + block_pos[2];

		p2[0] = (g_flora_faces[f][6] * 0.5f) + block_pos[0];
		p2[1] = (g_flora_faces[f][7] * 0.5f) + block_pos[1];
		p2[2] = (g_flora_faces[f][8] * 0.5f) + block_pos[2];

		p3[0] = (g_flora_faces[f][9] * 0.5f) + block_pos[0];
		p3[1] = (g_flora_faces[f][10] * 0.5f) + block_pos[1];
		p3[2] = (g_flora_faces[f][11] * 0.5f) + block_pos[2];
		if (point_in_triangle(point, p1, p2, p3))
		{
			*face = f;
			return (1);
		}
	}
	return (0);
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
	float	faces[6][12];
	int		face_amount;

	i = 0;
	for (; i < chunk->block_amount; i++)
	{
		// skip if gas or fluid;
		if (is_gas(&chunk->blocks[i]) || is_fluid(&chunk->blocks[i]))
			continue;

		get_block_local_pos_from_index(blocal, i);
		block_world_pos(block_world, chunk->world_coordinate, blocal);

		face_amount = 0;
		if (is_solid(&chunk->blocks[i]) ||
			(is_solid(&chunk->blocks[i]) && chunk->blocks[i].type == BLOCK_ALPHA_OAK_LEAF))
		{
			face_amount = 6;
			for (int j = 0; j < face_amount; j++)
				for (int p = 0; p < 12; p++)
					faces[j][p] = g_faces[j][p];
		}
		else if (is_flora(&chunk->blocks[i]))
		{
			face_amount = 2;
			for (int j = 0; j < face_amount; j++)
				for (int p = 0; p < 12; p++)
					faces[j][p] = g_flora_faces[j][p];
		}
		else if (is_solid_alpha(&chunk->blocks[i]))
		{
			face_amount = 6;
			for (int j = 0; j < face_amount; j++)
				for (int p = 0; p < 12; p++)
					faces[j][p] = g_faces_cactus[j][p];
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

	vec3_normalize(norm_dir, dir);
	vertices = mesh->vertices;
	indices = mesh->indices[mesh_type];
	for (int i = 0; i < mesh->indices_amount[mesh_type] / 3; i++)
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
		vec3_add(p1, p1, world_coords);
		vec3_add(p2, p2, world_coords);
		vec3_add(p3, p3, world_coords);
		if (ray_triangle_intersect(orig, dir, p1, p2, p3, 0, intersect_point[collisions]))
			if (vec3_dist(orig, intersect_point[collisions]) <= reach + 0.001f)
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
		if (ray_triangle_intersect(orig, dir, p1, p2, p3, 1, intersect_points[collisions]))
		{
			if (vec3_dist(orig, intersect_points[collisions]) <= reach + EPSILON)
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
 * Places block of type 'block_type' in the world position of 'world_pos' no
 *	matter what.
*/
void	set_block_at_world_pos(t_chunk_info *info, float *world_pos, int block_type)
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
	index = get_block_index(info, block_local[0], block_local[1], block_local[2]);
	chunk->blocks[index].type = block_type;
	LG_DEBUG("Block placed in chunk <%d %d %d> at (world : %f %f %f) (local : %d %d %d) (index : %d)",
		chunk->coordinate[0], chunk->coordinate[1], chunk->coordinate[2],
		world_pos[0], world_pos[1], world_pos[2],
		block_local[0], block_local[1], block_local[2],
		index);
	chunk->needs_to_update = 1;
	if (!is_type_gas(block_type))
		chunk->has_blocks = 1;
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
	index = get_block_index(info, block_local[0], block_local[1], block_local[2]);
	if (!is_gas(&chunk->blocks[index]))
		return ;
	chunk->blocks[index].type = block_type;
	LG_DEBUG("Block placed in chunk <%d %d %d> at (world : %f %f %f) (local : %d %d %d) (index : %d)",
		chunk->coordinate[0], chunk->coordinate[1], chunk->coordinate[2],
		world_pos[0], world_pos[1], world_pos[2],
		block_local[0], block_local[1], block_local[2],
		index);
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
	index = get_block_index(info, block_local[0], block_local[1], block_local[2]);
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
	if (type == BLOCK_DIRT)
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
	if (block_type != BLOCK_DIRT)
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
	int	highest = -1;
	int	highest_index = -1;

	for (int i = 0; i < CHUNKS_LOADED; i++)
	{
		if (info->chunks[i].coordinate[0] == x &&
			info->chunks[i].coordinate[2] == z &&
			info->chunks[i].has_blocks &&
			info->chunks[i].coordinate[1] > highest)
		{
			highest = info->chunks[i].coordinate[1];
			highest_index = i;
		}
	}
	if (highest_index >= 0 && highest_index < CHUNKS_LOADED)
		return (&info->chunks[highest_index]);
	return (NULL);
}

/*
 * Returns highest point in world at x, z (world_position);
*/
float	get_highest_point(t_chunk_info *info, float x, float z)
{
	int		chunk_pos[3];
	float	curr_highest = -1;
	t_chunk	*highest_chunk;

	get_chunk_pos_from_world_pos(chunk_pos, (float []){x, 0, z});
	highest_chunk = get_highest_chunk(info, chunk_pos[0], chunk_pos[2]);
	if (highest_chunk == NULL)
		return (-1); // error;
	for (int i = 0; i < 4096; i++)
	{
		if (highest_chunk->blocks[i].type != GAS_AIR)
		{
			int		local[3];
			float	world[3];

			get_block_local_pos_from_index(local, i);
			block_world_pos(world, highest_chunk->world_coordinate, local);
			if (world[0] == x && world[2] == z)
				if (world[1] > curr_highest)
					curr_highest = world[1];
		}
	}
	return (curr_highest);
}

/*
 * Get highets point in the world at x/z coordinate of block type 'type';
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
	for (int i = 0; i < 4096; i++)
	{
		if (highest_chunk->blocks[i].type == type)
		{
			int		local[3];
			float	world[3];

			get_block_local_pos_from_index(local, i);
			block_world_pos(world, highest_chunk->world_coordinate, local);
			if (world[0] == x && world[2] == z)
				if (world[1] > curr_highest)
					curr_highest = world[1];
		}
	}
	return (curr_highest);
}

void	flora_decider(t_chunk_info *info, float chance, float *world_pos)
{
	if (chance < 1.4f)
	{
		if (chance < 1.3f)
			flora_placer(info, FLORA_FLOWER_YELLOW, world_pos);
		else
			flora_placer(info, FLORA_FLOWER_RED, world_pos);
	}
	else
		flora_placer(info, FLORA_GRASS, world_pos);
}

void	tree_gen(t_chunk *chunk)
{
	float	freq = 0.99f;
	float	pers = 0.5;

	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			float	block_world_z = fabs(chunk->world_coordinate[2] + z);
			float	to_use_z = block_world_z * freq;
			float	perper =
				octave_perlin(to_use_x, to_use_x / to_use_z, to_use_z, 1, pers) +
				octave_perlin(to_use_x, to_use_x / to_use_z, to_use_z, 2, pers) +
				octave_perlin(to_use_x, to_use_x / to_use_z, to_use_z, 4, pers) +
				octave_perlin(to_use_x, to_use_x / to_use_z, to_use_z, 8, pers);
			if (perper < 1.5f)
			{
				float world_x_pos = chunk->world_coordinate[0] + (float)x;
				float world_z_pos = chunk->world_coordinate[2] + (float)z;
				float highest = get_highest_point_of_type(chunk->info, world_x_pos, world_z_pos, BLOCK_DIRT);
				if (highest == -1)
				{
					highest = get_highest_point_of_type(chunk->info, world_x_pos, world_z_pos, BLOCK_SAND);
					if (highest == -1)
						continue ;
				}
				if (perper < 1.25f)
					tree_placer(chunk->info,
						(float []){world_x_pos, highest + 1, world_z_pos});
				else if (perper < 1.5f)
					flora_decider(chunk->info, perper,
						(float []){world_x_pos, highest + 1, world_z_pos});
				
			}
		}
	}
}

void	update_chunk_light_0(t_chunk *chunk)
{
	t_block			*down_block;
	t_block			*block;
	t_block_data	data;
	int				found;

	// Get highest block in the column, make it emit light;
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_BREADTH; z++)
		{
			found = 0;
			for (int y = CHUNK_HEIGHT - 1; y >= 0; y--)
			{
				block = &chunk->blocks[get_block_index(chunk->info, x, y, z)];
				block->is_emit = 0;
				block->light_lvl = 0;
				if (is_gas(block) && !found)
					block->is_emit = 1;
				if (y - 1 >= 0)
				{
					down_block = &chunk->blocks[get_block_index(chunk->info, x, y - 1, z)];
					if (down_block && !is_gas(down_block))
						found = 1;
				}
			}
		}
	}
}

/*
 *
*/
void	emit_sky_light(t_chunk *chunk, int *coord, int light)
{
	t_block			*block;
	t_block_data	data;

	if (light <= 0 ||
		coord[0] < 0 || coord[0] > CHUNK_WIDTH - 1 ||
		coord[1] < 0 || coord[1] > CHUNK_HEIGHT - 1 ||
		coord[2] < 0 || coord[2] > CHUNK_BREADTH - 1)
		return ;
	block = &chunk->blocks[get_block_index(chunk->info, coord[0], coord[1], coord[2])];
	data = get_block_data(block);
	if (light > block->light_lvl)
		block->light_lvl = light;
	else // if block doesnt want more light, we can assume other blocks in that direction doesnt either;
		return ;
	if (block->light_lvl == 0)
		return ;
/*
*/
	emit_sky_light(chunk, (int []){coord[0], coord[1] + 1, coord[2]}, block->light_lvl + data.light_emit - 1);
	emit_sky_light(chunk, (int []){coord[0], coord[1] - 1, coord[2]}, block->light_lvl + data.light_emit);
	emit_sky_light(chunk, (int []){coord[0] + 1, coord[1], coord[2]}, block->light_lvl + data.light_emit - 1);
	emit_sky_light(chunk, (int []){coord[0] - 1, coord[1], coord[2]}, block->light_lvl + data.light_emit - 1);
	emit_sky_light(chunk, (int []){coord[0], coord[1], coord[2] + 1}, block->light_lvl + data.light_emit - 1);
	emit_sky_light(chunk, (int []){coord[0], coord[1], coord[2] - 1}, block->light_lvl + data.light_emit - 1);
/*
*/
}

void	update_chunk_light_1(t_chunk *chunk)
{
	t_block	*block;

	// for all emitters in the column of blocks, flood fill light around;
	for (int x = 0; x < CHUNK_WIDTH; x++)
	{
		for (int z = 0; z < CHUNK_WIDTH; z++)
		{
			for (int y = CHUNK_HEIGHT - 1; y >= 0; y--)
			{
				block = &chunk->blocks[get_block_index(chunk->info, x, y, z)];
				if (block->is_emit)
				{
					emit_sky_light(chunk, (int []){x, y, z}, 15);
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
void	update_chunk_light(t_chunk *chunk)
{
	update_chunk_light_0(chunk);
	update_chunk_light_1(chunk);
}

void	block_print(t_block *block)
{
	t_block_data	data;

	data = get_block_data(block);
	ft_printf("\n[%s] :\n", data.name);
	ft_printf("\tType : %d\n", block->type);
	ft_printf("\tIs Emit : %d\n", block->is_emit);
	ft_printf("\tLight Level : %d\n", block->light_lvl);
	ft_printf("\tVisible Faces : ");
	print_binary(block->visible_faces, 8);
	ft_printf(" (last 2 bits are not used)\n");
	ft_printf("Data :\n");
	ft_printf("\tLight Emit : %d\n", data.light_emit);
}