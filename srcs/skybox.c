#include "shaderpixel.h"

unsigned int	load_cube_map(char **paths, int paths_size)
{
	unsigned int	texture;
	int				orig_img_format = GL_RGB;
	t_bimgf			image;
	int				i;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	i = -1;
	while (++i < paths_size)
	{
		if (!bimgf_load(&image, paths[i]))
		{
			LG_WARN("Couldnt load texture: '%s'", paths[i]);
			return (0);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
			image.w, image.h, 0, orig_img_format, GL_UNSIGNED_BYTE, image.pixels);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
		bimgf_free(&image);
	}
	return (texture);
}

float	*skybox_vertices(float *res, size_t *size)
{
	float	vertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};
	*size = sizeof(vertices);
	memcpy(res, vertices, *size);
	return (res);
}

void	render_skybox(t_skybox *skybox, t_camera *camera)
{
	float	view[MAT4_SIZE];
	float	temp[MAT3_SIZE];
	int		error;

	error = glGetError();
	if (error)
		LG_ERROR("before (%d)", error);

	if (!skybox || !camera)
	{
		if (!skybox)
			LG_WARN("No skybox.");
		if (!camera)
			LG_WARN("No camera.");
		return ;
	}
	glDepthFunc(GL_LEQUAL);
	glUseProgram(skybox->shader);

	mat4_assign(view, camera->view);
	view[12] = 0.0f;
	view[13] = 0.0f;
	view[14] = 0.0f;
	glUniformMatrix4fv(glGetUniformLocation(skybox->shader, "view"), 1, GL_FALSE, &view[0]);
	glUniformMatrix4fv(glGetUniformLocation(skybox->shader, "projection"), 1, GL_FALSE, &camera->projection[0]);

	glBindVertexArray(skybox->vao);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->texture);

	glBindBuffer(GL_ARRAY_BUFFER, skybox->vbo_position);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
	glUseProgram(0);
	glDepthFunc(GL_LESS);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

t_skybox	*new_skybox(t_skybox *skybox, const char **faces)
{
	int	error = glGetError();

	skybox->texture = load_cube_map(faces, 6);
	new_shader(&skybox->shader, SHADER_PATH"skybox.vs", SHADER_PATH"skybox.fs");
	skybox_vertices(skybox->vertices, &skybox->vertices_size);
	glGenVertexArrays(1, &skybox->vao);
	glGenBuffers(1, &skybox->vbo_position);
	glBindVertexArray(skybox->vao);
	glBindBuffer(GL_ARRAY_BUFFER, skybox->vbo_position);
	glBufferData(GL_ARRAY_BUFFER, skybox->vertices_size, &skybox->vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

	error = glGetError();
	if (error)
		LG_ERROR("(error: %d)", error);
	return (skybox);
}