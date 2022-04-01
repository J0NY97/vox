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
		chunk->coordinate[i] = 999 - (i * nth);
		chunk->world_coordinate[i] = 999 - (i * nth);
	}
	/*
	int key = get_chunk_hash_key(chunk->coordinate);
	hash_item_insert(info->hash_table, info->hash_table_size, key, nth);
	*/

	init_chunk_mesh(&chunk->mesh);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

int	chunk_gen(t_chunk *chunk)
{
	int		start_y = 64;
	float	freq = 0.005f;
	float	pers = 0.5;
	int		i = 0;

	chunk->has_blocks = 0;

	for (int x = 0; x < chunk->info->width; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < chunk->info->breadth; z++)
		{
			float	block_world_z = fabs(chunk->world_coordinate[2] + z);
			float	to_use_z = block_world_z * freq;
			float	perper =
				octave_perlin(to_use_x, start_y * freq, to_use_z, 1, pers) +
				octave_perlin(to_use_x, start_y * freq, to_use_z, 2, pers) +
				octave_perlin(to_use_x, start_y * freq, to_use_z, 4, pers) +
				octave_perlin(to_use_x, start_y * freq, to_use_z, 8, pers);
			float	e = pers * 3;
			int		wanted_y = (start_y * (perper / e));
			int		whatchumacallit = wanted_y - (chunk->world_coordinate[1]);
			int		amount = ft_clamp(whatchumacallit, 0, chunk->info->height - 1);

			for (int y = 0; y < chunk->info->height; y++)
			{
				float	block_world_y = (chunk->world_coordinate[1] + y);
				/* ////// CAVE GEN ///////// */
				/*
				float	cave_freq = 0.005f;
				float	cave_height = cave_freq * 200;
				float	cave_x = block_world_x * cave_freq;
				float	cave_z = block_world_z * cave_freq;
				float	cave_y = (chunk->world_coordinate[1] + y) * cave_freq;
				float	rep = 1.0f;

				rep =
					octave_perlin(cave_x, cave_y, cave_z, 2, 0.5) +
					octave_perlin(cave_x, cave_y, cave_z, 4, 0.5) +
					octave_perlin(cave_x, cave_y, cave_z, 8, 0.5) +
					octave_perlin(cave_x, cave_y, cave_z, 16, 0.5);
					*/

//				vec3_new(chunk->blocks[i].pos, x, y, z);
				if (/*rep > 1.0f &&*/ y <= whatchumacallit)
				{
					if (y <= whatchumacallit - 1) // if we have 3 dirt block on top we make the rest stone blocks;
					{
						chunk->blocks[i].type = BLOCK_STONE;
					}
					else
					{
						if (block_world_y <= 67 && block_world_y >= 65)
							chunk->blocks[i].type = BLOCK_SAND;
						else
							chunk->blocks[i].type = BLOCK_DIRT;
					}
					chunk->has_blocks = 1;
				}
				else
				{
					if (block_world_y == 65)
					{
						chunk->blocks[i].type = BLOCK_WATER;
						chunk->has_blocks = 1;
					}
					else
						chunk->blocks[i].type = BLOCK_AIR;
				}
				i++;
			}
		}
	}
	return (i);
}

float	*player_in_chunk(float *res, float *player_coord, t_chunk_info *info)
{
	res[0] = floor(player_coord[0] / info->chunk_size[0]);
	res[1] = floor(player_coord[1] / info->chunk_size[1]);
	res[2] = floor(player_coord[2] / info->chunk_size[2]);
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
t_chunk	*get_adjacent_chunk(t_chunk *from, t_chunk *chunks, int *dir)
{
	int	from_coord[3];

	from_coord[0] = from->coordinate[0] + dir[0];
	from_coord[1] = from->coordinate[1] + dir[1];
	from_coord[2] = from->coordinate[2] + dir[2];
/*
	int key = get_chunk_hash_key(from_coord);
	t_hash_item *item = hash_item_search(from->info->hash_table, from->info->hash_table_size, key);
	if (!item)
		return (NULL);
	return (&from->info->chunks[item->data]);
	*/

	for (int i = 0; i < from->info->chunks_loaded; i++)
	{
		if (chunks[i].coordinate[0] == from_coord[0] &&
			chunks[i].coordinate[1] == from_coord[1] &&
			chunks[i].coordinate[2] == from_coord[2])
			return (&chunks[i]);
	}
	return (NULL);
	/*
	*/
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

int	*block_world_to_local_pos(int *res, float *world)
{
	res[0] = mod(world[0], 16);
	res[1] = mod(world[1], 16);
	res[2] = mod(world[2], 16);
	return (res);
}

/*
 * From 'index' in 'chunk->blocks' get the x,y,z pos in the chunk coordinates;
 * 	'max' is the max width, breadth and height of the chunk;
*/
int	*get_block_local_pos_from_index(int *res, int *max, int index)
{
	// TODO use chunk->w, h, b;
	res[0] = index / (16 * 16);
	res[2] = (index / 16) % 16;
	res[1] = index % 16;
	return (res);
}

/*
 * For each block in chunk;
 *	Go through each block in chunk;
 *		check if not touching air blocks;
 *	if touching;
 *		add to render array;
 *
 * THIS MIGHT ONLY WORK NOW THAT THE CHUNKS ARE SYMMETRIC, IF SOMETHING BREAKS
 * CHECK THAT THE XYZ CORRESPOND CORRECTLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
int	get_blocks_visible(t_chunk *chunk)
{
	t_block	*blocks;
	int		blocks_visible = 0;

	if (!chunk->has_blocks)
		return (0);
	blocks = chunk->blocks;

	/* MAKE ONLY TOUCHING AIR VISIBLE */
	t_chunk	*adj_chunk = NULL;
	t_block	*tmp_block = NULL;

	t_chunk *neighbors[6];
	for (int i = 0; i < 6; i++)
		neighbors[i] = get_adjacent_chunk(chunk, chunk->info->chunks, (int *)g_neighbors[i]);

	int		pos[3];
	int		j;// = get_block_index(chunk->info, x, y, z);
	for (int i = 0; i < chunk->block_amount; i++)
	{
		int	a = 0;
		if (blocks[i].type == BLOCK_AIR) // <-- very important, im not sure what happens if we are trying to render an air block;
			continue ;

		get_block_local_pos_from_index(pos, (int []){16, 16, 16}, i);
		int x = pos[0];
		int y = pos[1];
		int z = pos[2];

	if (1)
	{
		// left
		tmp_block = NULL;
		if (x - 1 >= 0)
		{
			j = get_block_index(chunk->info, x - 1, y, z);
			tmp_block = &blocks[j];
		}
		else
		{
			if (neighbors[0])
				tmp_block = &neighbors[0]->blocks[get_block_index(chunk->info, 15, y, z)];
		}
		if (tmp_block && g_block_data[tmp_block->type + 1].solid == 0)
		{
			add_to_chunk_mesh(chunk, (int []){x, y, z}, (float *)g_left_face, g_block_data[blocks[i].type + 1].left_texture);
			a++;
		}

		// right
		tmp_block = NULL;
		if (x + 1 < 16)
		{
			j = get_block_index(chunk->info, x + 1, y, z);
			tmp_block = &blocks[j];
		}
		else
		{
			if (neighbors[1])
				tmp_block = &neighbors[1]->blocks[get_block_index(chunk->info, 0, y, z)];
		}
		if (tmp_block && g_block_data[tmp_block->type + 1].solid == 0)
		{
			add_to_chunk_mesh(chunk, (int []){x, y, z}, (float *)g_right_face, g_block_data[blocks[i].type + 1].right_texture);
			a++;
		}

		// top
		tmp_block = NULL;
		if (y + 1 < 16)
		{
			j = get_block_index(chunk->info, x, y + 1, z);
			tmp_block = &blocks[j];
		}
		else
		{
			if (neighbors[2])
				tmp_block = &neighbors[2]->blocks[get_block_index(chunk->info, x, 0, z)];
		}
		if (tmp_block && g_block_data[tmp_block->type + 1].solid == 0)
		{
			add_to_chunk_mesh(chunk, (int []){x, y, z}, (float *)g_top_face, g_block_data[blocks[i].type + 1].top_texture);
			a++;
		}

		// bot
		tmp_block = NULL;
		if (y - 1 >= 0)
		{
			j = get_block_index(chunk->info, x, y - 1, z);
			tmp_block = &blocks[j];
		}
		else
		{
			if (neighbors[3])
				tmp_block = &neighbors[3]->blocks[get_block_index(chunk->info, x, 15, z)];
		}
		if (tmp_block && g_block_data[tmp_block->type + 1].solid == 0)
		{
			add_to_chunk_mesh(chunk, (int []){x, y, z}, (float *)g_bot_face, g_block_data[blocks[i].type + 1].bot_texture);
			a++;
		}

		// forward
		tmp_block = NULL;
		if (z + 1 < 16)
		{
			j = get_block_index(chunk->info, x, y, z + 1);
			tmp_block = &blocks[j];
		}
		else
		{
			if (neighbors[4])
				tmp_block = &neighbors[4]->blocks[get_block_index(chunk->info, x, y, 0)];
		}
		if (tmp_block && g_block_data[tmp_block->type + 1].solid == 0)
		{
			add_to_chunk_mesh(chunk, (int []){x, y, z}, (float *)g_front_face, g_block_data[blocks[i].type + 1].front_texture);
			a++;
		}

		// backward
		tmp_block = NULL;
		if (z - 1 >= 0)
		{
			j = get_block_index(chunk->info, x, y, z - 1);
			tmp_block = &blocks[j];
		}
		else
		{
			if (neighbors[5])
				tmp_block = &neighbors[5]->blocks[get_block_index(chunk->info, x, y, 15)];
		}
		if (tmp_block && g_block_data[tmp_block->type + 1].solid == 0)
		{
			add_to_chunk_mesh(chunk, (int []){x, y, z}, (float *)g_back_face, g_block_data[blocks[i].type + 1].back_texture);
			a++;
		}
	}
		if (a)
			++blocks_visible;
	}

	return (blocks_visible);
}

int	get_chunk_hash_key(int *coords)
{
	int	res;

	res = coords[0] +
		(8 * coords[1]) +
		(8 * 8 * coords[2]);
	return (res);
}

void	update_chunk(t_chunk *chunk, int *coord)
{
	/*
	int	old_key = get_chunk_hash_key(chunk->coordinate);
	t_hash_item	*old_item = hash_item_search(chunk->info->hash_table, chunk->info->hash_table_size, old_key);
	int				old_data = old_item->data;
	hash_item_delete(chunk->info->hash_table, chunk->info->hash_table_size, old_key);
	*/

	for (int i = 0; i < 3; i++)
		chunk->coordinate[i] = coord[i];
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size[0],
		chunk->coordinate[1] * chunk->info->chunk_size[1],
		chunk->coordinate[2] * chunk->info->chunk_size[2]);

/*
	int	new_key = get_chunk_hash_key(chunk->coordinate);	
	hash_item_insert(chunk->info->hash_table, chunk->info->hash_table_size, new_key, old_data);
	*/

	// Generate Chunks	
	chunk->block_amount = chunk_gen(chunk); // should always return max amount of blocks in a chunk;

	chunk->needs_to_update = 1;
}

void	*update_chunk_threaded(void *arg)
{
	t_chunk_args	*info = arg;

	update_chunk(info->chunk, info->coords);
	info->chunk->args.being_threaded = 0;
	return (NULL);
}

/*
 * Check which chunks are not going to be in the next iteration of
 *	loaded chunks, save those to 'reload_these_chunks' and when starting
 *	to update the new chunks that are going to be loaded, and put the
 *	new chunk info into those 'chunks' indices;
 * Takes 0.000000 seconds;
*/
int	get_chunks_to_reload(int *chunks, int *start_coord, t_chunk_info *info, float *player_chunk_v3)
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
					found = 1;
			}
		}
		if (!found)
		{
			chunks[reload_amount] = i;
			reload_amount++;
		}
	}
//	LG_INFO("Chunk amount to reload : %d", reload_amount);
	return (reload_amount);
}

void	regenerate_chunks_v32(t_chunk *chunks, t_chunk_info *info, float *player_chunk_v3)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	start_coord[3];
	int found = 0;
	int reload_amount;
	
	reload_amount = get_chunks_to_reload(reload_these_chunks, start_coord, info, player_chunk_v3);
	if (reload_amount <= 0)
		return ;
	
	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	int				nth_chunk = 0;

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
				for (int y = start_coord[1], y_amount = 0; y_amount < info->y_chunk_amount; y++, y_amount++)
				{
					if (nth_chunk >= 15)
						break ;
					int index = reload_these_chunks[nth_chunk];
					chunks[index].args.chunk = &chunks[index];
					chunks[index].args.coords[0] = x;
					chunks[index].args.coords[1] = y;
					chunks[index].args.coords[2] = z;
					update_chunk_threaded(&chunks[index].args);
					nth_chunk++;
					if (nth_chunk >= info->chunks_loaded) 
						break ;
				}
			}
			if (nth_chunk >= info->chunks_loaded) 
				break ;
		}
		if (nth_chunk >= info->chunks_loaded) 
			break ;
	}
}

void	regenerate_chunks(int *res, t_chunk *chunks, t_chunk_info *info, float *player_chunk_v3)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	start_coord[3];
	int found = 0;
	int reload_amount;
	
	reload_amount = get_chunks_to_reload(reload_these_chunks, start_coord, info, player_chunk_v3);
	if (reload_amount <= 0)
		return ;
	
//	ft_timer_start();

	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	int				max_threads = 64; // minimum amount of chunks on height;
	pthread_t		threads[max_threads];
	int				nth_thread = 0;
	int				nth_chunk = 0;

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
				for (int y = start_coord[1], y_amount = 0; y_amount < info->y_chunk_amount; y++, y_amount++)
				{
					if (nth_thread >= max_threads)
					{
						break ;
						int i = 0;
						while (i < nth_thread)
						{
							if (pthread_join(threads[i], NULL))
								LG_ERROR("Couldnt join thread. -1-");
							i++;
						}
						nth_thread = 0;
					}
					int index = reload_these_chunks[nth_chunk];
					chunks[index].args.chunk = &chunks[index];
					chunks[index].args.coords[0] = x;
					chunks[index].args.coords[1] = y;
					chunks[index].args.coords[2] = z;
					if (pthread_create(&threads[nth_thread], NULL, update_chunk_threaded, &chunks[index].args))
						LG_ERROR("Couldnt create thread.");
					nth_chunk++;
					nth_thread++;
					if (nth_chunk >= info->chunks_loaded) 
						break ;
				}
			}
			if (nth_chunk >= info->chunks_loaded) 
				break ;
		}
		if (nth_chunk >= info->chunks_loaded) 
			break ;
	}

	int j = 0;
	while (j < nth_thread)
	{
		if (pthread_join(threads[j], NULL))
			LG_ERROR("Couldnt join thread. -2-");
		j++;
	}

	res[0] = reload_amount;
	res[1] = nth_chunk;

//	ft_printf("One chunk timer : %f\n", ft_timer_end());
}

void	regenerate_chunks_v3(int *res, t_chunk *chunks, t_chunk_info *info, float *player_chunk_v3, t_thread_manager *tm)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	start_coord[3];
	int found = 0;
	int reload_amount;
	
	reload_amount = get_chunks_to_reload(reload_these_chunks, start_coord, info, player_chunk_v3);
	if (reload_amount <= 0)
		return ;
	if (tm->thread_amount >= tm->max_thread_amount)
		return ;

	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	int				nth_chunk = 0;

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
				for (int y = start_coord[1], y_amount = 0; y_amount < info->y_chunk_amount; y++, y_amount++)
				{
					while (chunks[reload_these_chunks[nth_chunk]].args.being_threaded == 1 &&
						nth_chunk < info->chunks_loaded && nth_chunk < reload_amount)
						nth_chunk++;
					if (nth_chunk >= info->chunks_loaded || nth_chunk >= reload_amount) 
						break ;
					int index = reload_these_chunks[nth_chunk];
					chunks[index].args.chunk = &chunks[index];
					chunks[index].args.coords[0] = x;
					chunks[index].args.coords[1] = y;
					chunks[index].args.coords[2] = z;
					chunks[index].args.being_threaded = 1;
					LG_INFO("Threading Chunk at %d %d %d", chunks[index].coordinate[0], chunks[index].coordinate[1], chunks[index].coordinate[2]);
					if (!thread_manager_new_thread(tm, update_chunk_threaded, &chunks[index].args))
						LG_WARN("Couldnt created new thread, better luck next time.");
					nth_chunk++;
					if (nth_chunk >= info->chunks_loaded || nth_chunk >= reload_amount || tm->thread_amount >= tm->max_thread_amount) 
						break ;
				}
			}
			if (nth_chunk >= info->chunks_loaded || nth_chunk >= reload_amount || tm->thread_amount >= tm->max_thread_amount) 
				break ;
		}
		if (nth_chunk >= info->chunks_loaded || nth_chunk >= reload_amount || tm->thread_amount >= tm->max_thread_amount) 
			break ;
	}

	res[0] = reload_amount;
	res[1] = nth_chunk;
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
	chunk->mesh.vertices_amount = 0;
	chunk->mesh.texture_id_amount = 0;
	chunk->mesh.indices_amount = 0;
	chunk->mesh.index_amount = 0;

	chunk->blocks_visible_amount_prev = chunk->blocks_visible_amount;

	// TODO: only update if it has all its neighbors;
	chunk->blocks_visible_amount = get_blocks_visible(chunk);
}

/*
 * Creates all the opengl stuff that needs to be created only once;
*/
void	init_chunk_mesh(t_chunk_mesh *mesh)
{
	GLuint	vbo[2];
	int		error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	mesh->vertices_allocated = 28296;
	mesh->vertices = malloc(sizeof(float) * mesh->vertices_allocated);
	mesh->vertices_amount = 0;

	mesh->texture_ids_allocated = 9432; 
	mesh->texture_ids = malloc(sizeof(int) * mesh->texture_ids_allocated);
	mesh->texture_id_amount = 0;

	mesh->indices_allocated = 14148;
	mesh->indices = malloc(sizeof(unsigned int) * mesh->indices_allocated);
	mesh->indices_amount = 0;

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // tex

	glGenBuffers(2, vbo);
	mesh->vbo_pos = vbo[0];
	mesh->vbo_texture_ids = vbo[1];

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texture_ids);
	glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), NULL);

	glGenBuffers(1, &mesh->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

void	render_chunk_mesh(t_chunk *chunk, t_camera *camera, t_shader *shader)
{
	t_chunk_mesh	*mesh;

	int	error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	mesh = &chunk->mesh;

	glUseProgram(shader->program);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, &camera->projection[0]);

	glUniform3fv(glGetUniformLocation(shader->program, "chunkPos"), 1, &chunk->world_coordinate[0]);

	glBindVertexArray(mesh->vao);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glDrawElements(GL_TRIANGLES, mesh->indices_amount, GL_UNSIGNED_INT, NULL);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

void	update_chunk_mesh(t_chunk *chunk)
{
	int error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	glBindVertexArray(chunk->mesh.vao);

error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
	// Pos
	glBindBuffer(GL_ARRAY_BUFFER, chunk->mesh.vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, chunk->mesh.vertices_amount * sizeof(float),
		&chunk->mesh.vertices[0], GL_STATIC_DRAW);

	error = glGetError();
	if (error)
	{
		LG_INFO("Vertices Amount : %d", chunk->mesh.vertices_amount);
		LG_ERROR("(%d)", error);
	}

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, chunk->mesh.vbo_texture_ids);
	glBufferData(GL_ARRAY_BUFFER, chunk->mesh.texture_id_amount * sizeof(int),
		&chunk->mesh.texture_ids[0], GL_STATIC_DRAW);

error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->mesh.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, chunk->mesh.indices_amount * sizeof(unsigned int),
		&chunk->mesh.indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * 'coord' position of block in the chunk->blocks array;
*/
void	add_to_chunk_mesh(t_chunk *chunk, int *coord, float *face_vertices, int texture_id)
{
	t_chunk_mesh	*mesh;

	mesh = &chunk->mesh;

// Vertices and Texture
	if (mesh->vertices_allocated < mesh->vertices_amount + 12)
	{
		mesh->vertices_allocated += 256;
		mesh->vertices = realloc(mesh->vertices, sizeof(float) * mesh->vertices_allocated);
		LG_WARN("Reallocating Vertices from %d to %d", mesh->indices_allocated - 256, mesh->indices_allocated);
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
		mesh->vertices[mesh->vertices_amount + ind + 0] = (face_vertices[ind + 0] * chunk->info->block_scale) + coord[0];
		mesh->vertices[mesh->vertices_amount + ind + 1] = (face_vertices[ind + 1] * chunk->info->block_scale) + coord[1];
		mesh->vertices[mesh->vertices_amount + ind + 2] = (face_vertices[ind + 2] * chunk->info->block_scale) + coord[2];

		tex = texture_id | i << 16;
		mesh->texture_ids[mesh->texture_id_amount + i] = tex;
	}
	mesh->vertices_amount += 4 * 3;
	mesh->texture_id_amount += 4;

// Indices
	if (mesh->indices_allocated < mesh->indices_amount + 6)
	{
		mesh->indices_allocated += 256;
		mesh->indices = realloc(mesh->indices, sizeof(float) * mesh->indices_allocated);
		LG_WARN("Reallocating Indices from %d to %d", mesh->indices_allocated - 256, mesh->indices_allocated);
	}

	mesh->indices[mesh->indices_amount + 0] = mesh->index_amount;
	mesh->indices[mesh->indices_amount + 1] = mesh->index_amount + 1;
	mesh->indices[mesh->indices_amount + 2] = mesh->index_amount + 2;

	mesh->indices[mesh->indices_amount + 3] = mesh->index_amount;
	mesh->indices[mesh->indices_amount + 4] = mesh->index_amount + 2;
	mesh->indices[mesh->indices_amount + 5] = mesh->index_amount + 3;

	mesh->indices_amount += 6;
	mesh->index_amount += 4;
}

void	player_chunk_mesh_collision(t_player *player, t_chunk *chunk)
{
	float			*vertices;
	unsigned int	*indices;
	float			p1[3];
	float			p2[3];
	float			p3[3];
	float			intersect_p[3];
	float			player_dir[3];

	vec3_normalize(player_dir, player->velocity);
	vertices = chunk->mesh.vertices;
	indices = chunk->mesh.indices;

	int	chunk_collision = 0;
	for (int i = 0; i < chunk->mesh.indices_amount / 3; i++)
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
		if (ray_triangle_intersect(player->camera.pos, player->camera.front,
			p1, p2, p3, intersect_p))
		{
			chunk_collision = 1;
			render_3d_line(p1, p2, (float []){0, 1, 0}, player->camera.view, player->camera.projection);
			render_3d_line(p1, p3, (float []){0, 1, 0}, player->camera.view, player->camera.projection);
			render_3d_line(p3, p2, (float []){0, 1, 0}, player->camera.view, player->camera.projection);
		}
		else
		{
			render_3d_line(p1, p2, (float []){1, 0, 1}, player->camera.view, player->camera.projection);
			render_3d_line(p1, p3, (float []){1, 0, 1}, player->camera.view, player->camera.projection);
			render_3d_line(p3, p2, (float []){1, 0, 1}, player->camera.view, player->camera.projection);
		}

	}
	if (chunk_collision)
	{
		LG_INFO("Intersection time cmon!");
		vec3_string("Chunk At : ", chunk->world_coordinate);
		vec3_string("Player At : ", player->camera.pos);
		vec3_string("player.front : ", player->camera.front);
		vec3_string("player.velocity : ", player->velocity);
	}
}