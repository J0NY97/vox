/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/13 17:25:11 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VOX_ENTITY_H
# define VOX_ENTITY_H

enum e_entity_types
{
	ENTITY_MELON_GOLEM = 0,
	ENTITY_AMOUNT
};

typedef struct s_vox_entity_data
{
	char	*texture;
}		t_vox_entity_data;

static const t_vox_entity_data	g_entity_data[] = {
	
};

typedef struct s_vox_entity
{
	float	pos[3];
	float	rot[3];

	char	type; // 'e_entity_types';
}		t_vox_entity;

#endif