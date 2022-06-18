/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   entity.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/17 10:42:27 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/18 10:37:15 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include "shader.h"

void	new_entity(t_entity *entity)
{
	v3_new(entity->pos, 0.0f, 0.0f, 0.0f);
	entity->rot_x_angle = 0.0f;
	entity->rot_y_angle = 0.0f;
	entity->rot_z_angle = 0.0f;
	entity->scale_value = 1.0f;

	entity->collision_detection_enabled = 0;
	entity->collision_use_precise = 0;
	entity->collision = 0;
	entity->render_aabb = 1;

	entity->show_normal_map = 0;

	update_entity(entity);
	LG_INFO("new entity made.");
}

void	entity_print(t_entity *entity)
{
	ft_printf("Entity :\n");
	v3_string("entity.pos : ", entity->pos);
	ft_printf("entity.rot_x_angle : %f\n", entity->rot_x_angle);
	ft_printf("entity.rot_y_angle : %f\n", entity->rot_y_angle);
	ft_printf("entity.rot_z_angle : %f\n", entity->rot_z_angle);

	m4_string("scale_mat :", entity->scale_mat);
	m4_string("rot_mat :", entity->rot_mat);
	m4_string("trans_mat :", entity->trans_mat);
	m4_string("model_mat :", entity->model_mat);
}

void	new_model_matrix(float *m4_res, float scale, float *v3_rot, float *v3_pos)
{
	float	scale_mat[16];
	float	rot_mat[16];
	float	trans_mat[16];

	scale_matrix(scale_mat, scale);
	rotation_matrix(rot_mat, v3_rot);
	translation_matrix(trans_mat, v3_pos);
	model_matrix(m4_res, scale_mat, rot_mat, trans_mat);
}

/*
 * Creates a model matrix from scale-, rotation-, and transformation matrix;
*/
void	model_matrix(float *m4_res, float *m4_scale, float *m4_rot, float *m4_trans)
{
	m4_identity(m4_res);
	m4_multiply(m4_res, m4_scale, m4_res);
	m4_multiply(m4_res, m4_rot, m4_res);
	m4_multiply(m4_res, m4_trans, m4_res);
}

void	scale_matrix(float *m4_res, float scale)
{
	float	tmp[3];

	m4_identity(m4_res);
	m4_scale(m4_res, m4_res, v3_new(tmp, scale, scale, scale));
}

void	rotation_matrix(float *m4_res, float *v3_rot)
{
	float	tmp_mat[16];
	float	tmp[3];

	m4_identity(m4_res);
	// x
	m4_identity(tmp_mat);
	m4_rotation_x(tmp_mat, to_radians(v3_rot[0]));
	m4_multiply(m4_res, tmp_mat, m4_res);
	// y
	m4_identity(tmp_mat);
	m4_rotation_y(tmp_mat, to_radians(v3_rot[1]));
	m4_multiply(m4_res, tmp_mat, m4_res);
	// z
	m4_identity(tmp_mat);
	m4_rotation_z(tmp_mat, to_radians(v3_rot[2]));
	m4_multiply(m4_res, tmp_mat, m4_res);
}

void	translation_matrix(float *m4_res, float *v3_pos)
{
	// Trans
	m4_identity(m4_res);
	m4_translate(m4_res, m4_res, v3_pos);
}

/*
 * Re-calculates the matrices (rot & trans & model);
 */
void	update_entity(t_entity *entity)
{
	float	temp[V3_SIZE];

	m4_identity(entity->scale_mat);
	m4_scale(entity->scale_mat, entity->scale_mat,
		v3_new(temp, entity->scale_value, entity->scale_value, entity->scale_value));


	m4_identity(entity->rot_mat);

	float	rot_x[M4_SIZE];
	m4_identity(rot_x);
	m4_rotation_x(rot_x, to_radians(entity->rot_x_angle));

	float	rot_y[M4_SIZE];
	m4_identity(rot_y);
	m4_rotation_y(rot_y, to_radians(entity->rot_y_angle));

	float	rot_z[M4_SIZE];
	m4_identity(rot_z);
	m4_rotation_z(rot_z, to_radians(entity->rot_z_angle));

	m4_multiply(entity->rot_mat, rot_x, entity->rot_mat);
	m4_multiply(entity->rot_mat, rot_y, entity->rot_mat);
	m4_multiply(entity->rot_mat, rot_z, entity->rot_mat);

/*
	float	q[QUAT_SIZE];
	euler_to_quat(q,
		to_radians(entity->rot_x_angle),
		to_radians(entity->rot_y_angle),
		to_radians(entity->rot_z_angle));
	m4_rotation_quat(entity->rot_mat, q);
	*/

	m4_identity(entity->trans_mat);
	m4_translate(entity->trans_mat, entity->trans_mat, entity->pos);


	m4_identity(entity->model_mat);
	m4_multiply(entity->model_mat, entity->scale_mat, entity->model_mat);
	m4_multiply(entity->model_mat, entity->rot_mat, entity->model_mat);
	m4_multiply(entity->model_mat, entity->trans_mat, entity->model_mat);

	// AABB
	if (entity->collision_detection_enabled)
	{
		create_bb_vertices(entity->bb_vertices, entity->aabb.min, entity->aabb.max);
		create_bb_indices(entity->bb_indices);
	}
}

void	render_entity(t_entity *entity, t_camera *camera, t_model *model, GLuint shader)
{
	int error = glGetError();

	update_entity(entity);

	glUseProgram(shader);
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, &entity->model_mat[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, &camera->projection[0]);
	glUniform3fv(glGetUniformLocation(shader, "aViewPos"), 3, &camera->pos[0]);

//	if (vec3_distance(camera->pos, entity->pos) <= 5.0f)
	if (entity->collision)
		glUniform1i(glGetUniformLocation(shader, "useColor"), 1);
	else
		glUniform1i(glGetUniformLocation(shader, "useColor"), 0);
	
	if (entity->show_normal_map)
		glUniform1i(glGetUniformLocation(shader, "show_normal_map"), 1);
	else
		glUniform1i(glGetUniformLocation(shader, "show_normal_map"), 0);

	render_model(model);

	if (entity->collision_detection_enabled && entity->render_aabb)
		render_box(entity->bb_vertices, entity->bb_indices, (float[]){1, 0, 0},
			camera->view, camera->projection);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

// TODO: When not using anymore, REMOVE!
float	*create_bb_vertices(float *res, float *min, float *max)
{
	float	vertices[] = {
		// front
		min[0], max[1], max[2], // top left		0
		max[0], max[1], max[2], // top right	1
		min[0], min[1], max[2], // bot left		2
		max[0], min[1], max[2], // bot right	3

		// back
		min[0], max[1], min[2], // top left		4
		max[0], max[1], min[2], // top right	5
		min[0], min[1], min[2], // bot left		6
		max[0], min[1], min[2] // bot right		7
	};
	memcpy(res, vertices, sizeof(float) * 24);
	return (res);
}

unsigned int	*create_bb_indices(unsigned int *res)
{
	unsigned int	indices[] = {
		// front
			0, 2, 3,
			0, 3, 1,
		// right
			1, 3, 7,
			1, 7, 5,
		// back
			5, 7, 6,
			5, 6, 4,
		// left
			4, 6, 2,
			4, 2, 0,
		// top
			4, 0, 1,
			4, 1, 5,
		// bot
			6, 7, 3,
			6, 3, 2
	};
	memcpy(res, indices, sizeof(unsigned int) * 36);
	return (res);
}

float	*create_bb_normals(float *res)
{

}

typedef struct	s_render_info
{
	GLuint	shader;
	GLuint	vao;
	GLuint	vbo;
	GLuint	ebo;
}	t_render_info;

int	setup_render_box(t_render_info *info)
{
	new_shader(&info->shader, ROOT_PATH"shaders/box.vs", ROOT_PATH"shaders/box.fs");

	glGenVertexArrays(1, &info->vao);
	glBindVertexArray(info->vao);

	glEnableVertexAttribArray(0);

	glGenBuffers(1, &info->vbo);
	glGenBuffers(1, &info->ebo);

	return (1);
}

void	render_box(float *vertices, unsigned int *indices, float *col, float *view, float *proj)
{
	static t_render_info	info = {};
	static int				set = 0;
	int	error = 0;

	if (!set)
		set = setup_render_box(&info);

	glUseProgram(info.shader);

	glBindVertexArray(info.vao);

	glBindBuffer(GL_ARRAY_BUFFER, info.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, &indices[0], GL_DYNAMIC_DRAW);

	glUniform3fv(glGetUniformLocation(info.shader, "inColor"), 1, col);
	glUniformMatrix4fv(glGetUniformLocation(info.shader, "view"), 1, GL_FALSE, &view[0]);
	glUniformMatrix4fv(glGetUniformLocation(info.shader, "projection"), 1, GL_FALSE, &proj[0]);

	GLint	prev_pol_mode;
	glGetIntegerv(GL_POLYGON_MODE, &prev_pol_mode);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glPolygonMode(GL_FRONT_AND_BACK, prev_pol_mode);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}
