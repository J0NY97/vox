/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collision.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/04 09:39:11 by jsalmi            #+#    #+#             */
/*   Updated: 2022/03/17 12:58:50y jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COLLISION_H
# define COLLISION_H
# include "libft.h"
# include "stdlib.h"
# include "math.h"
# include "bmath.h"
# include "stdio.h"

# define EPSILON 0.00001f

typedef struct	s_entity	t_entity;

////////////////////////
//	AXIS ALIGNED BOUNDING BOX COLLISION
////////////////////////

typedef struct s_aabb
{
	float	min[3];
	float	max[3];
	float	vertices[24];
}			t_aabb;

void		aabb_create(t_aabb *res, float *vertices, size_t vertex_amount);
void		aabb_transform(t_aabb *a, float *model);
void		aabb_transform_new(t_aabb *a, float *model);
void		aabb_vertify(t_aabb *a);
int			aabb_aabb_collision(t_aabb *a, t_aabb *b);
int			point_aabb_collision(float *point, t_aabb *b);

void		aabb_print(t_aabb *a);

////////////////////////
//	LINE
////////////////////////

int			point_on_line(float *point, float *start, float *end);
int			ray_line_intersect(float *orig, float *dir, float *p1, float *p2, float *intersect_point);

////////////////////////
//	TRIANGLE
////////////////////////

int			point_in_triangle(float *p, float *v1, float *v2, float *v3);
int			old_ray_triangle_intersect(float *orig, float *dir, float *p1, float *p2, float *p3, int cull_backface, float *intersect_point);
int			ray_triangle_intersect(float *orig, float *dir, float *p0, float *p1, float *p2, float *out_intersection_point);
float		*triangle_face_normal(float *res, float *n0, float *n1, float *n2);

////////////////////////
//	PLANE
////////////////////////

float		ray_plane_intersect(float *orig, float *dir, float *pos, float *norm);

///////////////////
//	ELLIPSOID
///////////////////

int			ellipsoid_collision(float *pos, float *velocity, float *p1, float *p2, float *p3);

#endif
