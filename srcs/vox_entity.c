/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/18 11:58:23 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 12:16:23 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "entity.h"
#include "bmath.h"
#include "vox.h"
#include "chunk.h"

void	entity_update(t_entity *entity)
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

	scale_matrix(entity->scale_m4, entity->scale);
	rotation_matrix(entity->rot_m4, entity->rot);
	translation_matrix(entity->trans_m4, entity->pos);
}

int	vox_entity_state_idle(t_entity *entity)
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

int	vox_entity_state_wander(t_entity *entity, t_fps *fps)
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

void	entity_event(t_entity *entity, t_world *info, t_fps *fps)
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
	t_block			*block = NULL;
	t_block_data	data;
	if (chunk)
	{
		// Velocity
		float	forward[3];

		v3_add(forward, entity->pos, entity->velocity);
		forward[1] += BLOCK_SCALE;
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
						entity->velocity[1] += BLOCK_SCALE;
					}
				}
			}
		}

		// Gravity
		// If no block one block down, move the entity downward
		float	gravity = (9.8f * g_entity_data[entity->type].fall_speed) * fps->delta_time;
		float	downward[3];
		v3_new(downward, entity->pos[0], ceil(entity->pos[1] - BLOCK_SCALE - gravity), entity->pos[2]);
		block = get_block(info, downward);
		if (block)
		{
			data = get_block_data(block);
			if (data.entity_collision)
				entity->pos[1] = downward[1] + BLOCK_SCALE;
			else
				entity->velocity[1] -= gravity;
		}
	}

	// if any of the functions changed the entity, we need to update it;
	entity->needs_update = result;
}

//////////////////

void	entity_manager_update_entity_palette(t_entity_manager *manager)
{
	memset(manager->entity_palette, 0, sizeof(int) * ENTITY_AMOUNT);
	for (int i = 0; i < manager->entity_amount; i++)
		if (manager->slot_taken[i])
			++manager->entity_palette[(int)manager->entities[i].type];
}