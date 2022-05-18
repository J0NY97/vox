#include "shaderpixel.h"

/*
 * Takes in 'float [12]';
 */
void	get_fullscreen_vertices(float *vertices)
{
	float	v[] = {
		-1.0f, -1.0f, -0.0f,
		 1.0f,  1.0f, -0.0f,
		-1.0f,  1.0f, -0.0f,
		 1.0f, -1.0f, -0.0f
	};
	memcpy(vertices, v, sizeof(float) * 12);
}

/*
 * Takes in 'unsigned int [6]';
 */
void	get_fullscreen_indices(unsigned int *indices)
{
	unsigned int i[] = {
		0, 1, 2,
		0, 3, 1
	};
	memcpy(indices, i, sizeof(unsigned int) * 6);
}

void	new_fractal2d(t_fractal2d *fractal)
{
	glGetError();// just clearing all errors that occurred before this func;

	glGenVertexArrays(1, &fractal->vao);
	glGenBuffers(1, &fractal->vbo);
	glGenBuffers(1, &fractal->ebo);

	glBindVertexArray(fractal->vao);

	get_fullscreen_vertices(fractal->vertices);
	glBindBuffer(GL_ARRAY_BUFFER, fractal->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, &fractal->vertices[0], GL_STATIC_DRAW);

	get_fullscreen_indices(fractal->indices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, &fractal->indices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	LG_INFO("New fractal made. (%d)", glGetError());
}

void	render_fractal2d(t_fractal2d *fractal, GLuint shader)
{
	glUseProgram(shader);
	glBindVertexArray(fractal->vao);
	glBindBuffer(GL_ARRAY_BUFFER, fractal->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fractal->ebo);
	glEnableVertexAttribArray(0); // <- does the vao have this memorized?
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}