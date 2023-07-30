/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 12:03:17 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 11:40:57 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PLAYER_H
# define PLAYER_H

# include "key.h"
# include "fps.h"
# include "camera.h"

typedef struct	s_player	t_player;
typedef struct	s_entity	t_entity;

typedef struct s_player_info
{
	float	reach; // how far away player can reach from player position;
	int		equipped_block;
	int		equipped_hotbar; // 0 - 8; one of 'hotbar_item_ids' indices;
	int		hotbar_item_ids[9];
}	t_player_info;

struct s_player
{
	t_camera	camera;

	float		velocity[V3_SIZE];
	float		gravity;

	int			moving;
	int			colliding;
	int			enabled_mouse;
	double		mouse_pos[2];
	double		last_mouse_pos[2];

	t_player_info info;

	t_aabb		aabb;

};

void		new_player(t_player *player);
void		player_events(t_player *player, t_key *keys, GLFWwindow *win);
void		player_movement(t_player *player, GLFWwindow *win, t_fps *fps);
void		player_movement_creative(t_player *player, GLFWwindow *win, t_fps *fps);
void		player_movement_survival(t_player *player, GLFWwindow *win, t_fps *fps);
void		player_apply_velocity(t_player *player);
void		player_apply_gravity(t_player *player);
void		player_looking(t_player *player, GLFWwindow *win);
void		player_entity_collision(t_player *player, t_entity *entity);
int			player_entity_mesh_collision(t_player *player, t_entity *entity);
void		player_print(t_player *player);

#endif