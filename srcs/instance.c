#include "shaderpixel.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/

void	new_chunk(t_chunk *chunk, t_chunk_info *info, float *coord)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size, 1,
		chunk->coordinate[2] * chunk->info->chunk_size);
	
	int	max_blocks = chunk->info->width * chunk->info->breadth * 256;
	chunk->blocks = malloc(sizeof(t_block) * (max_blocks));
	chunk->block_amount = chunk_gen(chunk);
	
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->block_amount;
	chunk->block_matrices = malloc(sizeof(float) * 16 * max_blocks);

	chunk->block_textures_size = sizeof(int) * chunk->block_amount;
	chunk->block_textures = malloc(sizeof(int) * max_blocks);

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

	// Matrices
	glGenBuffers(1, &chunk->vbo_matrices);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_matrices);
	glBufferData(GL_ARRAY_BUFFER, chunk->block_matrices_size,
		&chunk->block_matrices[0], GL_STATIC_DRAW);

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
	glBufferData(GL_ARRAY_BUFFER, chunk->block_textures_size,
		&chunk->block_textures[0], GL_STATIC_DRAW);

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
 * When ever we are generating new chunks, call this on the chunks we 'stopped' rendering;
 * Basically just use the same memory spot and update the data with the correct new one;
*/
void	update_chunk(t_chunk *chunk, float *coord)
{
	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size, 1,
		chunk->coordinate[2] * chunk->info->chunk_size);
	
	chunk->block_amount = chunk_gen(chunk);
	
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->block_amount;

	chunk->block_textures_size = sizeof(int) * chunk->block_amount;

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
	// Matrices
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_matrices);
	glBufferData(GL_ARRAY_BUFFER, chunk->block_matrices_size,
		&chunk->block_matrices[0], GL_STATIC_DRAW);

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_texture_ids);
	glBufferData(GL_ARRAY_BUFFER, chunk->block_textures_size,
		&chunk->block_textures[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
 * Returns amount of blocks generated;
*/
int	chunk_gen(t_chunk *chunk)
{
	int		start_y = 65;
	int		max_y = 256;
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
			for (int y = start_y + perper, b = 0; b < /*start_y + perper*/ 5; y--, b++) // the 'b' is the amount of blocks we have on the y axis;
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
				if (rep > -0.03f)
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
	res[0] = player_coord[0] / info->chunk_size;
	res[1] = player_coord[2] / info->chunk_size;
	return (res);
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

void	regenerate_chunks(t_chunk *chunks, t_chunk_info *info, float *player_chunk_v2)
{
	int	reload_these_chunks[50];
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
	//ft_printf("Chunk unload checker time : %f\n", ft_timer_end());
	
	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	// Takes 0.018 - 0.009 seconds with current 'chunk_gen' (b < start_y + actual)
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
				update_chunk(&chunks[reload_these_chunks[nth_chunk]], (float []){x, 1, z});
				nth_chunk++;
			}
		}
	}
}