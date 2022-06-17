#include "shaderpixel.h"

/*
 * Pretty self explanatory;
 * (what the function does, not the code)
 */
int	is_point_in_triangle(float *point, float *p1, float *p2, float *p3)
{
	float	e0[V3_SIZE];
	float	e1[V3_SIZE];

	v3_sub(e0, p2, p1);
	v3_sub(e1, p3, p1);

	float a = v3_dot(e0, e0);
	float b = v3_dot(e0, e1);
	float c = v3_dot(e1, e1);
	float ac_bb = (a * c) + (b * b);

	float vp[V3_SIZE];
	v3_new(vp, point[0] - p1[0], point[1] - p1[1], point[2] - p1[2]);

	float d = v3_dot(vp, e0);
	float e = v3_dot(vp, e1);
	float x = (d * c) - (e * b);
	float y = (e * a) - (d * b);
	float z = x + y - ac_bb;

	return (((uint32_t)z & ~((uint32_t)x | (uint32_t)y)) & 0x80000000);
}

/*
 * Check if pos is colliding with the edges of a triangle;
 */
int	triangle_edge_collision(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	(void)pos;
	(void)velocity;
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

/*
 * Optimization? Check distance to each point with velocity? if thats <= 0???
*/
/*
 * Check if pos is colliding with the vertices of a triangle;
 */
int	triangle_vertex_collision(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	(void)pos;
	(void)velocity;
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

/*
 * Check if pos is colliding with the face of a triangle;
 */
int	triangle_face_collision(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	(void)pos;
	(void)velocity;
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

/*
 * Could this be used to check if youre looking in the direction of this particular triangle?
 *	...probably need to somehow have an infinite velocity...
*/
/*
 * If the pos with velocity ever intersects the plane of the triangle...
 *	at signed_distance(C(t0)) == 1 and signed_distance(C(t1)) == -1;
 *	at t0 the pos it at the front side of triangle, and at t1 its at the back;
 */
int	triangle_intersection(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	(void)pos;
	(void)velocity;
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

/*
 * p1-3 are the 3 points of the triangle.
 */
int	triangle_collision(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	if (triangle_intersection(pos, velocity, p1, p2, p3))
	{
		if (triangle_face_collision(pos, velocity, p1, p2, p3))
		{
			triangle_vertex_collision(pos, velocity, p1, p2, p3);
			triangle_edge_collision(pos, velocity, p1, p2, p3);
		}
	}
	return (0);
}

/*
 * Both 'res' and 'src' should be 'float [V3_SIZE]';
 *
 * Returns 'res';
 */
float	*espace_conversion(float *res, float *src)
{
	(void)src;
	return (res);
}

// http://www.peroxide.dk/papers/collision/collision.pdf
/*
 * Takes in position of ellipsoid and the mesh we want to check its colliding;
 * Returns 1 if yes, 0 if no;
 *
 * NOTE: The coordinates for pos and mesh triangle vertex pos need to be in the
 *  same coordinate space. (just so you dont forget)
 *
 * velocity is basically just the length of the line you want to 'cast' in front
 * and check if it hits a mesh;
 *
 * STEPS:
 * Convert to eSpace, both pos and mesh triangle vertices;
 * check if sphere...
 * 1st ... ever intersects the infinite plane of the triangle;
 * 2nd ... collides with face of triangle;
 * 3rd ... collides with vertices of triangle;
 * 4th ... collides with edges of triangle;
 */
int	ellipsoid_collision(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	float	epos[V3_SIZE];
	int		found;

	found = 0;
	/*
	for (int elem = 0; elem < mesh->element_amount; elem++)
	{
		for (size_t face = 0; face < mesh->elements[elem].face_count; face++)
		{
			int index1 = mesh->elements[elem].indices[face * 3 + 0];
			int index2 = mesh->elements[elem].indices[face * 3 + 1];
			int index3 = mesh->elements[elem].indices[face * 3 + 2];
			new_vec3(p1,
				mesh->vertices[index1 * 3 + 0],
				mesh->vertices[index1 * 3 + 1],
				mesh->vertices[index1 * 3 + 2]);
			new_vec3(p2,
				mesh->vertices[index2 * 3 + 0],
				mesh->vertices[index2 * 3 + 1],
				mesh->vertices[index2 * 3 + 2]);
			new_vec3(p3,
				mesh->vertices[index3 * 3 + 0],
				mesh->vertices[index3 * 3 + 1],
				mesh->vertices[index3 * 3 + 2]);
			espace_conversion(p1, p1);
			espace_conversion(p2, p2);
			espace_conversion(p3, p3);
			espace_conversion(epos, pos);
			if (triangle_collision(pos, velocity, p1, p2, p3))
				found = 1;
		}
	}
	*/
	if (found)
		LG_INFO("Collision detected.");
	return (found);
}
