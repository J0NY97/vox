/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 11:34:42 by jsalmi           ###   ########.fr       */
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

# define MAX_ENTITIES 64400

enum e_entity_types
{
	ENTITY_MELON_GOLEM = 0,
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

typedef struct s_vox_entity_data
{
	char		*name;
	char		*readable_name;
	char		type;			// 'e_entity_types';
	char		*model_path;
}		t_vox_entity_data;

static const t_vox_entity_data	g_entity_data[] = {
	{
		"ENTITY_MELON_GOLEM",
		"Melon Golem",
		ENTITY_MELON_GOLEM,
		MODEL_PATH"melon_golem/melon_golem.obj"
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
	float	roll;

	float	front[3];
	float	up[3];
	float	velocity[3];

	char	type;	// 'e_entity_types';
	char	state;	// 'e_entity_states';

	char	needs_update;

	// Debug
	char	draw_dir;

	// Matrices
	float	scale_m4[16];
	float	rot_m4[16];
	float	trans_m4[16];
}		t_vox_entity;

void	vox_entity_new(t_vox_entity *entity);
void	vox_entity_update(t_vox_entity *entity);

void	vox_entity_event(t_vox_entity *entity, t_fps *fps);

void	set_entity_at_world_pos(t_world *info, float *world_pos, int entity_type);

#endif