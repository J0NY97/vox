#include "shaderpixel.h"

/*
 * Yoinked from : https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
 *
 * Checks if ray, starting at 'orig' and going in 'dir', intersects triangle,
 *	made of 'v0', 'v1', 'v2'.
*/
int	ray_triangle_intersect(float *orig, float *dir, float *v0, float *v1, float *v2, float *t_normal)
{
	// compute plane's normal
	float	v0v1[3];
	vec3_sub(v0v1, v1, v0);

	float	v0v2[3];
	vec3_sub(v0v2, v2, v0);

	// no need to normalize
	float	N[3];
	vec3_cross(N, v0v1, v0v2); // N
	float	area2 = vec3_length(N);

	// Step 1: finding P

	// check if ray and plane are parallel ?
	float	NdotRayDirection = vec3_dot(N, dir);
	if (fabs(NdotRayDirection) < 0.001) // almost 0
		return (0); // they are parallel so they don't intersect !

	// compute d parameter using equation 2
	float	d = -vec3_dot(N, v0);

	// compute t (equation 3)
	float	t = -(vec3_dot(N, orig) + d) / NdotRayDirection;

	// check if the triangle is in behind the ray
	if (t < 0)
		return (0); // the triangle is behind

	// compute the intersection point using equation 1
	float	P[3];
	vec3_add(P, orig, vec3_multiply_f(P, dir, t));

	// Step 2: inside-outside test
	float	C[3]; // vector perpendicular to triangle's plane

	// edge 0
	float	edge0[3];
	vec3_sub(edge0, v1, v0);

	float	vp0[3];
	vec3_sub(vp0, P, v0);

	vec3_cross(C, edge0, vp0);
	if (vec3_dot(N, C) < 0)
		return (0); // P is on the right side

	// edge 1
	float	edge1[3];
	vec3_sub(edge1, v2, v1);

	float	vp1[3];
	vec3_sub(vp1, P, v1);
	vec3_cross(C, edge1, vp1);
	if (vec3_dot(N, C) < 0)
		return (0); // P is on the right side

	// edge 2
	float	edge2[3];
	vec3_sub(edge2, v0, v2);
	float	vp2[3];
	vec3_sub(vp2, P, v2);
	vec3_cross(C, edge2, vp2);
	if (vec3_dot(N, C) < 0)
		return (0); // P is on the right side;

	return (1); // this ray hits the triangle
}
