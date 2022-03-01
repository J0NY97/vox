#include "shaderpixel.h"

void	new_entity(t_entity *entity)
{
	new_vec3(entity->pos, 0.0f, 0.0f, 0.0f);
	entity->rot_x_angle = 0.0f;
	entity->rot_y_angle = 0.0f;
	entity->rot_z_angle = 0.0f;
	entity->scale_value = 1.0f;

	entity->collision = 0;

	entity->model = NULL;

	LG_INFO("new entity made.");
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
	mat4_rotation_x(rot_y, to_radians(entity->rot_y_angle));

	float	rot_z[MAT4_SIZE];
	mat4_identity(rot_z);
	mat4_rotation_x(rot_z, to_radians(entity->rot_z_angle));

	mat4_multiply(entity->rot_mat, entity->rot_mat, rot_x);
	mat4_multiply(entity->rot_mat, entity->rot_mat, rot_y);
	mat4_multiply(entity->rot_mat, entity->rot_mat, rot_z);


	mat4_identity(entity->trans_mat);
	mat4_translate(entity->trans_mat, entity->trans_mat, entity->pos);


	mat4_identity(entity->model_mat);
	mat4_multiply(entity->model_mat, entity->model_mat, entity->scale_mat);
	mat4_multiply(entity->model_mat, entity->model_mat, entity->rot_mat);
	mat4_multiply(entity->model_mat, entity->model_mat, entity->trans_mat);
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

	glUniform1i(glGetUniformLocation(shader->program, "useColor"), 1);
	render_box(entity->aabb.min, entity->aabb.max);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * 'entity_list' the list of entities we want the collision detection to be
 *	compared against the 'point';
 *
 * Improvement perhaps : split the aabb into elements instead of creating it
 * 	from the whole mesh, so that the detection is more precise... if needed;
 */
void	entity_collision_detection(t_list *entity_list, float *point)
{
	t_aabb		aabb;
	t_list		*curr;
	t_entity	*entity;

	curr = entity_list;
	while (curr)
	{
		entity = curr->content;
		aabb_create(&entity->aabb, entity->model->info->mesh.vertices,
			entity->model->info->mesh.vertex_amount);
		aabb = entity->aabb;
		aabb_transform(&aabb, entity->model_mat);
		entity->collision = point_aabb_collision(point, &aabb);
		curr = curr->next;
	}
}

// TODO: Move this to some place that makes more sense;
// Improvement?: Have the vbo, vao, ebo in a static;
/*
 * Pretty self-explanatory;
 *
 * Renders box immediately;
 * Creates temporary vao, vbo, ebo that gets deleted after used;
 *
 * NOTE: you have to glUseProgram before calling this function, with attribs:
 * 	0 = pos,
 * 	1 = col,
*/
void	render_box(float *min, float *max)
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

	float	colors[8 * 3];
	float	temp[3] = {255, 0, 0};
	memset_pattern(colors, 8 * 3 * sizeof(float), temp, sizeof(float) * 3);

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

	GLuint	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLuint	vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	GLuint	vbo_col;
	glGenBuffers(1, &vbo_col);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), &colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	GLuint	ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	GLint	prev_pol_mode;
	glGetIntegerv(GL_POLYGON_MODE, &prev_pol_mode);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, NULL);
	glPolygonMode(GL_FRONT_AND_BACK, prev_pol_mode);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

