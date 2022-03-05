/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collision.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 09:39:11 by jsalmi            #+#    #+#             */
/*   Updated: 2022/03/05 11:14:54 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLLISION_H
# define COLLISION_H
# include "libft.h"
# include "stdlib.h"
# include "math.h"
# include "stdio.h"

typedef struct	s_entity	t_entity;

////////////////////////
//	AXIS ALIGNED BOUNDING BOX COLLISION
////////////////////////

typedef struct s_aabb
{
	float	min[3];
	float	max[3];
	float	vertices[24];
	float	normals[24];
}			t_aabb;

void		aabb_create(t_aabb *res, float *vertices, size_t vertex_amount);
void		aabb_transform(t_aabb *a, float *model);
int			aabb_aabb_collision(t_aabb *a, t_aabb *b);
int			point_aabb_collision(float *point, t_aabb *b);

void		aabb_print(t_aabb *a);

////////////////////////
//	TRIANGLE
////////////////////////

int			ray_triangle_intersect(float *orig, float *dir, float *p1, float *p2, float *p3, float *intersect_point);

////////////////////////
//	PLANE
////////////////////////

float		ray_plane_intersect(float *orig, float *dir, float *pos, float *norm);

///////////////////
//	ELLIPSOID
///////////////////

int			ellipsoid_collision(float *pos, float *velocity, float *p1, float *p2, float *p3);

///////////////////
//	DETECTOR
///////////////////

typedef struct s_collision_detector
{
	t_list		*nodes;
	int			show_aabb; // DEBUG purposes;
}				t_collision_detector;

typedef struct s_collision_node
{
	t_entity	*entity;
	t_aabb		aabb;
	int			collided;
}				t_collision_node;

void	new_collision_detector(t_collision_detector *detector);
void	add_collision_entity(t_collision_detector *detector, t_entity *entity);
void	remove_collision_entity(t_collision_detector *detector, t_entity *entity);
void	free_collision_detector(t_collision_detector *detector);
void	free_collision_node(void *pointer, size_t size);
void	update_collision_detector(t_collision_detector *detector);

#endif
