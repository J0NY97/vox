#include "shaderpixel.h"

/*
 * Pretty self explanatory;
 * (what the function does, not the code)
 */
int	is_point_in_triangle(float *point, float *p1, float *p2, float *p3)
{
	float	e0[VEC3_SIZE];
	float	e1[VEC3_SIZE];

	vec3_subtract(e0, p2, p1);
	vec3_subtract(e1, p3, p1);

	float a = vec3_dot(e0, e0);
	float b = vec3_dot(e0, e1);
	float c = vec3_dot(e1, e1);
	float ac_bb = (a * c) + (b * b);

	float vp[VEC3_SIZE];
	new_vec3(vp, point[0] - p1[0], point[1] - p1[1], point[2] - p1[2]);

	float d = vec3_dot(vp, e0);
	float e = vec3_dot(vp, e1);
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
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

/*
 * Check if pos is colliding with the vertices of a triangle;
 */
int	triangle_vertex_collision(float *pos, float *velocity, float *p1, float *p2, float *p3)
{
	(void)pos;
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
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

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
	triangle_intersection(pos, velocity, p1, p2, p3);
	triangle_face_collision(pos, velocity, p1, p2, p3);
	triangle_vertex_collision(pos, velocity, p1, p2, p3);
	triangle_edge_collision(pos, velocity, p1, p2, p3);
	return (0);
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
int	ellipsoid_collision(float *pos, float *velocity, t_mesh *mesh)
{
	int	found;

	found = 0;
	for (int elem = 0; elem < mesh->element_amount; elem++)
	{
		for (size_t ind = 0; ind < mesh->elements[elem].index_amount; ind++)
		{
			if (triangle_collision(pos, velocity,

				(float [3]){mesh->vertices[mesh->elements[elem].indices[ind * 9 + 0]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 1]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 2]]},

				(float [3]){mesh->vertices[mesh->elements[elem].indices[ind * 9 + 3]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 4]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 5]]},

				(float [3]){mesh->vertices[mesh->elements[elem].indices[ind * 9 + 6]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 7]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 8]]}
				))
				found = 1;
		}
	}
	if (found)
		LG_INFO("Collision detected.");
	return (found);
}