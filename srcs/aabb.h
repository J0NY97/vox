/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   aabb.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 13:59:58 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 14:11:04 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AABB_H
# define AABB_H

typedef struct s_aabb
{
	float	min[3];
	float	max[3];
	float	vertices[24];
}			t_aabb;

void		aabb_create(t_aabb *res, float *vertices, int vertex_amount);
void		aabb_create_from_indices(t_aabb *res, float *vertices, unsigned int *indices, int indices_amount);
void		aabb_transform(t_aabb *a, float *model);
void		aabb_transform_new(t_aabb *a, float *model);
void		aabb_vertify(t_aabb *a);
int			aabb_aabb_collision(t_aabb *a, t_aabb *b);
int			point_aabb_collision(float *point, t_aabb *b);
float		point_aabb_nearest_distance(float *point, t_aabb *b);
float		point_aabb_center_distance(float *point, t_aabb *b);

void		aabb_print(t_aabb *a);

#endif
