#include "collision.h"
#include "bmath.h"

/*
 * We are taking a pause on the ellipsoid on triangle collision detection,
 * and we are now trying to implement the AABB detection... so that we can have
 * something to work with, its been so long since ive made some progress.
 * Ill implement the more complicated thing later when i know more.
*/

/*NOTE*/
/*
 * This detection type probably only works on objects, not really terrain....
*/

/*
 * amount of values in vertices should be vertex_amount * 3 (xyz per vertex);
*/
void	aabb_create(t_aabb *res, float *vertices, size_t vertex_amount)
{
	res->min[0] = INFINITY;
	res->min[1] = INFINITY;
	res->min[2] = INFINITY;

	res->max[0] = -INFINITY;
	res->max[1] = -INFINITY;
	res->max[2] = -INFINITY;

	for (size_t i = 0; i < vertex_amount; i++)
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
 * Transforms the aabb with the given model matrix;
 * Probably to world space, dont know if this has any other use;
*/
void	aabb_transform(t_aabb *a, float *model)
{
	float	v4[VEC4_SIZE];

	vec4_new(v4, a->min[0], a->min[1], a->min[2], 1);
	vec4_multiply_mat4(v4, v4, model);
	vec4_to_vec3(a->min, v4);

	vec4_new(v4, a->max[0], a->max[1], a->max[2], 1);
	vec4_multiply_mat4(v4, v4, model);
	vec4_to_vec3(a->max, v4);
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