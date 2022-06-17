/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/17 13:21:51 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VOX_ENTITY_H
# define VOX_ENTITY_H

# include "stdlib.h"
# include "glad.h"
# include "glfw3.h"
# include "enum.h"
# include "model.h"

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
}		t_vox_entity;

void	vox_entity_new(t_vox_entity *entity);

#endif