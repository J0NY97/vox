#include "shaderpixel.h"

void	new_chunk(t_chunk *chunk)
{
	memset(chunk, 0, sizeof(t_chunk));
	vec3_new(chunk->pos_offset, 0, 0, 0);
	chunk->block_amount = 9 * 9 * 1;
	chunk->blocks = malloc(sizeof(t_block) * chunk->block_amount);
	gen_chunk_blocks(chunk->blocks, (int []){9, 9, 1});
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

void	render_chunk(t_chunk *chunk, t_camera *camera, t_model *model, t_shader *shader)
{
	char	tmp_str[25];
	char	tmp_num[25];
	float	v3[VEC3_SIZE];
	float	model_matrix[MAT4_SIZE];
	float	scale_matrix[MAT4_SIZE];
	float	trans_matrix[MAT4_SIZE];

	mat4_identity(scale_matrix);
	mat4_scale(scale_matrix, scale_matrix, new_vec3(v3, 0.1, 0.1, 0.1));

	glUseProgram(shader->program);
	for (int i = 0; i < chunk->block_amount; i++)
	{
		mat4_identity(model_matrix);
		mat4_identity(trans_matrix);
		mat4_translate(trans_matrix, trans_matrix, chunk->blocks[i].pos);
		mat4_multiply(model_matrix, scale_matrix, model_matrix);
		mat4_multiply(model_matrix, trans_matrix, model_matrix);

		ft_strcpy(tmp_str, "model[");
		ft_b_itoa(i, tmp_num);
		ft_strcpy(tmp_str + 6, tmp_num);
		ft_strcpy(tmp_str + 6 + ft_strlen(tmp_num), "]");
		glUniformMatrix4fv(glGetUniformLocation(shader->program, tmp_str), 1, GL_FALSE, &model_matrix[0]);
	}
	glBindVertexArray(model->info[0].vao);

	if (model->info[0].elem_info[0].material && model->info[0].elem_info[0].material->loaded)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, model->info[0].elem_info[0].material->texture);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->info[0].elem_info[0].ebo);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, chunk->block_amount);

	glUseProgram(0);
	glBindVertexArray(0);
}