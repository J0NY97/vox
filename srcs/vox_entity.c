/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/18 11:58:23 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/18 12:17:14 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vox_entity.h"
#include "bmath.h"
#include "shaderpixel.h"

void	vox_entity_new(t_vox_entity *entity)
{
	v3_new(entity->pos, 0, 0, 0);
	v3_new(entity->rot, 90, 0, 0);
	entity->scale = 1.0f;
	entity->type = 0;
}

/*
 * 
*/
void	vox_entity_update(t_vox_entity *entity)
{
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