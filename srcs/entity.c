#include "shaderpixel.h"

void	new_entity(t_entity *entity)
{
	new_vec3(entity->pos, 0.0f, 0.0f, 0.0f);
	entity->rot_x_angle = 0.0f;
	entity->rot_y_angle = 0.0f;
	entity->rot_z_angle = 0.0f;
	entity->scale_value = 1.0f;

	entity->collision_detection_enabled = 0;
	entity->collision_use_precise = 0;
	entity->collision = 0;

	update_entity(entity);
	LG_INFO("new entity made.");
}

void	entity_print(t_entity *entity)
{
	ft_printf("Entity :\n");
	vec3_string("entity.pos : ", entity->pos);
	ft_printf("entity.rot_x_angle : %f\n", entity->rot_x_angle);
	ft_printf("entity.rot_y_angle : %f\n", entity->rot_y_angle);
	ft_printf("entity.rot_z_angle : %f\n", entity->rot_z_angle);

	mat4_string("scale_mat :", entity->scale_mat);
	mat4_string("rot_mat :", entity->rot_mat);
	mat4_string("trans_mat :", entity->trans_mat);
	mat4_string("model_mat :", entity->model_mat);

	float	tmp[MAT4_SIZE];
	mat4_identity(tmp);
	mat4_string("step1 :", tmp);

	mat4_multiply(tmp, tmp, entity->scale_mat);
	mat4_string("step2 :", tmp);

	float	m0[MAT4_SIZE];
	float	m1[MAT4_SIZE];
	mat4_assign(m0, tmp);
	mat4_assign(m1, entity->rot_mat);

	mat4_multiply(tmp, tmp, entity->rot_mat);
	mat4_string("step3 :", tmp);

	mat4_multiply(tmp, tmp, entity->trans_mat);
	mat4_string("final :", tmp);

	float	mult[MAT4_SIZE];

	mult[12] = m0[0] * m1[12] + m0[4] * m1[13] + m0[8] * m1[14] + m0[12] * m1[15];
	ft_printf("mult[12] : (%f) + (%f) + (%f) + (%f)\n",
		m0[0] * m1[0],
		m0[4] * m1[1],
		m0[8] * m1[2],
		m0[12] * m1[15]
		);
	ft_printf("m0[10] : %f, m1[2] : %f\n", m0[10], m1[2]);
	ft_printf("mult[12] : %f\n", mult[12]);
}

/*
 * Re-calculates the matrices (rot & trans & model);
 */
void	update_entity(t_entity *entity)
{
	float	temp[VEC3_SIZE];

	mat4_identity(entity->scale_mat);
	mat4_scale(entity->scale_mat, entity->scale_mat,
		new_vec3(temp, entity->scale_value, entity->scale_value, entity->scale_value));


	mat4_identity(entity->rot_mat);

	float	rot_x[MAT4_SIZE];
	mat4_identity(rot_x);
	mat4_rotation_x(rot_x, to_radians(entity->rot_x_angle));

	float	rot_y[MAT4_SIZE];
	mat4_identity(rot_y);
	mat4_rotation_y(rot_y, to_radians(entity->rot_y_angle));

	float	rot_z[MAT4_SIZE];
	mat4_identity(rot_z);
	mat4_rotation_z(rot_z, to_radians(entity->rot_z_angle));

	mat4_multiply(entity->rot_mat, entity->rot_mat, rot_x);
	mat4_multiply(entity->rot_mat, entity->rot_mat, rot_y);
	mat4_multiply(entity->rot_mat, entity->rot_mat, rot_z);

/*
	float	q[QUAT_SIZE];
	euler_to_quat(q,
		to_radians(entity->rot_x_angle),
		to_radians(entity->rot_y_angle),
		to_radians(entity->rot_z_angle));
	mat4_rotation_quat(entity->rot_mat, q);
	*/

	mat4_identity(entity->trans_mat);
	mat4_translate(entity->trans_mat, entity->trans_mat, entity->pos);


	mat4_identity(entity->model_mat);
	mat4_multiply(entity->model_mat, entity->model_mat, entity->scale_mat);
	mat4_multiply(entity->model_mat, entity->model_mat, entity->rot_mat);
	mat4_multiply(entity->model_mat, entity->model_mat, entity->trans_mat);

	// AABB
	if (entity->collision_detection_enabled)
	{
		create_bb_vertices(entity->bb_vertices, entity->aabb.min, entity->aabb.max);
		create_bb_indices(entity->bb_indices);
	}
}

void	render_entity(t_entity *entity, t_camera *camera, t_model *model, t_shader *shader)
{
	int error = glGetError();

	update_entity(entity);

	glUseProgram(shader->program);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "model"), 1, GL_FALSE, &entity->model_mat[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, &camera->projection[0]);
	glUniform3fv(glGetUniformLocation(shader->program, "aViewPos"), 3, &camera->pos[0]);

//	if (vec3_distance(camera->pos, entity->pos) <= 5.0f)
	if (entity->collision)
		glUniform1i(glGetUniformLocation(shader->program, "useColor"), 1);
	else
		glUniform1i(glGetUniformLocation(shader->program, "useColor"), 0);

	render_model(model);

	if (entity->collision_detection_enabled)
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
			//ftl, ftr, fbl,
			//ftr, fbr, fbl
			0, 1, 2,
			1, 3, 2,
		// right
			//ftr, btr, fbr,
			//btr, bbr, fbr,
			1, 5, 3,
			5, 7, 3,
		// back
			//btl, btr, bbl,
			//btr, bbr, bbl,
			4, 5, 6,
			5, 7, 6,
		// left
			//btl, ftl, bbl,
			//ftl, fbl, bbl,
			4, 0, 6,
			0, 2, 6,
		// top
			//btl, btr, ftl,
			//btr, ftr, ftl,
			4, 5, 0,
			5, 1, 0,
		// bot
			//bbl, bbr, fbl,
			//bbr, fbr, fbl
			6, 7, 2,
			7, 3, 2
	};
	memcpy(res, indices, sizeof(unsigned int) * 36);
	return (res);
}

float	*create_bb_normals(float *res)
{

}

typedef struct	s_render_info
{
	t_shader	shader;
	GLuint		vao;
	GLuint		vbo;
	GLuint		ebo;
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

	glUseProgram(info.shader.program);

	glBindVertexArray(info.vao);

	glBindBuffer(GL_ARRAY_BUFFER, info.vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, &vertices[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info.ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, &indices[0], GL_DYNAMIC_DRAW);

	glUniform3fv(glGetUniformLocation(info.shader.program, "inColor"), 1, col);
	glUniformMatrix4fv(glGetUniformLocation(info.shader.program, "view"), 1, GL_FALSE, &view[0]);
	glUniformMatrix4fv(glGetUniformLocation(info.shader.program, "projection"), 1, GL_FALSE, &proj[0]);

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
