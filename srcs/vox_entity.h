/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/22 10:48:35 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VOX_ENTITY_H
# define VOX_ENTITY_H

# include "stdlib.h"
# include "glad.h"
# include "glfw3.h"
# include "enum.h"
# include "model.h"
# include "chunk.h"
# include "fps.h"

# define MAX_ENTITIES 1024

enum e_entity_types
{
	ENTITY_MELON_GOLEM,
	ENTITY_CHICKEN,
	ENTITY_AMOUNT
};

enum e_entity_states
{
	ENTITY_STATE_IDLE = 0,
	ENTITY_STATE_WANDER,
	ENTITY_STATE_FLEE,
	ENTITY_STATE_SEEK,
	ENTITY_STATE_ATTACK,
	ENTITY_STATE_AMOUNT
};

// entities should probably have 'persistent' flag, and wont despawn if it's set
// (torches / doors / beds / boats / farmed animals / dropped items);
	
typedef struct s_vox_entity_data
{
	char		*name;
	char		*readable_name;
	char		type;			// 'e_entity_types';
	char		*model_path;
	float		fall_speed;
}		t_vox_entity_data;

static const t_vox_entity_data	g_entity_data[] = {
	{
		"ENTITY_MELON_GOLEM",
		"Melon Golem",
		ENTITY_MELON_GOLEM,
		MODEL_PATH"melon_golem/melon_golem.obj",
		1.5	
	},
	{
		"ENTITY_CHICKEN",
		"Chicken",
		ENTITY_CHICKEN,
		MODEL_PATH"chicken/chiekn.obj",
		1
	}
};

typedef struct s_vox_entity
{
	int		health; // TODO
	int		speed;

	float	pos[3];	// model pos / tbh entity pos aswell
	float	rot[3];	// model rotation
	float	scale;	// model scale

	float	yaw;
	float	pitch;

	float	front[3];
	float	up[3];
	float	velocity[3];

	char	type;	// 'e_entity_types';
	char	state;	// 'e_entity_states';

	char	needs_update;

	char	ai;
	// Debug
	char	draw_dir;
	char	draw_terrain_collision;
	char	draw_aabb;

	// Matrices
	float	scale_m4[16];
	float	rot_m4[16];
	float	trans_m4[16];
}		t_vox_entity;

void	vox_entity_new(t_vox_entity *entity);
void	vox_entity_update(t_vox_entity *entity);

void	vox_entity_event(t_vox_entity *entity, t_world *info, t_fps *fps);

void	set_entity_at_world_pos(t_world *info, float *world_pos, int entity_type);

#endif