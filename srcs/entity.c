#include "shaderpixel.h"

void	new_entity(t_entity *entity)
{
	new_vec3(entity->pos, 0.0f, 0.0f, 0.0f);
	new_vec3(entity->rot, 1.0f, 1.0f, 1.0f);
	new_vec3(entity->scale, 1.0f, 1.0f, 1.0f);
	entity->rot_x_angle = 0.0f;
	entity->rot_y_angle = 0.0f;
	entity->rot_z_angle = 0.0f;
	entity->scale_value = 1.0f;

	LG_INFO("new entity made.");
}

/*
 * Re-calculates the matrices (rot & trans & model);
 */
void	update_entity(t_entity *entity)
{
	float	temp[VEC3_SIZE];

	mat4_identity(entity->scale);
	mat4_scale(entity->scale, entity->scale,
		new_vec3(temp, entity->scale_value, entity->scale_value, entity->scale_value));
	

	mat4_identity(entity->rot);

	float	rot_x[MAT4_SIZE];
	mat4_identity(rot_x);
	mat4_rotation_x(rot_x, to_radians(entity->rot_x_angle));

	float	rot_y[MAT4_SIZE];
	mat4_identity(rot_y);
	mat4_rotation_x(rot_y, to_radians(entity->rot_y_angle));

	float	rot_z[MAT4_SIZE];
	mat4_identity(rot_z);
	mat4_rotation_x(rot_z, to_radians(entity->rot_z_angle));

	mat4_multiply(entity->rot, entity->rot, rot_x);
	mat4_multiply(entity->rot, entity->rot, rot_y);
	mat4_multiply(entity->rot, entity->rot, rot_z);


	mat4_identity(entity->trans);
	mat4_translate(entity->trans, entity->trans, entity->pos);


	mat4_identity(entity->model);
	mat4_multiply(entity->model, entity->model, entity->scale);
	mat4_multiply(entity->model, entity->model, entity->rot);
	mat4_multiply(entity->model, entity->model, entity->trans);
}

void	render_entity(t_entity *entity, t_camera *camera, t_model *model, t_shader *shader)
{
	update_entity(entity);

	glUseProgram(shader->program);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "model"), 1, GL_FALSE, &entity->model[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, &camera->projection[0]);
	glUniform3fv(glGetUniformLocation(shader->program, "aViewPos"), 3, &camera->pos[0]);

	render_model(model);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}