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

		res->max[0] = fmin(res->max[0], vertices[i * 3 + 0]);
		res->max[1] = fmin(res->max[1], vertices[i * 3 + 1]);
		res->max[2] = fmin(res->max[2], vertices[i * 3 + 2]);
	}
}

/*
 * Transforms the aabb with the given model matrix;
 * Probably to world space, dont know if this has any other use;
*/
void	aabb_transform(t_aabb *a, float *model)
{
	float	v_min[VEC4_SIZE];
	float	v_max[VEC4_SIZE];

	vec4_new(v_min, a->min[0], a->min[1], a->min[2], 1);
	vec4_new(v_max, a->max[0], a->max[1], a->max[2], 1);

	vec4_multiply_mat4(v_min, v_min, model);
	vec4_multiply_mat4(v_max, v_max, model);

	new_vec3(a->min, v_min[0], v_min[1], v_min[2]);
	new_vec3(a->max, v_max[0], v_max[1], v_max[2]);
}

/*
 *
*/
int	aabb_aabb_collision(t_aabb *a, t_aabb *b)
{
	(void)a;
	(void)b;
	return (0);
}

void	aabb_print(t_aabb *a)
{
	printf("[%s]\n", __FUNCTION__);
	for (int i = 0; i < 3; i++)
		printf("\tmin[%d] = %.2f;\n", i, a->min[i]);
	for (int i = 0; i < 3; i++)
		printf("\tmax[%d] = %.2f;\n", i, a->max[i]);

}