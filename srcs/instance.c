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
	chunk->block_amount = chunk_gen(chunk, 25);
	
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
	
	chunk->block_amount = chunk_gen(chunk, 25);
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->block_amount;

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

	// Update matrix buffer
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_matrices);
	glBufferData(GL_ARRAY_BUFFER, chunk->block_matrices_size,
		&chunk->block_matrices[0], GL_STATIC_DRAW);

	// Update texture id buffer
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_texture_ids);
	glBufferData(GL_ARRAY_BUFFER, chunk->block_textures_size,
		&chunk->block_textures[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
 * Returns amount of blocks generated;
*/
int	chunk_gen(t_chunk *chunk, float freq)
{
	int	start_y = 16;
	int	max_y = 256;
	int i = 0;

	for (int x = 0; x < chunk->info->width; x++)
	{
		for (int z = 0; z < chunk->info->breadth; z++)
		{
			float	block_world_x = ((chunk->coordinate[0] * chunk->info->chunk_size) +
				((float)x * chunk->info->block_size)) / freq;
			float	block_world_z = ((chunk->coordinate[2] * chunk->info->chunk_size) +
				((float)z * chunk->info->block_size)) / freq;

			float	perper = 1 * perlin(1 * block_world_x, 1 * block_world_z, chunk->info->seed);
			/*
					1 * perlin(1 * block_world_x, 1 * block_world_z) +
					0.5 * perlin(2 * block_world_x, 2 * block_world_z) +
					0.25 * perlin(4 * block_world_x, 4 * block_world_z) +
					0.125 * perlin(8 * block_world_x, 8 * block_world_z);
					*/
			float	rounded = perper;
			rounded = round(rounded * 32) / 32;
			if (rounded < 0)
				rounded = -powf(fabs(rounded), 1.5f);
			else
				rounded = powf(fabs(rounded), 1.5f);
			float	actual = start_y * rounded;
			/*
			ft_printf("actual : %f\n", actual);
			ft_printf("powf(%f, 1.20) = %f\n", perper, rounded);
			ft_printf("%d + (%d * %f) = %f\n", start_y, start_y, rounded, actual);
			*/
			for (int y = actual, b = 0; b < start_y + actual; y--, b++) // the 'b' is the amount of blocks we have on the y axis;
			{
				vec3_new(chunk->blocks[i].pos, x, y, z);
				if (b > 2) // if we have 3 dirt block on top we make the rest stone blocks;
					chunk->blocks[i].texture_id = 1;
				else
					chunk->blocks[i].texture_id = 0;
				i++;
			}
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

int	dist_2d(int *p1, int *p2)
{
	int	x = p2[0] - p1[0];
	int	y = p2[1] - p1[1];
	return (sqrt((x * x) + (y * y)));
}

void	int_sort(int *arr, size_t size)
{
	for (int i = 0; i < size - 1; i++)
	{
		if (arr[i + 0] > arr[i + 1])
		{
			ft_swap(&arr[i + 0], &arr[i + 1]);
			i = -1;
		}
	}
}

int	furthest_away_chunks(int *res, float *player_chunk, t_chunk *chunks, int chunk_amount)
{
	int	amount_to_find = sqrt(chunk_amount) * 2 - 1;
	int	distances[chunk_amount];
	int	found = 0;

	for (int i = 0; i < chunk_amount; i++)
		distances[i] = dist_2d((int []){player_chunk[0], player_chunk[1]},
				(int []){chunks[i].coordinate[0], chunks[i].coordinate[2]});
	int_sort(distances, chunk_amount);
	for (int i = 0; i < chunk_amount; i++)
	{
		for (int j = chunk_amount - 1; j >= 0; j--)
		{
			if (dist_2d((int []){player_chunk[0], player_chunk[1]},
				(int []){chunks[i].coordinate[0], chunks[i].coordinate[2]}) ==
				distances[j])
			{
				res[found] = i;
				found++;
				break ;
			}
		}
		if (found == amount_to_find)
			break ;
	}
	return (found);
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