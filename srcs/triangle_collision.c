#include "shaderpixel.h"

/*
 * Checks if line, starting at 'orig' and going in 'dir', intersects triangle,
 *	made of 'p1', 'p2', 'p3'.
*/
int	line_triangle_intersect(float *orig, float *dir, float *p1, float *p2, float *p3)
{
	float	e1[VEC3_SIZE];
	float	e2[VEC3_SIZE];
	float	n[VEC3_SIZE];
	float	ao[VEC3_SIZE];
	float	dao[VEC3_SIZE];

	vec3_sub(e1, p2, p1);
	vec3_sub(e2, p3, p1);
	vec3_cross(n, e1, e1);
	float det = -vec3_dot(dir, n);
	float invdet = 1.0 / det;
	vec3_sub(ao, orig, p1);
	vec3_sub(dao, ao, dir);
	float u =  vec3_dot(e2, dao) * invdet;
	float v = -vec3_dot(e1, dao) * invdet;
	float t =  vec3_dot(ao, n)  * invdet;
	return (det >= 1e-6 && t >= 0.0 && u >= 0.0 && v >= 0.0 && (u + v) <= 1.0);
}
