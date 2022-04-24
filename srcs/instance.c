#include "shaderpixel.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/
void	new_chunk(t_chunk *chunk, t_chunk_info *info, int nth)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	chunk->needs_to_update = 0;

	int	max_blocks = chunk->info->width * chunk->info->breadth * chunk->info->height;

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
	for (int x = 0; x < chunk->info->width; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		for (int z = 0; z < chunk->info->breadth; z++)
		{
			float	block_world_z = fabs(chunk->world_coordinate[2] + z);
			int		whatchumacallit = noise_map[x * 16 + z] - (int)chunk->world_coordinate[1];

			for (int y = 0; y < chunk->info->height; y++)
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

/*
 * @MODULAR : MAKE
*/
void	get_chunk_world_pos_from_local_pos(float *res, int *local_pos)
{
	vec3_multiply_f(res, (float []){local_pos[0], local_pos[1], local_pos[2]}, 16.0f);
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
int	*get_chunk_pos_from_world_pos(int *res, float *world_coords, t_chunk_info *info)
{
	res[0] = floor(world_coords[0] / info->chunk_size[0]);
	res[1] = floor(world_coords[1] / info->chunk_size[1]);
	res[2] = floor(world_coords[2] / info->chunk_size[2]);
	return (res);
}

void	chunk_aabb_update(t_chunk *chunk)
{
	t_aabb	*a;

	a = &chunk->aabb;
	a->min[0] = chunk->world_coordinate[0] - (chunk->info->block_size / 2);
	a->min[1] = chunk->world_coordinate[1] - (chunk->info->block_size / 2);
	a->min[2] = chunk->world_coordinate[2] - (chunk->info->block_size / 2);
	a->max[0] = a->min[0] + chunk->info->chunk_size[0];
	a->max[1] = a->min[1] + chunk->info->chunk_size[1];
	a->max[2] = a->min[2] + chunk->info->chunk_size[2];
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
	static int times_called = -1;
	int	from_coord[3];
//	ft_printf("times_called %d\n", ++times_called);

	from_coord[0] = from->coordinate[0] + (int)dir[0];
	from_coord[1] = from->coordinate[1] + (int)dir[1];
	from_coord[2] = from->coordinate[2] + (int)dir[2];

	/*
	unsigned long int	key = get_chunk_hash_key(from_coord);
	t_hash_item	*item = hash_item_search(info->hash_table, info->hash_table_size, key);
	if (!item)
		return (NULL);
	return (&info->chunks[item->data]);
	*/
	for (int i = 0; i < info->chunks_loaded; i++)
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
	for (int i = 0; i < info->chunks_loaded; i++)
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
	return ((x * info->width * info->breadth) + (z * info->height) + y);
}

int	*get_block_local_pos_from_world_pos(int *res, float *world)
{
	res[0] = mod(world[0], 16);
	res[1] = mod(world[1], 16);
	res[2] = mod(world[2], 16);
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
	// TODO use chunk->w, h, b;
	res[0] = index / (16 * 16);
	res[2] = (index / 16) % 16;
	res[1] = index % 16;
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

	get_chunk_pos_from_world_pos(chunk_pos, coords, info);
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
 * 'chunk' : the chunk of the block we are currently checking;
 * 'i' : index of the block in 'chunk->blocks';
 * 'local_pos' : local coordinates of the block in the chunk (x/y/z == 0 - 15);
 * 'face' : which cardinal direction the block we want to check is in;
 * 'neighbor' : the neighbor in the same cardinal direction (we give this whether or not needed);
*/
void	adjacent_block_checker(t_chunk *chunk, int i, int *local_pos, int face, t_chunk *neighbor)
{
	t_block	*tmp_block;
	t_block	*blocks = chunk->blocks;
	int		coords[3];
	float	block_world[3];

	coords[0] = local_pos[0] + g_card_dir[face][0];
	coords[1] = local_pos[1] + g_card_dir[face][1];
	coords[2] = local_pos[2] + g_card_dir[face][2];

	tmp_block = NULL;
	// Try to get from the same chunk first;
	if (coords[0] >= 0 && coords[0] <= 15 &&
		coords[1] >= 0 && coords[1] <= 15 &&
		coords[2] >= 0 && coords[2] <= 15)
		tmp_block = &blocks[get_block_index(chunk->info, coords[0], coords[1], coords[2])];
	else if (neighbor) // then check the neighbor; (the neighbor should already be the correct one)
		tmp_block = &neighbor->blocks[get_block_index(chunk->info,
			mod(coords[0], 16), mod(coords[1], 16), mod(coords[2], 16))];
	else
		return ;
	if (tmp_block)
	{
		if (is_fluid(&blocks[i]) &&
			!(is_fluid(&blocks[i]) && is_fluid(tmp_block)))
		{
			// If both blocks are fluid, we dont add face to mesh;
			float	verts[12];
			int		type = -1;

			block_world_pos(block_world, chunk->world_coordinate, local_pos);
			flowing_water_verts(verts, face, &blocks[i], block_world, chunk->info);

			add_to_chunk_mesh_v2(&chunk->meshes, FLUID_MESH, local_pos, verts, g_fluid_data[blocks[i].type - FLUID_FIRST - 1].texture, (int)g_face_light[face]);
			++chunk->blocks_fluid_amount;
		}
		else if (is_solid(&blocks[i]) && !is_solid(tmp_block))
		{
			add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_MESH, local_pos, (float *)g_faces[face], g_block_data[blocks[i].type - BLOCK_FIRST - 1].face_texture[face], (int)g_face_light[face]);
			++chunk->blocks_solid_amount;
		}
		else if (is_solid_alpha(&blocks[i]))
		{
			add_to_chunk_mesh_v2(&chunk->meshes, BLOCK_ALPHA_MESH, local_pos, (float *)g_faces_cactus[face], g_block_alpha_data[blocks[i].type - BLOCK_ALPHA_FIRST - 1].face_texture[face], (int)g_face_light[face]);
			++chunk->blocks_solid_alpha_amount;
		}
	}
}

/*
 * For each block in chunk;
 *	Go through each block in chunk;
 *		check if not touching solid block;
 *	if touching;
 *		add to render array;
 *
 * THIS MIGHT ONLY WORK NOW THAT THE CHUNKS ARE SYMMETRIC, IF SOMETHING BREAKS
 * CHECK THAT THE XYZ CORRESPOND CORRECTLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
void	get_blocks_visible(t_chunk *chunk)
{
	t_block	*blocks;

	chunk->blocks_solid_amount = 0;
	chunk->blocks_flora_amount = 0;
	chunk->blocks_fluid_amount = 0;
	chunk->blocks_solid_alpha_amount = 0;

	if (!chunk->has_blocks)
		return ;
	blocks = chunk->blocks;

	// Get all neighbors of current chunk;
	t_chunk *neighbors[6];
	for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; ++dir, ++i)
		neighbors[i] = get_adjacent_chunk(chunk->info, chunk, (float *)g_card_dir[dir]);

	int		pos[3];
	for (int i = 0; i < chunk->block_amount; i++)
	{
		if (blocks[i].type == GAS_AIR) // <-- very important, im not sure what happens if we are trying to render an air block;
			continue ;

		get_block_local_pos_from_index(pos, i);
		if (is_flora(&blocks[i]))
		{
			add_to_chunk_mesh_v2(&chunk->meshes, FLORA_MESH, pos, (float *)g_flora_faces[0], g_flora_data[blocks[i].type - FLORA_FIRST - 1].face_texture[0], 100);
			add_to_chunk_mesh_v2(&chunk->meshes, FLORA_MESH, pos, (float *)g_flora_faces[1], g_flora_data[blocks[i].type - FLORA_FIRST - 1].face_texture[1], 100);
			++chunk->blocks_flora_amount;
		}
		else // these are the solid blocks;
		{
			adjacent_block_checker(chunk, i, pos, DIR_NORTH, neighbors[DIR_NORTH]);
			adjacent_block_checker(chunk, i, pos, DIR_EAST, neighbors[DIR_EAST]);
			adjacent_block_checker(chunk, i, pos, DIR_SOUTH, neighbors[DIR_SOUTH]);
			adjacent_block_checker(chunk, i, pos, DIR_WEST, neighbors[DIR_WEST]);
			adjacent_block_checker(chunk, i, pos, DIR_UP, neighbors[DIR_UP]);
			adjacent_block_checker(chunk, i, pos, DIR_DOWN, neighbors[DIR_DOWN]);
		}
	}
}

unsigned long int	get_chunk_hash_key(int *coords)
{
	/*
	int	res;

	res = coords[0] +
		(8 * coords[1]) +
		(8 * 8 * coords[2]);
	return (res);
	*/

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
	// Get old data;
	/*
	unsigned long int	old_key = get_chunk_hash_key(chunk->coordinate);
	t_hash_item *old_item = hash_item_search(chunk->info->hash_table, chunk->info->hash_table_size, old_key);
	int	old_data = old_item->data;
	hash_item_delete(chunk->info->hash_table, chunk->info->hash_table_size, old_key);
	*/

//	ft_printf("old coord %d %d %d\n", chunk->coordinate[0], chunk->coordinate[1], chunk->coordinate[2]);

	for (int i = 0; i < 3; i++)
		chunk->coordinate[i] = coord[i];
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size[0],
		chunk->coordinate[1] * chunk->info->chunk_size[1],
		chunk->coordinate[2] * chunk->info->chunk_size[2]);

	// insert new data;
	/*
	unsigned long int	new_key = get_chunk_hash_key(chunk->coordinate);	
	if (!hash_item_insert(chunk->info->hash_table, chunk->info->hash_table_size, new_key, old_data))
		LG_WARN("Chunk data couldnt be inserted.");
		*/
	
	// Generate Chunks	
	chunk->block_amount = chunk_gen_v2(chunk, noise_map); // should always return max amount of blocks in a chunk;

	chunk->update_structures = 1;
	chunk->needs_to_update = 1;
}

void	*update_chunk_threaded_v2(void *arg)
{
	t_chunk_args	*info = arg;

	update_chunk_v2(info->chunk, info->coords, info->noise_map);
	info->chunk->args.being_threaded = 0;
	return (NULL);
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

	start_coord[0] = player_chunk_v3[0] - (info->render_distance / 2);
	start_coord[1] = 0;
	start_coord[2] = player_chunk_v3[2] - (info->render_distance / 2);
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
		{
			for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
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

	start_coord[0] = player_chunk_v3[0] - (info->render_distance / 2);
	start_coord[1] = 0;
	start_coord[2] = player_chunk_v3[2] - (info->render_distance / 2);

	// Find chunk that shouldnt be loaded anymore;
	int	coord_x = start_coord[0];
	int	coord_z = start_coord[2];
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
		{
			for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
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
		if (reload_amount >= max_get * 16)
			break;
	}

	int	coord_amount = 0;
	for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
	{
		for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
		{
			found = 0;
			for (int i = 0; i < info->chunks_loaded; i++)
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

/*
 * Returns amount of chunks that still need to get generated;
*/
int	regenerate_chunks(t_chunk *chunks, t_chunk_info *info, int *player_chunk_v3)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	start_coord[3];
	int found = 0;
	int reload_amount;
	
	reload_amount = get_chunks_to_reload(reload_these_chunks, start_coord, info, player_chunk_v3);
	if (reload_amount <= 0)
		return (reload_amount);

	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	int	nth_chunk = 0;

	for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
	{
		for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
		{
			found = 0;
			for (int i = 0; i < info->chunks_loaded; i++)
			{
				if (chunks[i].coordinate[0] == x && chunks[i].coordinate[2] == z)
				{
					found = 1;
					break ;
				}
			}
			if (!found)
			{
				int	noise_map[256];
				create_noise_map(noise_map, 16, 16, x, z);
				for (int y = start_coord[1], y_amount = 0; y_amount < info->y_chunk_amount; y++, y_amount++)
				{
					if (nth_chunk >= 16) // MUST BE 16
						break ;
					int index = reload_these_chunks[nth_chunk];
					chunks[index].args.chunk = &chunks[index];
					chunks[index].args.coords[0] = x;
					chunks[index].args.coords[1] = y;
					chunks[index].args.coords[2] = z;
					chunks[index].args.noise_map = noise_map;
					update_chunk_threaded_v2(&chunks[index].args);
					nth_chunk++;
					if (nth_chunk >= info->chunks_loaded) 
						break ;
				}
			}
			if (nth_chunk >= info->chunks_loaded || nth_chunk >= 16) 
				break ;
		}
		if (nth_chunk >= info->chunks_loaded || nth_chunk >= 16) 
			break ;
	}
	return (reload_amount - nth_chunk);
}

int	regenerate_chunks_v2(int *these, int coord[2], t_chunk_info *info)
{
	int			nth_chunk = 0;
	int			noise_map[256];

	create_noise_map(noise_map, 16, 16, coord[0], coord[1]);
	for (int y = 0; y < 16; y++)
	{
		if (nth_chunk >= 16 || nth_chunk >= info->chunks_loaded) 
			break ;
		int index = these[nth_chunk];
		info->chunks[index].args.chunk = &info->chunks[index];
		info->chunks[index].args.coords[0] = coord[0];
		info->chunks[index].args.coords[1] = y;
		info->chunks[index].args.coords[2] = coord[1];
		info->chunks[index].args.noise_map = noise_map;
		update_chunk_threaded_v2(&info->chunks[index].args);
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
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);

	// HORIZONTAL LINES Bottom
	render_3d_line(
		(float []){a->min[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->min[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->min[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->min[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->min[1], a->min[2]},
		col,
		camera->view, camera->projection);
	// TOP
	render_3d_line(
		(float []){a->min[0], a->max[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->max[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->max[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->max[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
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
void	update_chunk_mesh_v2(t_chunk_mesh_v2 *mesh)
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

	i = 0;
	for (; i < chunk->block_amount; i++)
	{
		// not solid and not flora, we can just skip it;
		if (!is_solid(&chunk->blocks[i]) &&
			!is_flora(&chunk->blocks[i]))
			continue;
		get_block_local_pos_from_index(blocal, i);
		block_world_pos(block_world, chunk->world_coordinate, blocal);
		if (is_solid(&chunk->blocks[i]))
		{
			if (is_hovering_solid_block(block_world, point, face))
			{
				vec3_assign(block_pos, block_world);
				return (&chunk->blocks[i]);
			}
		}
		else if (is_flora(&chunk->blocks[i]))
		{
			if (is_hovering_flora_block(block_world, point, face))
			{
				vec3_assign(block_pos, block_world);
				return (&chunk->blocks[i]);
			}
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
	// Front
	float	p1[3]; // top left
	float	p2[3]; // bot left
	float	p3[3]; // bot right
	float	p4[3]; // top right

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

	// Back
	float	b1[3]; // top left
	float	b2[3]; // bot left
	float	b3[3]; // bot right
	float	b4[3]; // top right

	b1[0] = (g_faces[1][0] * 0.5) + pos[0];
	b1[1] = (g_faces[1][1] * 0.5) + pos[1];
	b1[2] = (g_faces[1][2] * 0.5) + pos[2];

	b2[0] = (g_faces[1][3] * 0.5) + pos[0];
	b2[1] = (g_faces[1][4] * 0.5) + pos[1];
	b2[2] = (g_faces[1][5] * 0.5) + pos[2];

	b3[0] = (g_faces[1][6] * 0.5) + pos[0];
	b3[1] = (g_faces[1][7] * 0.5) + pos[1];
	b3[2] = (g_faces[1][8] * 0.5) + pos[2];

	b4[0] = (g_faces[1][9] * 0.5) + pos[0];
	b4[1] = (g_faces[1][10] * 0.5) + pos[1];
	b4[2] = (g_faces[1][11] * 0.5) + pos[2];

	// Front
	render_3d_line(p1, p2, color, view, projection);
	render_3d_line(p2, p3, color, view, projection);
	render_3d_line(p3, p4, color, view, projection);
	render_3d_line(p4, p1, color, view, projection);

	// Back
	render_3d_line(b1, b2, color, view, projection);
	render_3d_line(b2, b3, color, view, projection);
	render_3d_line(b3, b4, color, view, projection);
	render_3d_line(b4, b1, color, view, projection);

	// Top / Bot
	render_3d_line(p1, b4, color, view, projection);
	render_3d_line(p4, b1, color, view, projection);

	render_3d_line(p2, b3, color, view, projection);
	render_3d_line(p3, b2, color, view, projection);
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
		get_chunk_pos_from_world_pos(player_chunk, pos, info);
		player_collision_amount = 0;
		vec3_normalize(normed_velocity, final);
		for (int i = 0; i < info->chunks_loaded; i++)
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

	get_chunk_pos_from_world_pos(chunk_pos, world_pos, info);
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
	chunk->has_blocks = 1;
}

void	set_block_at_world_pos_if_empty(t_chunk_info *info, float *world_pos, int block_type)
{
	int		block_local[3];
	int		chunk_pos[3];
	int		index;
	t_chunk	*chunk;

	get_chunk_pos_from_world_pos(chunk_pos, world_pos, info);
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
	get_chunk_pos_from_world_pos(chunk_pos, under_block, info);
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

	for (int i = 0; i < info->chunks_loaded; i++)
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
	if (highest_index >= 0 && highest_index < info->chunks_loaded)
		return (&info->chunks[highest_index]);
	return (NULL);

	/*
	int	coords[3];
	int	highest = -1;
	int	highest_index = -1;
	unsigned long int	key;
	t_hash_item	*item;
	t_chunk	*chunk;

	coords[0] = x;
	coords[2] = z;
	for (int y = 0; y < 16; y++)
	{
		coords[1] = y;
		key = get_chunk_hash_key(coords);
		item = hash_item_search(info->hash_table, info->hash_table_size, key);
		if (item)
		{
			chunk = &info->chunks[item->data];
			if (chunk->has_blocks && y > highest)
			{
				highest = y;
				highest_index = item->data;
			}
		}
	}
	if (highest_index >= 0 && highest_index < info->chunks_loaded)
		return (&info->chunks[highest_index]);
	return (NULL);
	*/
}

/*
 * Returns highest point in world at x, z (world_position);
*/
float	get_highest_point(t_chunk_info *info, float x, float z)
{
	int		chunk_pos[3];
	float	curr_highest = -1;
	t_chunk	*highest_chunk;

	get_chunk_pos_from_world_pos(chunk_pos, (float []){x, 0, z}, info);
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

	get_chunk_pos_from_world_pos(chunk_pos, (float []){x, 0, z}, info);
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

	for (int x = 0; x < chunk->info->width; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < chunk->info->breadth; z++)
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