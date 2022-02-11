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
}

void	render_entity(t_entity *entity, t_model *model, t_shader *shader)
{
	(void)entity;
	(void)model;
	(void)shader;
}