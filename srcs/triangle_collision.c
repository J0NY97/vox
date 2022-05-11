#include "shaderpixel.h"

/*
 * Counter Clock Wise; winding, normals pointing outward;
*/
float	*triangle_face_normal(float *res, float *p1, float *p2, float *p3)
{
	float	u[3];
	float	v[3];

	vec3_sub(u, p2, p1);
	vec3_sub(v, p3, p1);
	res[0] = (u[1] * v[2]) - (u[2] * v[1]);
	res[1] = (u[2] * v[0]) - (u[0] * v[2]);
	res[2] = (u[0] * v[1]) - (u[1] * v[0]);
	vec3_normalize(res, res);
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
int	old_ray_triangle_intersect(float *orig, float *dir, float *v0, float *v1, float *v2, int cull_backface, float *intersect_point)
{
	// compute plane's normal
	float	N[3];
	triangle_face_normal(N, v0, v1, v2);

	// back face culler
	/* Enable for only one sided collision detection;
		*/
	if (cull_backface && vec3_dot(dir, N) > EPSILON)
		return (0);

	float	area2 = vec3_length(N);

	// Step 1: finding P

	// check if ray and plane are parallel ?
	float	NdotRayDirection = vec3_dot(N, dir);
	if (fabs(NdotRayDirection) < EPSILON)
		return (0); // they are parallel so they don't intersect !

	// compute d parameter using equation 2
	float	d = -vec3_dot(N, v0);

	// compute t (equation 3)
	float	t = -(vec3_dot(N, orig) + d) / NdotRayDirection;

	// check if the triangle is in behind the ray
	if (t < EPSILON)
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
	if (vec3_dot(N, C) < EPSILON)
		return (0); // P is on the right side

	// edge 1
	float	edge1[3];
	vec3_sub(edge1, v2, v1);

	float	vp1[3];
	vec3_sub(vp1, P, v1);
	vec3_cross(C, edge1, vp1);
	if (vec3_dot(N, C) < EPSILON)
		return (0); // P is on the right side

	// edge 2
	float	edge2[3];
	vec3_sub(edge2, v0, v2);
	float	vp2[3];
	vec3_sub(vp2, P, v2);
	vec3_cross(C, edge2, vp2);
	if (vec3_dot(N, C) < EPSILON)
		return (0); // P is on the right side;

	memcpy(intersect_point, P, sizeof(float) * 3);
	return (1); // this ray hits the triangle
}

/*
 * Möller omega 3 fish oil and trumbone algorithm;
*/
int	ray_triangle_intersect(float *orig, float *dir, float *p0, float *p1, float *p2, float *out_intersection_point, float *out_dist)
{
	float	e1[3];
	float	e2[3];
	float	s[3];
	float	q[3];
	float	h[3];
	float	a, f, u, v;

	vec3_sub(e1, p1, p0);
	vec3_sub(e2, p2, p0);
	vec3_cross(h, dir, e2);
	a = vec3_dot(e1, h);
	if (a > -EPSILON && a < EPSILON)
		return (0); // ray is parallel to triangle;
	f = 1.0f / a;
	vec3_sub(s, orig, p0);
	u = f * vec3_dot(s, h);
	if (u < 0.0f || u > 1.0f)
		return (0);
	vec3_cross(q, s, e1);
	v = f * vec3_dot(dir, q);
	if (v < 0.0f || u + v > 1.0f)
		return (0);
	// Get intersection point;
	*out_dist = f * vec3_dot(e2, q);
	if (*out_dist > EPSILON)
		vec3_add(out_intersection_point, orig, vec3_multiply_f(out_intersection_point, dir, *out_dist));
	else
		return (0);
	return (1);
}

int	point_in_triangle(float *p, float *v1, float *v2, float *v3)
{
	float	t1[VEC3_SIZE];
	float	t2[VEC3_SIZE];
	float	t3[VEC3_SIZE];

	vec3_sub(t1, v1, p);
	vec3_sub(t2, v2, p);
	vec3_sub(t3, v3, p);

	float	u[VEC3_SIZE];
	float	v[VEC3_SIZE];
	float	w[VEC3_SIZE];

	vec3_cross(u, t2, t3);
	vec3_cross(v, t3, t1);
	vec3_cross(w, t1, t2);

	if (vec3_dot(u, v) < EPSILON)
		return (0);
	if (vec3_dot(u, w) < EPSILON)
		return (0);
	return (1);
}

