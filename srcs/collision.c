#include "shaderpixel.h"

/*
 * Check if pos is colliding with the edges of a triangle;
 */
int	triangle_edge_collision(float *pos, float *p1, float *p2, float *p3)
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
int	triangle_vertex_collision(float *pos, float *p1, float *p2, float *p3)
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
int	triangle_face_collision(float *pos, float *p1, float *p2, float *p3)
{
	(void)pos;
	(void)p1;
	(void)p2;
	(void)p3;
	return (0);
}

/*
 * p1-3 are the 3 points of the triangle.
 */
int	triangle_collision(float *pos, float *p1, float *p2, float *p3)
{
	triangle_face_collision(pos, p1, p2, p3);
	triangle_vertex_collision(pos, p1, p2, p3);
	triangle_edge_collision(pos, p1, p2, p3);
	return (0);
}

// http://www.peroxide.dk/papers/collision/collision.pdf
/*
 * Takes in position of ellipsoid and the mesh we want to check its colliding;
 * Returns 1 if yes, 0 if no;
 * 
 * NOTE: The coordinates for pos and mesh triangle vertex pos need to be in the
 *  same coordinate space. (just so you dont forget)
 */
int	ellipsoid_collision(float *pos, t_mesh *mesh)
{
	for (int elem = 0; elem < mesh->element_amount; elem++)
	{
		for (size_t ind = 0; ind < mesh->elements[elem].index_amount; ind++)
		{
			triangle_collision(pos,

				(float [3]){mesh->vertices[mesh->elements[elem].indices[ind * 9 + 0]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 1]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 2]]},

				(float [3]){mesh->vertices[mesh->elements[elem].indices[ind * 9 + 3]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 4]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 5]]},

				(float [3]){mesh->vertices[mesh->elements[elem].indices[ind * 9 + 6]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 7]],
				mesh->vertices[mesh->elements[elem].indices[ind * 9 + 8]]}
				);
		}
	}
	return (0);
}