#ifndef COLLISION_H
# define COLLISION_H
# include "stdlib.h"
# include "math.h"
# include "stdio.h"

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

int			ray_triangle_intersect(float *orig, float *dir, float *p1, float *p2, float *p3, float *t_normal);

////////////////////////
//	PLANE
////////////////////////

float		ray_plane_intersect(float *orig, float *dir, float *pos, float *norm);

///////////////////
//	ELLIPSOID
///////////////////

int			ellipsoid_collision(float *pos, float *velocity, float *p1, float *p2, float *p3);

#endif
