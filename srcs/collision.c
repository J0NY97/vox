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

#include "shaderpixel.h"
void	aabb_render(t_aabb *a)
{
	float	vertices[] = {
		// front
		a->min[0], a->max[1], a->max[2], // top left	0
		a->max[0], a->max[1], a->max[2], // top right	1
		a->min[0], a->min[1], a->max[2], // bot left	2
		a->max[0], a->min[1], a->max[2], // bot right	3

		// back
		a->min[0], a->max[1], a->min[2], // top left	4
		a->max[0], a->max[1], a->min[2], // top right	5
		a->min[0], a->min[1], a->min[2], // bot left	6
		a->max[0], a->min[1], a->min[2] // bot right	7
	};

	float	colors[8 * 3];
	float	temp[3] = {255, 0, 0};
	memset_pattern(colors, 8 * 3 * sizeof(float), temp, sizeof(float) * 3);

	unsigned int	indices[] = {
		// front
			//ftl, ftr, fbl,
			//ftr, fbr, fbl
			0, 1, 2,
			1, 3, 2,
		// right
			//ftr, btr, fbr,
			//btr, bbr, fbr,
			1, 5, 3,
			5, 7, 3,
		// back
			//btl, btr, bbl,
			//btr, bbr, bbl,
			4, 5, 6,
			5, 7, 6,
		// left
			//btl, ftl, bbl,
			//ftl, fbl, bbl,
			4, 0, 6,
			0, 2, 6,
		// top
			//btl, btr, ftl,
			//btr, ftr, ftl,
			4, 5, 0,
			5, 1, 0,
		// bot
			//bbl, bbr, fbl,
			//bbr, fbr, fbl
			6, 7, 2,
			7, 3, 2
	};

//	printf("vertices size : %d * %d * %d = %d == %d\n", sizeof(float), 8, 3, sizeof(float) * 8 * 3, sizeof(vertices));
//	printf("indices size : %d * %d * %d = %d == %d\n", sizeof(unsigned int), 12, 3, sizeof(unsigned int) * 12 * 3, sizeof(indices));

	GLuint	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	GLuint	vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	GLuint	vbo_col;
	glGenBuffers(1, &vbo_col);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_col);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), &colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	GLuint	ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

	glDrawElements(GL_LINE_LOOP, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, NULL);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	aabb_print(t_aabb *a)
{
	printf("[%s]\n", __FUNCTION__);
	for (int i = 0; i < 3; i++)
		printf("\tmin[%d] = %.2f;\n", i, a->min[i]);
	for (int i = 0; i < 3; i++)
		printf("\tmax[%d] = %.2f;\n", i, a->max[i]);
}
