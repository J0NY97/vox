/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/18 12:09:04 by jsalmi           ###   ########.fr       */
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

# define MAX_ENTITIES 64400

enum e_entity_types
{
	ENTITY_MELON_GOLEM = 0,
	ENTITY_AMOUNT
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
	float	pos[3];
	float	rot[3];
	float	scale;

	char	type; // 'e_entity_types';

	// Matrices
	float	scale_m4[16];
	float	rot_m4[16];
	float	trans_m4[16];
}		t_vox_entity;

void	vox_entity_new(t_vox_entity *entity);
void	vox_entity_update(t_vox_entity *entity);

void	set_entity_at_world_pos(t_world *info, float *world_pos, int entity_type);

#endif