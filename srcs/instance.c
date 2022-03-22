#include "shaderpixel.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/

void	new_chunk(t_chunk *chunk, t_chunk_info *info, float *coord)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	chunk->needs_to_update = 0;

	int	max_blocks = chunk->info->width * chunk->info->breadth * chunk->info->height;

	chunk->blocks = malloc(sizeof(t_block) * (max_blocks));
	chunk->block_matrices = malloc(sizeof(float) * 16 * max_blocks);
	chunk->block_textures = malloc(sizeof(int) * max_blocks);

	chunk->blocks_visible = malloc(sizeof(t_block) * max_blocks);

	mat4_identity(chunk->block_matrices);
	chunk->block_textures[0] = 0;

	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size[0],
		chunk->coordinate[1] * chunk->info->chunk_size[1],
		chunk->coordinate[2] * chunk->info->chunk_size[2]);
	
	// Matrices
	glGenBuffers(1, &chunk->vbo_matrices);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_matrices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, &chunk->block_matrices[0], GL_STATIC_DRAW);

	glBindVertexArray(chunk->model.info[0].vao);

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), NULL);
	glVertexAttribDivisor(4, 1);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void *)(1 * 4 * sizeof(float)));
	glVertexAttribDivisor(5, 1);

	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void *)(2 * 4 * sizeof(float)));
	glVertexAttribDivisor(6, 1);

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float), (void *)(3 * 4 * sizeof(float)));
	glVertexAttribDivisor(7, 1);

	// Texture ID
	glGenBuffers(1, &chunk->vbo_texture_ids);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_texture_ids);
	glBufferData(GL_ARRAY_BUFFER, sizeof(int), &chunk->block_textures[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(8);
	glVertexAttribIPointer(8, 1, GL_INT, GL_FALSE, NULL);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

int	chunk_gen(t_chunk *chunk)
{
	int		start_y = 64;
//	float	freq = 120.0f;
	float	freq = 0.008f;
	float	height = freq * 100; // less is more;
	int		i = 0;

	for (int x = 0; x < chunk->info->width; x++)
	{
		float	block_world_x = (chunk->world_coordinate[0] + ((float)x * chunk->info->block_size));
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < chunk->info->breadth; z++)
		{
			float	block_world_z = (chunk->world_coordinate[2] + ((float)z * chunk->info->block_size));
			float	to_use_z = block_world_z * freq;

			float	perper = 1 * perlin(1 * to_use_x, 1 * to_use_z, chunk->info->seed);
//			float	perper2 = 1 * perlin_v2(1 * to_use_x, chunk->info->seed, 1 * to_use_z);
			int		wanted_y = start_y + (start_y * perper);
			int		whatchumacallit = wanted_y - (chunk->coordinate[1] * chunk->info->height);
			int		amount = ft_clamp(whatchumacallit, 0, chunk->info->height - 1);

			for (int y = 0; y < chunk->info->height; y++)
			{
				/* ////// CAVE GEN ///////// */
				/*
				float	cave_freq = 0.005f;
				float	cave_height = cave_freq * 200;
				float	cave_x = block_world_x * cave_freq;
				float	cave_z = block_world_z * cave_freq;
				float	cave_y = (chunk->world_coordinate[1] + ((float)y * chunk->info->block_size)) * cave_freq;
				*/
				float	rep = 1.0f;
				/*
				rep = perlin3(cave_x, cave_y, cave_z, chunk->info->seed)
					+ 0.5 * perlin3(2 * cave_x, 2 * cave_y, 2 * cave_z, chunk->info->seed)
					+ 0.25 * perlin3(4 * cave_x, 4 * cave_y, 4 * cave_z, chunk->info->seed)
					+ 0.125 * perlin3(8 * cave_x, 8 * cave_y, 8 * cave_z, chunk->info->seed);
				rep /= 1 + 0.5 + 0.25 + 0.125;
				if (rep > 0)
					rep = powf(rep, cave_height);
				rep = octave_perlin(cave_x, cave_y, cave_z, 1, 0.5);
					*/
				chunk->blocks[i].chunk = chunk;
				vec3_new(chunk->blocks[i].pos, x, y, z);
				if (rep > 0.0f && y <= whatchumacallit)
				{
					if (y <= whatchumacallit - 1) // if we have 3 dirt block on top we make the rest stone blocks;
						chunk->blocks[i].type = BLOCK_STONE;
					else
						chunk->blocks[i].type = BLOCK_DIRT;
				}
				else
					chunk->blocks[i].type = BLOCK_AIR;
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

void	render_chunk(t_chunk *chunk, t_camera *camera, t_shader *shader)
{
	char	tmp_str[25];
	char	tmp_num[25];

	int error = glGetError();

	glUseProgram(shader->program);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, &camera->projection[0]);

	glBindVertexArray(chunk->model.info[0].vao);

	if (chunk->model.info[0].elem_info[0].material && chunk->model.info[0].elem_info[0].material->loaded)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, chunk->model.info[0].elem_info[0].material->texture);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, chunk->model.info[0].elem_info[0].ebo);
	glDrawElementsInstanced(GL_TRIANGLES, chunk->model.info[0].elem_info[0].element.indices_value_amount , GL_UNSIGNED_INT, NULL, chunk->block_amount);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * Returns pointer to chunk if we can find the correct one;
 * 	else NULL;
 * 
 * 'from' is the chunk we want to look in a direction from,
 * 'chunks' is all the loaded chunks,
 * 'dir' is the direction you want to look for the chunk in; (v3)
*/
t_chunk	*get_adjacent_chunk(t_chunk *from, t_chunk *chunks, float *dir)
{
	for (int i = 0; i < from->info->chunks_loaded; i++)
	{
		if (chunks[i].coordinate[0] == from->coordinate[0] + dir[0] &&
			chunks[i].coordinate[1] == from->coordinate[1] + dir[1] &&
			chunks[i].coordinate[2] == from->coordinate[2] + dir[2])
			return (&chunks[i]);
	}
	return (NULL);
}

t_chunk	*get_chunk(t_chunk_info	*info, float *chunk_pos)
{
	float	pos[VEC3_SIZE];

	pos[0] = floor(chunk_pos[0]);
	pos[1] = floor(chunk_pos[1]);
	pos[2] = floor(chunk_pos[2]);
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		if (info->chunks[i].coordinate[0] == pos[0] && 
			info->chunks[i].coordinate[1] == pos[1] && 
			info->chunks[i].coordinate[2] == pos[2])
			return (&info->chunks[i]);
	}
	return (NULL);
}

int	get_block_index(t_chunk_info *info, int x, int y, int z)
{
	return ((x * info->width * info->breadth) + (z * info->height) + y);
}

/*
 * Dont use this before you know how slow this is;
 *
 * !!!!!REMEMBER!!!!!
 * This block_pos is the world coordinates of the block;
 * The t_block->pos is the chunk coordinate;
*/
t_block	*get_block(t_chunk_info	*info, float *block_pos)
{
	float	chunk_coord[3];
	t_chunk	*in;

	chunk_coord[0] = block_pos[0] / info->width;
	chunk_coord[1] = block_pos[1] / info->height;
	chunk_coord[2] = block_pos[2] / info->breadth;
	in = get_chunk(info, chunk_coord);
	if (!in)
		return (NULL);
	int	x = (int)block_pos[0] % info->width;
	int	y = (int)block_pos[1] % info->height;
	int	z = (int)block_pos[2] % info->breadth;
	if (x < 0 || x > 15 || y < 0 || y > 15 || z < 0 || z > 15)
		return (NULL);
	return (&in->blocks[get_block_index(info, x, y, z)]);
}

float	*get_block_world_pos(float *res, t_block *block)
{
	res[0] = (block->chunk->world_coordinate[0]) + (block->pos[0] * block->chunk->info->block_size);
	res[1] = (block->chunk->world_coordinate[1]) + (block->pos[1] * block->chunk->info->block_size);
	res[2] = (block->chunk->world_coordinate[2]) + (block->pos[2] * block->chunk->info->block_size);
	return (res);
}

/*
 * From 'index' in 'chunk->blocks' get the x,y,z pos in the chunk coordinates;
 * 	'max' is the max width, breadth and height of the chunk;
*/
int	*get_block_chunk_pos_from_index(int *res, int *max, int index)
{
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
	int		a = -1;

	blocks = chunk->blocks;

	/* MAKE ONLY TOUCHING AIR VISIBLE */
	/*
	for (int i = 0; i < chunk->block_amount; i++)
	{
		if (blocks[i].type == BLOCK_AIR)
			continue ;
		for (int j = 0; j < chunk->block_amount; j++)
		{
			if (blocks[j].type == BLOCK_AIR && // <- checking this first with 10 render distance takes time from 24s to 8s;
				vec3_dist(blocks[i].pos, blocks[j].pos) <= 1.0f)
			{
				chunk->blocks_visible[++a] = blocks[i];
				break ;
			}
		}
	}
	*/

	/* MAKE ONLY TOUCHING AIR VISIBLE */
	/* More improvements, instead of looping twice through all the blocks */
	/* We can just yoink the correct block from the array, now that they are all in order */
	float	i_block_w[VEC3_SIZE]; // block world pos for index i;
	float	j_block_w[VEC3_SIZE]; // block world pos for index j;
	int		enable_adjacent_chunk = 1;
	t_chunk	*adj_chunk = NULL;
	t_block	*tmp_block = NULL;
	int		pos[3];
	int	j;// = get_block_index(chunk->info, x, y, z);
	for (int i = 0; i < chunk->block_amount; i++)
	{
		if (blocks[i].type == BLOCK_AIR) // <-- very important, im not sure what happens if we are trying to render an air block;
			continue ;

		get_block_chunk_pos_from_index(pos, (int []){16, 16, 16}, i);
		int x = pos[0];
		int y = pos[1];
		int z = pos[2];

		get_block_world_pos(i_block_w, &blocks[i]);

		// left
		if (x - 1 >= 0)
		{
			j = get_block_index(chunk->info, x - 1, y, z);
			if (blocks[j].type == BLOCK_AIR)
				chunk->blocks_visible[++a] = blocks[i];
		}
		else if (enable_adjacent_chunk)
		{
			adj_chunk = get_adjacent_chunk(chunk, chunk->info->chunks, (float []){-1, 0, 0});
			if (adj_chunk)
			{
				tmp_block = &adj_chunk->blocks[get_block_index(chunk->info, 15, y, z)];
				if (tmp_block && tmp_block->type == BLOCK_AIR)
					chunk->blocks_visible[++a] = blocks[i];
			}
		}

		// right
		if (x + 1 < 16)
		{
			j = get_block_index(chunk->info, x + 1, y, z);
			if (blocks[j].type == BLOCK_AIR)
				chunk->blocks_visible[++a] = blocks[i];
		}
		else if (enable_adjacent_chunk)
		{
			adj_chunk = get_adjacent_chunk(chunk, chunk->info->chunks, (float []){1, 0, 0});
			if (adj_chunk)
			{
				tmp_block = &adj_chunk->blocks[get_block_index(chunk->info, 0, y, z)];
				if (tmp_block && tmp_block->type == BLOCK_AIR)
					chunk->blocks_visible[++a] = blocks[i];
			}
		}

		// top
		if (y + 1 < 16)
		{
			j = get_block_index(chunk->info, x, y + 1, z);
			if (blocks[j].type == BLOCK_AIR)
				chunk->blocks_visible[++a] = blocks[i];
		}
		else if (enable_adjacent_chunk)
		{
			adj_chunk = get_adjacent_chunk(chunk, chunk->info->chunks, (float []){0, 1, 0});
			if (adj_chunk)
			{
				tmp_block = &adj_chunk->blocks[get_block_index(chunk->info, x, 0, z)];
				if (tmp_block && tmp_block->type == BLOCK_AIR)
					chunk->blocks_visible[++a] = blocks[i];
			}
		}

		// bot
		if (y - 1 >= 0)
		{
			j = get_block_index(chunk->info, x, y - 1, z);
			if (blocks[j].type == BLOCK_AIR)
				chunk->blocks_visible[++a] = blocks[i];
		}
		else if (enable_adjacent_chunk)
		{
			adj_chunk = get_adjacent_chunk(chunk, chunk->info->chunks, (float []){0, -1, 0});
			if (adj_chunk)
			{
				tmp_block = &adj_chunk->blocks[get_block_index(chunk->info, x, 15, z)];
				if (tmp_block && tmp_block->type == BLOCK_AIR)
					chunk->blocks_visible[++a] = blocks[i];
			}
		}

		// forward
		if (z + 1 < 16)
		{
			j = get_block_index(chunk->info, x, y, z + 1);
			if (blocks[j].type == BLOCK_AIR)
				chunk->blocks_visible[++a] = blocks[i];
		}
		else if (enable_adjacent_chunk)
		{
			adj_chunk = get_adjacent_chunk(chunk, chunk->info->chunks, (float []){0, 0, 1});
			if (adj_chunk)
			{
				tmp_block = &adj_chunk->blocks[get_block_index(chunk->info, x, y, 0)];
				if (tmp_block && tmp_block->type == BLOCK_AIR)
					chunk->blocks_visible[++a] = blocks[i];
			}
		}

		// backward
		if (z - 1 >= 0)
		{
			j = get_block_index(chunk->info, x, y, z - 1);
			if (blocks[j].type == BLOCK_AIR)
				chunk->blocks_visible[++a] = blocks[i];
		}
		else if (enable_adjacent_chunk)
		{
			adj_chunk = get_adjacent_chunk(chunk, chunk->info->chunks, (float []){0, 0, -1});
			if (adj_chunk)
			{
				tmp_block = &adj_chunk->blocks[get_block_index(chunk->info, x, y, 15)];
				if (tmp_block && tmp_block->type == BLOCK_AIR)
					chunk->blocks_visible[++a] = blocks[i];
			}
		}


	}

	/* MAKE ALL VISIBLE */
	/*
	for (int i = 0; i < chunk->block_amount; i++)
	{
		if (blocks[i].type == BLOCK_AIR)
			continue ;
		chunk->blocks_visible[++a] = blocks[i];
	}
	*/
	return (a + 1); // '+ 1' because we start at '-1';
}

void	update_chunk(t_chunk *chunk, float *coord)
{
	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size[0],
		chunk->coordinate[1] * chunk->info->chunk_size[1],
		chunk->coordinate[2] * chunk->info->chunk_size[2]);
	
	// Generate Chunks	
	chunk->block_amount = chunk_gen(chunk); // should always return max amount of blocks in a chunk;

	// Check which are touching air;
	/*
	chunk->blocks_visible_amount = get_blocks_visible(chunk);
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->blocks_visible_amount;
	chunk->block_textures_size = sizeof(int) * chunk->blocks_visible_amount;
	*/
	/* Call update_chunk_visible_blocks() instead of the lines above. */

//	ft_printf("Rendered %d/%d\n", chunk->blocks_visible_amount, chunk->block_amount);
	chunk->needs_to_update = 1;
}

void	update_chunk_matrices(t_chunk *chunk)
{
	float	tmp[VEC3_SIZE];
	float	model[MAT4_SIZE];
	float	scale[MAT4_SIZE];
//	float	rot[MAT4_SIZE];
	float	trans[MAT4_SIZE];

	for (int i = 0; i < chunk->blocks_visible_amount; i++)
	{
		mat4_identity(trans);
		mat4_translate(trans, trans, vec3_new(tmp,
			(chunk->blocks_visible[i].pos[0] * chunk->info->block_size) + chunk->world_coordinate[0],
			(chunk->blocks_visible[i].pos[1] * chunk->info->block_size) + chunk->world_coordinate[1],
			(chunk->blocks_visible[i].pos[2] * chunk->info->block_size) + chunk->world_coordinate[2]));

		mat4_identity(scale);
		mat4_scale(scale, scale, vec3_new(tmp,
			chunk->info->block_scale,
			chunk->info->block_scale,
			chunk->info->block_scale));

		mat4_identity(model);
		mat4_multiply(model, scale, model);
		mat4_multiply(model, trans, model);

		memcpy(chunk->block_matrices + (i * 16), model, sizeof(float) * 16);
		memcpy(chunk->block_textures + (i), &chunk->blocks_visible[i].type, sizeof(int));
	}
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
/*
int	get_chunks_to_reload(int *chunks, int *start_coord, t_chunk_info *info, float *player_chunk_v3)
{
	int	reload_amount = 0;
	int	found = 0;

	start_coord[0] = player_chunk_v3[0] - (info->render_distance / 2);
	start_coord[1] = player_chunk_v3[2] - (info->render_distance / 2);
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
		{
			for (int z = start_coord[1], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
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
	int				max_threads = 16; // minimum amount of chunks on height;
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
					vec3_new(chunks[index].args.coords, x, y, z);
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
					vec3_new(chunks[index].args.coords, x, y, z);
					chunks[index].args.being_threaded = 1;
					LG_INFO("Threading Chunk at %f %f %f", chunks[index].coordinate[0], chunks[index].coordinate[1], chunks[index].coordinate[2]);
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
	t_aabb *a;
	
	a = &chunk->aabb;
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

void	block_aabb_update(t_aabb *res, t_block *block)
{
	res->min[0] = block->chunk->world_coordinate[0] * block->chunk->info->block_size - (block->chunk->info->block_size / 2);
	res->min[1] = block->chunk->world_coordinate[1] * block->chunk->info->block_size - (block->chunk->info->block_size / 2);
	res->min[2] = block->chunk->world_coordinate[2] * block->chunk->info->block_size - (block->chunk->info->block_size / 2);
	res->max[0] = res->min[0] + block->chunk->info->block_size;
	res->max[1] = res->min[1] + block->chunk->info->block_size;
	res->max[2] = res->min[2] + block->chunk->info->block_size;
}

/*
 * update 'chunk_aabb' with the info from the 'chunk';
*/
void	update_chunk_aabb(t_chunk_block_aabb *chunk_block_aabb, t_chunk *chunk)
{
	for (int i = 0; i < chunk_block_aabb->block_amount; i++)
	{
		chunk_block_aabb->block_pointers[i] = &chunk->blocks[i];
		block_aabb_update(&chunk_block_aabb->aabb[i],
			chunk_block_aabb->block_pointers[i]);
	}
}

/*
 * Update all the chunks surrounding the player;
*/
void	update_surrounding_chunk_aabbs(t_chunk *chunks, float *player_chunk_v3)
{
	t_chunk_info	*info;
	t_chunk			*tmp;
	t_chunk			*player_chunk;

	info = chunks[0].info;
	vec3_string("player_chunk :", player_chunk_v3);
	player_chunk = get_chunk(info, player_chunk_v3);
}

/*
 * I think this has to be called after all the chunk creation since we need all the adjacent
 *	chunks to already exist, and if the update_chunk is multithreaded we might not have 
 *	created the needed chunks yet;
*/
void	update_chunk_visible_blocks(t_chunk *chunk)
{
	chunk->blocks_visible_amount = get_blocks_visible(chunk);
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->blocks_visible_amount;
	chunk->block_textures_size = sizeof(int) * chunk->blocks_visible_amount;
}