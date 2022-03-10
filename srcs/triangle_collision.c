#include "shaderpixel.h"

float	*triangle_face_normal(float *res, float *n0, float *n1, float *n2)
{
	float	norm[3];
	float	n10[3];
	float	n20[3];

	vec3_sub(n10, n1, n0);
	vec3_sub(n20, n2, n0);
	vec3_cross(norm, n10, n20);
	res[0] = norm[0];
	res[1] = norm[1];
	res[2] = norm[2];
	return (res);
}

/*
 * Yoinked from : https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/ray-triangle-intersection-geometric-solution
 * Improvement? : Look into : Möller-Trumbore algorithm
 * Improvment: Instead of calculating the normals, we can give in them.
 *
 * Checks if ray, starting at 'orig' and going in 'dir', intersects triangle,
 *	made of 'v0', 'v1', 'v2'.
*/
int	ray_triangle_intersect(float *orig, float *dir, float *v0, float *v1, float *v2, float *intersect_point)
{
	// compute plane's normal
	float	N[3];
	triangle_face_normal(N, v0, v1, v2);

	// back face culler
	/* Enable for only one sided collision detection;
	if (vec3_dot(dir, N) < 0)
		return (0);
		*/

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

	memcpy(intersect_point, P, sizeof(float) * 3);
	return (1); // this ray hits the triangle
}
