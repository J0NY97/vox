#include "shaderpixel.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/

void	new_chunk(t_chunk *chunk, t_chunk_info *info, float *coord)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	int	max_blocks = chunk->info->width * chunk->info->breadth * chunk->info->height;

	chunk->blocks = malloc(sizeof(t_block) * (max_blocks));
	chunk->block_matrices = malloc(sizeof(float) * 16 * max_blocks);
	chunk->block_textures = malloc(sizeof(int) * max_blocks);

	chunk->blocks_to_render = malloc(sizeof(t_block) * max_blocks);

	mat4_identity(chunk->block_matrices);
	chunk->block_textures[0] = 0;

	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size, 1,
		chunk->coordinate[2] * chunk->info->chunk_size);
	
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

/*
 * Returns amount of blocks generated;
*/
int	chunk_gen(t_chunk *chunk)
{
	int		start_y = 65;
	float	freq = 120.0f;
	float	height = freq / 100; // less is more;
	int		i = 0;

	for (int x = 0; x < chunk->info->width; x++)
	{
		float	block_world_x = ((chunk->coordinate[0] * chunk->info->chunk_size) + ((float)x * chunk->info->block_size));
		float	to_use_x = block_world_x / freq;
		for (int z = 0; z < chunk->info->breadth; z++)
		{
			float	block_world_z = ((chunk->coordinate[2] * chunk->info->chunk_size) + ((float)z * chunk->info->block_size));
			float	to_use_z = block_world_z / freq;

			float	perper = 1 * perlin(1 * to_use_x, 1 * to_use_z, chunk->info->seed);
			perper = round(perper * (freq / 2)) / (freq / 2);
			if (perper < 0)
				perper = -powf(fabs(perper), height);
			else
				perper = powf(fabs(perper), height);
			perper = start_y * perper;
//			ft_printf("perlin : %f\n", perper);
			int	amount = min(start_y + perper, chunk->info->height);
			for (int y = amount, b = 0; b < amount; y--, b++) // the 'b' is the amount of blocks we have on the y axis;
			{
				float	cave_freq = 200.0f;
				float	cave_height = cave_freq / 200;
				float	cave_x = block_world_x / cave_freq;
				float	cave_z = block_world_z / cave_freq;
				float	cave_y = ((float)y * chunk->info->block_size) / cave_freq;
				float	rep = 1.0f;
				rep = perlin3(cave_x, cave_y, cave_z, chunk->info->seed)
					+ 0.5 * perlin3(2 * cave_x, 2 * cave_y, 2 * cave_z, chunk->info->seed)
					+ 0.25 * perlin3(4 * cave_x, 4 * cave_y, 4 * cave_z, chunk->info->seed)
					+ 0.125 * perlin3(8 * cave_x, 8 * cave_y, 8 * cave_z, chunk->info->seed);
				rep /= 1 + 0.5 + 0.25 + 0.125;
				if (rep > 0)
					rep = powf(rep, cave_height);
			//	ft_printf("to_use_x : %f, to_use_y : %f, to_use_z : %f\n", to_use_x, to_use_y, to_use_z);
			//	ft_printf("perlin3 : %f\n", rep);
				/*
				*/
				if (rep > -0.10f)
				{
					vec3_new(chunk->blocks[i].pos, x, y, z);
					if (b >= 1) // if we have 3 dirt block on top we make the rest stone blocks;
						chunk->blocks[i].texture_id = 1;
					else
						chunk->blocks[i].texture_id = 0;
					i++;
				}
			}
			vec3_new(chunk->blocks[i - 1].pos, x, 0, z);
			chunk->blocks[i - 1].texture_id = 2;
		}
	}
	return (i);
}

void	gen_chunk_blocks(t_block *blocks, int *dim)
{
	int	i = 0;

	for (int y = 0; y < dim[1]; y++)
	{
		for (int x = 0; x < dim[0]; x++)
		{
			for (int z = 0; z < dim[2]; z++)
			{
				vec3_new(blocks[i].pos, x, y, z);
				i++;
			}
		}
	}
}

float	*player_in_chunk(float *res, float *player_coord, t_chunk_info *info)
{
	res[0] = floor(player_coord[0] / info->chunk_size);
	res[1] = floor(player_coord[2] / info->chunk_size);
	return (res);
}

void	chunk_aabb_update(t_chunk *chunk)
{
	t_aabb	*a;

	a = &chunk->aabb;
	a->min[0] = chunk->world_coordinate[0] - (chunk->info->block_size / 2);
	a->min[1] = 0;
	a->min[2] = chunk->world_coordinate[2] - (chunk->info->block_size / 2);
	a->max[0] = a->min[0] + chunk->info->chunk_size;
	a->max[1] = chunk->info->height * chunk->info->block_size;
	a->max[2] = a->min[2] + chunk->info->chunk_size;
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
 * For each block in chunk;
 *	Go through each block in chunk;
 *		check if not touching 4 blocks;
 *	if not 4;
 *		add to render array;
*/
int	get_blocks_to_render(t_chunk *chunk)
{
	t_block	*blocks;
	int		a = 0;

	blocks = chunk->blocks;
	for (int i = 0; i < chunk->block_amount; i++)
	{
		int	touching = 0;
		for (int j = 0; j < chunk->block_amount; j++)
		{
			if (vec3_dist(blocks[i].pos, blocks[j].pos) <= 1)
				touching++;
			if (touching >= 6)
				break ;
		}
		if (touching < 6)
		{
			chunk->blocks_to_render[a] = blocks[i];
			a++;
		}
	}
	return (a);
}

typedef struct s_chunk_args
{
	t_chunk	*chunk;
	float	coords[VEC3_SIZE];
}	t_chunk_args;

void	update_chunk(t_chunk *chunk, float *coord)
{
	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size, 1,
		chunk->coordinate[2] * chunk->info->chunk_size);

	// Generate Chunks	
	chunk->block_amount = chunk_gen(chunk);
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->block_amount;
	chunk->block_textures_size = sizeof(int) * chunk->block_amount;

	// Check which are touching air;
	chunk->amount_to_render = get_blocks_to_render(chunk);

	float	tmp[VEC3_SIZE];
	float	model[MAT4_SIZE];
	float	scale[MAT4_SIZE];
//	float	rot[MAT4_SIZE];
	float	trans[MAT4_SIZE];

	for (int i = 0; i < chunk->block_amount; i++)
	{
		mat4_identity(trans);
		mat4_translate(trans, trans, vec3_new(tmp,
			(chunk->blocks[i].pos[0] * chunk->info->block_size) + chunk->world_coordinate[0],
			(chunk->blocks[i].pos[1] * chunk->info->block_size) + chunk->world_coordinate[1],
			(chunk->blocks[i].pos[2] * chunk->info->block_size) + chunk->world_coordinate[2]));

		mat4_identity(scale);
		mat4_scale(scale, scale, vec3_new(tmp,
			chunk->info->block_scale,
			chunk->info->block_scale,
			chunk->info->block_scale));

		mat4_identity(model);
		mat4_multiply(model, scale, model);
		mat4_multiply(model, trans, model);

		memcpy(chunk->block_matrices + (i * 16), model, sizeof(float) * 16);
		memcpy(chunk->block_textures + (i), &chunk->blocks[i].texture_id, sizeof(int));
	}
}

void	*update_chunk_threaded(void *arg)
{
	t_chunk_args	*info = arg;

	update_chunk(info->chunk, info->coords);
	return (NULL);
}

void	regenerate_chunks(int *res, t_chunk *chunks, t_chunk_info *info, float *player_chunk_v2)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	reload_amount = 0;
	int	found;
	int	start_coord[2];

	start_coord[0] = player_chunk_v2[0] - (info->render_distance / 2);
	start_coord[1] = player_chunk_v2[1] - (info->render_distance / 2);
	// Check which chunks are not going to be in the next iteration of
	//	loaded chunks, save those to 'reload_these_chunks' and when starting
	// to update the new chunks that are going to be loaded, and put the
	// new chunk info into those 'chunks' indices;
	// Takes 0.000000 seconds
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
		{
			for (int z = start_coord[1], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
			{
				if (chunks[i].coordinate[0] == x && chunks[i].coordinate[2] == z)
					found = 1;
			}
		}
		if (!found)
		{
			reload_these_chunks[reload_amount] = i;
			reload_amount++;
		}
	}

	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	pthread_t		threads[4];
	t_chunk_args	args[4];
	int				nth_thread = 0;

	int	nth_chunk = 0;
	for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
	{
		for (int z = start_coord[1], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
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
				if (nth_thread >= 4)
				{
					//break ;
					int i = 0;
					while (i < nth_thread)
					{
						if (pthread_join(threads[i], NULL))
							LG_ERROR("Couldnt join thread. -1-");
						i++;
					}
					nth_thread = 0;
				}
				args[nth_thread].chunk = &chunks[reload_these_chunks[nth_chunk]];
				vec3_new(args[nth_thread].coords, x, 1, z);
				if (pthread_create(&threads[nth_thread], NULL, update_chunk_threaded, &args[nth_thread]))
					LG_ERROR("Couldnt create thread.");
				nth_thread++;
				nth_chunk++;
			}
		}
	}
	int j = 0;
	while (j < nth_thread)
	{
		if (pthread_join(threads[j], NULL))
			LG_ERROR("Couldnt join thread. -2-");
		j++;
	}


	// Send all the updated chunk info to the gpu;
	// This needs to be done on the main thread (or the thread that the context is made on)
	for (int i = 0; i < reload_amount; i++)
	{
		// Matrices
		glBindBuffer(GL_ARRAY_BUFFER, chunks[reload_these_chunks[i]].vbo_matrices);
		glBufferData(GL_ARRAY_BUFFER, chunks[reload_these_chunks[i]].block_matrices_size,
			&chunks[reload_these_chunks[i]].block_matrices[0], GL_STATIC_DRAW);

		// Texture ID
		glBindBuffer(GL_ARRAY_BUFFER, chunks[reload_these_chunks[i]].vbo_texture_ids);
		glBufferData(GL_ARRAY_BUFFER, chunks[reload_these_chunks[i]].block_textures_size,
			&chunks[reload_these_chunks[i]].block_textures[0], GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	res[0] = reload_amount;
	res[1] = nth_chunk;
	/*
	*/
}

void	show_chunk_borders(t_chunk *chunk, t_camera *camera)
{
	t_aabb *a;
	
	a = &chunk->aabb;
	render_3d_line(
		(float []){a->min[0], a->min[1], a->min[2]},
		(float []){a->min[0], a->max[1], a->min[2]},
		(float []){1, 0, 0},
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		(float []){1, 0, 0},
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		(float []){1, 0, 0},
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		(float []){1, 0, 0},
		camera->view, camera->projection);
}