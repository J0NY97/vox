#include "shaderpixel.h"

void	new_chunk(t_chunk *chunk, float *coord)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->block_scale = 0.1f;
	vec3_new(chunk->coordinate, coord[0], coord[1], coord[2]);
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * (9 * chunk->block_scale * 2),
		chunk->coordinate[1] * (9 * chunk->block_scale * 2),
		chunk->coordinate[2] * (9 * chunk->block_scale * 2));

	chunk->block_amount = 9 * 9 * 256; // try to have >= 9 * 9
	chunk->blocks = malloc(sizeof(t_block) * chunk->block_amount);
	int	x_amount = chunk->block_amount > 9 ? 9 : chunk->block_amount;
	int y_amount = chunk->block_amount / (9 * 9);
	int z_amount = chunk->block_amount > 9 ? 9 : 9 - (chunk->block_amount % 9);
	gen_chunk_blocks(chunk->blocks, chunk->block_scale,
		(int []){x_amount, y_amount, z_amount});
	
	chunk->block_matrices_size = sizeof(float) * 16 * chunk->block_amount;
	chunk->block_matrices = malloc(chunk->block_matrices_size);

	float	tmp[VEC3_SIZE];
	float	model[MAT4_SIZE];
	float	scale[MAT4_SIZE];
//	float	rot[MAT4_SIZE];
	float	trans[MAT4_SIZE];

	for (int i = 0; i < chunk->block_amount; i++)
	{
		mat4_identity(trans);
		mat4_translate(trans, trans, vec3_new(tmp,
			chunk->blocks[i].pos[0] + chunk->world_coordinate[0],
			chunk->blocks[i].pos[1] + chunk->world_coordinate[1],
			chunk->blocks[i].pos[2] + chunk->world_coordinate[2]));

		mat4_identity(scale);
		mat4_scale(scale, scale, vec3_new(tmp,
			chunk->block_scale, chunk->block_scale, chunk->block_scale));

		mat4_identity(model);
		mat4_multiply(model, scale, model);
		mat4_multiply(model, trans, model);

		memcpy(chunk->block_matrices + (i * 16), model, sizeof(float) * 16);

//		mat4_string("model :", chunk->block_matrices + (i * 16));
	}

	ft_printf("block_amount : %d, block_matrices_size : %d\n",
		chunk->block_amount, chunk->block_matrices_size);

	glGenBuffers(1, &chunk->vbo_instance);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo_instance);
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

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

}

void	gen_chunk_blocks(t_block *blocks, float scale, int *dim)
{
	int	i = 0;

	for (int y = 0; y < dim[1]; y++)
	{
		for (int x = 0; x < dim[0]; x++)
		{
			for (int z = 0; z < dim[2]; z++)
			{
				vec3_new(blocks[i].pos,
					x * scale + x * scale,
					y * scale + y * scale,
					z * scale + z * scale);
				i++;
			}
		}
	}
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