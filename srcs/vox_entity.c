/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/18 11:58:23 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 13:47:09 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vox_entity.h"
#include "bmath.h"
#include "shaderpixel.h"

void	vox_entity_new(t_vox_entity *entity)
{
	v3_new(entity->pos, 0, 0, 0);
	v3_new(entity->rot, 0, 0, 0);
	v3_new(entity->velocity, 0, 0, 0);
	entity->scale = 1.0f;
	entity->type = 0;
	entity->state = ENTITY_STATE_IDLE;
	entity->health = 100;
	entity->speed = 1;

	entity->yaw = 0;
	entity->pitch = 0;
	entity->roll = 0;

	// Debug toggles
	entity->draw_dir = 0;
	entity->draw_terrain_collision = 0;
	entity->draw_aabb = 0;

	entity->ai = 1;
}

/*
 * 
*/
void	vox_entity_update(t_vox_entity *entity)
{
	float	rad_yaw;
	float	rad_pitch;

	rad_yaw = to_radians(entity->yaw);
	rad_pitch = to_radians(entity->pitch);
	v3_new(entity->front, cos(rad_yaw) * cos(rad_pitch),
		sin(rad_pitch), sin(rad_yaw) * cos(rad_pitch));
	v3_normalize(entity->front, entity->front);

	v3_add(entity->pos, entity->pos, entity->velocity);
	v3_new(entity->velocity, 0, 0, 0);

	entity->rot[0] = fmod(entity->pitch, 360);
	entity->rot[1] = fmod(-entity->yaw, 360);
	entity->rot[2] = fmod(entity->roll, 360);

	scale_matrix(entity->scale_m4, entity->scale);
	rotation_matrix(entity->rot_m4, entity->rot);
	translation_matrix(entity->trans_m4, entity->pos);
}

void	set_entity_at_world_pos(t_world *info, float *world_pos, int entity_type)
{
	t_vox_entity	*entity;

	if (info->entity_amount >= MAX_ENTITIES)
	{
		LG_WARN("Max entities (%d) reached.", MAX_ENTITIES);
		return ;
	}
	entity = &info->entities[info->entity_amount];
	vox_entity_new(entity);
	entity->type = entity_type;
	v3_assign(entity->pos, world_pos);

	// TODO : entity->needs_update = 1;
	// instead of this;
	vox_entity_update(entity);

	++info->entity_amount;
	LG_INFO("Entity (#%d) added at %f %f %f", info->entity_amount, entity->pos[0], entity->pos[1], entity->pos[2]);
}

int	vox_entity_state_idle(t_vox_entity *entity)
{
	float	rot[3];
	int		rot_amount;
	int		rot_chance;

	rot_chance = rand() % 100;
	rot_amount = rand() % 45;
	if (rot_chance == 1)
	{
		entity->yaw += rot_amount;
		return (1);
	}
	else if (rot_chance == 2)
	{
		entity->yaw -= rot_amount;
		return (1);
	}
	return (0);
}

int	vox_entity_state_wander(t_vox_entity *entity, t_fps *fps)
{
	// While wandering the entity can change direction it walking in;
	vox_entity_state_idle(entity);

	float	rad_yaw;
	float	rad_pitch;
	rad_yaw = to_radians(entity->yaw);
	rad_pitch = to_radians(entity->pitch);
	v3_new(entity->front, cos(rad_yaw) * cos(rad_pitch),
		sin(rad_pitch), sin(rad_yaw) * cos(rad_pitch));
	v3_normalize(entity->front, entity->front);

	v3_multiply_f(entity->velocity, entity->front, entity->speed * fps->delta_time);
	/*
	ft_printf("speed * delta == %d * %f = %f\n", entity->speed, fps->delta_time, entity->speed * fps->delta_time);
	v3_string("rot : ", entity->rot);
	v3_string("rot_norm : ", rot_norm);
	v3_string("velocity : ", entity->velocity);
	*/
	return (1);
}

void	vox_entity_event(t_vox_entity *entity, t_world *info, t_fps *fps)
{
	int	result;
	int	change_state;
	int	change_state_chance;

	result = 0;
	change_state_chance = 1;
	change_state = rand() % 100 > change_state_chance;
	if (change_state)
	{
		if (entity->state == ENTITY_STATE_IDLE)
			entity->state = ENTITY_STATE_WANDER;
		else if (entity->state == ENTITY_STATE_WANDER)
			entity->state = ENTITY_STATE_IDLE;
	}	
	if (entity->state == ENTITY_STATE_IDLE)
		result = vox_entity_state_idle(entity);
	else if (entity->state == ENTITY_STATE_WANDER)
		result = vox_entity_state_wander(entity, fps);
	
	// Collision
	// Check if the entity is inside a loaded chunk;
	int	chunk_pos[3];
	get_chunk_pos_from_world_pos(chunk_pos, entity->pos);
	t_chunk *chunk = get_chunk(info, chunk_pos);
	t_block *block = NULL;
	t_block_data	data;
	if (chunk)
	{
		// Velocity
		float	forward[3];

		v3_add(forward, entity->pos, entity->velocity);
		/*
		forward[0] = floor(forward[0]);
		forward[1] = floor(forward[1]);
		forward[2] = floor(forward[2]);
		*/
		block = get_block(info, forward);
		if (block)
		{
			data = get_block_data(block);
			if (data.entity_collision)
			{
				// Forward up
				v3_add(forward, forward, (float *)g_card_dir[DIR_UP]);
				block = get_block(info, forward);
				if (block)
				{
					data = get_block_data(block);
					if (data.entity_collision)
					{
						if (entity->draw_terrain_collision)
							render_block_outline(forward, (float []){0, 255, 0}, info->player->camera.view, info->player->camera.projection);
						v3_new(entity->velocity, 0, entity->velocity[1], 0);
					}
					else
					{
						entity->velocity[1] += 1.0f;
					}
				}
			}
		}

		// Gravity
		// If no block one block down, move the entity downward
		// TODO : the '- 1.41f',  is basically the height from the pos to the entity feet,
		//		in the future take this from the difference from pos and entity model aabb min;
		float	gravity = (9.8f * fps->delta_time);
		float	downward[3];
		v3_new(downward, entity->pos[0], ceil((entity->pos[1] - 1.41f) - gravity), entity->pos[2]);
		block = get_block(info, downward);
		if (block)
		{
			data = get_block_data(block);
			if (data.entity_collision)
				entity->pos[1] = downward[1] + 1.4f;
			else
				entity->velocity[1] -= gravity;
		}
	}

	// if any of the functions changed the entity, we need to update it;
	entity->needs_update = result;
}