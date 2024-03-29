/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox_entity.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:21:38 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 13:26:44 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ENTITY_H
# define ENTITY_H

# include "stdlib.h"
# include "glad.h"
# include "glfw3.h"
# include "enum.h"
# include "model.h"
# include "fps.h"

# define MAX_ENTITIES 1024

typedef struct s_world t_world;
typedef struct s_camera t_camera;

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
	char			*name;
	char			*readable_name;
	unsigned char	type;			// 'e_entity_types';
	char			*model_path;
	float			fall_speed;
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

typedef struct s_entity
{
	int id;

	int		health; // todo
	int		speed;

	float	pos[3];	// model pos / tbh entity pos aswell
	float	rot[3];	// model rotation
	float	scale;	// model scale

	float	yaw;
	float	pitch;

	float	front[3];
	float	up[3];
	float	velocity[3];

	unsigned char	type;	// 'e_entity_types';
	char	state;	// 'e_entity_states';

	char	needs_update;

	/// @brief TODO : ai shouldnt be in here, ai entities should be their own type; and should get handled by other means;
	char	ai;

	char collision_detection_enabled;
	char collision_use_precise;
	char collision;

	// matrices
	float	scale_mat[16];
	float	rot_mat[16]; // TODO : Quaternion;
	float	trans_mat[16];
	float	model_mat[16];
	t_model *model;

	// debug
	char	draw_dir;
	char	draw_terrain_collision;
	char	draw_aabb;
	t_aabb			aabb;
	float			aabb_vertices[24];
	unsigned int	aabb_indices[36];
}		t_entity;

void	entity_init(t_entity *entity);
void	entity_update(t_entity *entity);
void	ai_entity_event(t_entity *entity, t_world *info, t_fps *fps);
void	entity_render(t_entity *entity, t_camera *camera, t_model *model, GLuint shader);
void	entity_print(t_entity *entity);

#endif