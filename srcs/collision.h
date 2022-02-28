#ifndef COLLISION_H
# define COLLISION_H
# include "stdlib.h"
# include "math.h"
# include "stdio.h"

/*
 * Axis-aligned bounding box;
 */
typedef struct s_aabb
{
	float	min[3];
	float	max[3];
}			t_aabb;

void		aabb_create(t_aabb *res, float *vertices, size_t vertex_amount);
void		aabb_transform(t_aabb *a, float *model);
int			aabb_aabb_collision(t_aabb *a, t_aabb *b);
int			point_aabb_collision(float *point, t_aabb *b);
void		aabb_render(t_aabb *a);

void		aabb_print(t_aabb *a);

#endif
