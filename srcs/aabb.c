/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 14:01:02 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 14:10:52 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bmath.h"
#include "aabb.h"
#include "string.h"
#include "libft.h"

/*
 * amount of values in vertices should be vertex_amount * 3 (xyz per vertex);
*/
void	aabb_create(t_aabb *res, float *vertices, int vertex_amount)
{
	res->min[0] = INFINITY;
	res->min[1] = INFINITY;
	res->min[2] = INFINITY;

	res->max[0] = -INFINITY;
	res->max[1] = -INFINITY;
	res->max[2] = -INFINITY;

	for (int i = 0; i < vertex_amount; i++)
	{
		res->min[0] = fmin(res->min[0], vertices[i * 3 + 0]);
		res->min[1] = fmin(res->min[1], vertices[i * 3 + 1]);
		res->min[2] = fmin(res->min[2], vertices[i * 3 + 2]);

		res->max[0] = fmax(res->max[0], vertices[i * 3 + 0]);
		res->max[1] = fmax(res->max[1], vertices[i * 3 + 1]);
		res->max[2] = fmax(res->max[2], vertices[i * 3 + 2]);
	}
}

/*
 * 
*/
void	aabb_create_from_indices(t_aabb *res, float *vertices, unsigned int *indices, int indices_amount)
{
	res->min[0] = INFINITY;
	res->min[1] = INFINITY;
	res->min[2] = INFINITY;

	res->max[0] = -INFINITY;
	res->max[1] = -INFINITY;
	res->max[2] = -INFINITY;

	for (int i = 0; i < indices_amount; i++)
	{
		res->min[0] = fmin(res->min[0], vertices[indices[i] * 3 + 0]);
		res->min[1] = fmin(res->min[1], vertices[indices[i] * 3 + 1]);
		res->min[2] = fmin(res->min[2], vertices[indices[i] * 3 + 2]);

		res->max[0] = fmax(res->max[0], vertices[indices[i] * 3 + 0]);
		res->max[1] = fmax(res->max[1], vertices[indices[i] * 3 + 1]);
		res->max[2] = fmax(res->max[2], vertices[indices[i] * 3 + 2]);
	}
}

/*
 * Transforms the aabb with the given model matrix;
 * Probably to world space, dont know if this has any other use;
*/
void	aabb_transform(t_aabb *a, float *model)
{
	float	v4[V4_SIZE];
	t_aabb	temp;

	v4_new(v4, a->min[0], a->min[1], a->min[2], 1);
	v4_multiply_m4(v4, v4, model);
	v4_to_v3(a->min, v4);

	v4_new(v4, a->max[0], a->max[1], a->max[2], 1);
	v4_multiply_m4(v4, v4, model);
	v4_to_v3(a->max, v4);

	temp = *a;
	if (temp.min[0] > temp.max[0])
		ft_fswap(&temp.min[0], &temp.max[0]);
	if (temp.min[1] > temp.max[1])
		ft_fswap(&temp.min[1], &temp.max[1]);
	if (temp.min[2] > temp.max[2])
		ft_fswap(&temp.min[2], &temp.max[2]);
	*a = temp;
}

// TODO : Replace the old version with this;
void	aabb_transform_new(t_aabb *a, float *model)
{
	float	v4[V4_SIZE];

	a->min[0] = INFINITY;
	a->min[1] = INFINITY;
	a->min[2] = INFINITY;

	a->max[0] = -INFINITY;
	a->max[1] = -INFINITY;
	a->max[2] = -INFINITY;

	for (int i = 0; i < 24; i += 3)
	{
		v4_new(v4, a->vertices[i + 0], a->vertices[i + 1], a->vertices[i + 2], 1);
		v4_multiply_m4(v4, v4, model);
		a->vertices[i + 0] = v4[0];
		a->vertices[i + 1] = v4[1];
		a->vertices[i + 2] = v4[2];

		a->min[0] = fmin(a->min[0], a->vertices[i + 0]);
		a->min[1] = fmin(a->min[1], a->vertices[i + 1]);
		a->min[2] = fmin(a->min[2], a->vertices[i + 2]);

		a->max[0] = fmax(a->max[0], a->vertices[i + 0]);
		a->max[1] = fmax(a->max[1], a->vertices[i + 1]);
		a->max[2] = fmax(a->max[2], a->vertices[i + 2]);
	}
}

void	aabb_vertify(t_aabb *a)
{
	float	vertices[] = {
		// front
		a->min[0], a->max[1], a->max[2], // top left		0
		a->max[0], a->max[1], a->max[2], // top right	1
		a->min[0], a->min[1], a->max[2], // bot left		2
		a->max[0], a->min[1], a->max[2], // bot right	3

		// back
		a->min[0], a->max[1], a->min[2], // top left		4
		a->max[0], a->max[1], a->min[2], // top right	5
		a->min[0], a->min[1], a->min[2], // bot left		6
		a->max[0], a->min[1], a->min[2] // bot right		7
	};
	memcpy(a->vertices, vertices, sizeof(float) * 24);
}

/*
 * Checks if aabb is inside aabb;
*/
int	aabb_aabb_collision(t_aabb *a, t_aabb *b)
{
	return (
		(a->min[0] <= b->max[0] && a->max[0] >= b->min[0]) &&
		(a->min[1] <= b->max[1] && a->max[1] >= b->min[1]) &&
		(a->min[2] <= b->max[2] && a->max[2] >= b->min[2]) &&
		1);
}

/*
 * Checks if point is inside aabb;
*/
int	point_aabb_collision(float *point, t_aabb *b)
{
	return (
		(point[0] >= b->min[0] && point[0] <= b->max[0]) &&
		(point[1] >= b->min[1] && point[1] <= b->max[1]) &&
		(point[2] >= b->min[2] && point[2] <= b->max[2]) &&
		1);
}

void	aabb_print(t_aabb *a)
{
	printf("[%s]\n", __FUNCTION__);
	for (int i = 0; i < 3; i++)
		printf("\tmin[%d] = %.2f;\n", i, a->min[i]);
	for (int i = 0; i < 3; i++)
		printf("\tmax[%d] = %.2f;\n", i, a->max[i]);
}

/*
 * From 'point' distance to nearest corner in 'b';
*/
float	point_aabb_nearest_distance(float *point, t_aabb *b)
{
	float	dist = INFINITY;

	float d0 = v3_dist_sqrd(point, (float []){b->min[0], b->min[1], b->min[2]});
	float d1 = v3_dist_sqrd(point, (float []){b->min[0], b->max[1], b->min[2]});
	float d2 = v3_dist_sqrd(point, (float []){b->max[0], b->max[1], b->min[2]});
	float d3 = v3_dist_sqrd(point, (float []){b->max[0], b->min[1], b->min[2]});

	float b0 = v3_dist_sqrd(point, (float []){b->max[0], b->max[1], b->max[2]});
	float b1 = v3_dist_sqrd(point, (float []){b->max[0], b->min[1], b->max[2]});
	float b2 = v3_dist_sqrd(point, (float []){b->min[0], b->min[1], b->max[2]});
	float b3 = v3_dist_sqrd(point, (float []){b->min[0], b->max[1], b->max[2]});

	float c0 = fmin(d0, d1);
	float c1 = fmin(d2, d3);

	float e0 = fmin(b0, b1);
	float e1 = fmin(b2, b3);

	float f0 = fmin(c0, c1);
	float f1 = fmin(e0, e1);

	return (fmin(f0, f1));
}

/*
 * Returns distance squared from the center of the aabb 'b' to the point 'point';
*/
float	point_aabb_center_distance(float *point, t_aabb *b)
{
	float	e[3];
	float	c[3];

	v3_multiply_f(e, v3_sub(e, b->max, b->min), 0.5f);
	v3_add(c, b->min, e);
	return (v3_dist_sqrd(point, c));
}