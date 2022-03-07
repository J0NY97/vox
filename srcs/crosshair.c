#include "shaderpixel.h"

void	new_crosshair_shader(t_shader *shader)
{
	GLuint	vs_shader;
	GLuint	gs_shader;
	GLuint	fs_shader;

	char	*vs_src;
	char	*gs_src;
	char	*fs_src;

	vs_src = get_file_content(ROOT_PATH"shaders/crosshair.vs");
	gs_src = get_file_content(ROOT_PATH"shaders/crosshair.gs");
	fs_src = get_file_content(ROOT_PATH"shaders/crosshair.fs");

	create_shader(&vs_shader, vs_src, GL_VERTEX_SHADER);
	create_shader(&gs_shader, gs_src, GL_GEOMETRY_SHADER);
	create_shader(&fs_shader, fs_src, GL_FRAGMENT_SHADER);

	free(vs_src);
	free(gs_src);
	free(fs_src);

	shader->program = glCreateProgram();
	glAttachShader(shader->program, vs_shader);
	glAttachShader(shader->program, gs_shader);
	glAttachShader(shader->program, fs_shader);
	glLinkProgram(shader->program);

	check_program_errors(shader->program);
}

void	render_crosshair(void)
{
	GLint	viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glDisable(GL_DEPTH_TEST);
	float	len = 0.009;
	float	h_len = (viewport[2] * len) / viewport[3];
	render_2d_line((float []){-len, 0, 0}, (float []){len, 0, 0}, (float []){1, 1, 1});
	render_2d_line((float []){0, -h_len, 0}, (float []){0, h_len, 0}, (float []){1, 1, 1});
}

typedef struct s_render_line
{
	float		vertices[6];
	t_shader	shader;
	GLuint		vao;
	GLuint		vbo_pos;
}				t_render_line;

////////////////////////////////////
// 2D LINE
////////////////////////////////////

void	setup_2d_line(t_render_line *info)
{
	new_shader(&info->shader, ROOT_PATH"shaders/2d.vs", ROOT_PATH"shaders/2d.fs");

	glGenVertexArrays(1, &info->vao);
	glBindVertexArray(info->vao);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &info->vbo_pos);
}

void	render_2d_line(float *p1, float *p2, float *col)
{
	static t_render_line	info = {};
	static int				set = 0;

	if (!set)
	{
		setup_2d_line(&info);
		set = 1;
	}
	int i = 0;
	for (; i < 3; i++)
		info.vertices[i] = p1[i];
	for (; i < 6; i++)
		info.vertices[i] = p2[i - 3];

	glUseProgram(info.shader.program);
	glBindVertexArray(info.vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, &info.vertices[0], GL_DYNAMIC_DRAW);
	glUniform3fv(glGetUniformLocation(info.shader.program, "inColor"), 1, col);

	glDrawArrays(GL_LINES, 0, 2);

	int error = glGetError();
	if (error)
		LG_WARN("(%d)", error);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

////////////////////////////////////
// 3D LINE
////////////////////////////////////

void	setup_3d_line(t_render_line *info)
{
	new_shader(&info->shader, ROOT_PATH"shaders/3d.vs", ROOT_PATH"shaders/3d.fs");

	glGenVertexArrays(1, &info->vao);
	glBindVertexArray(info->vao);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &info->vbo_pos);
}

/*
 * 'p1' and 'p2' should be coordinates in world space already;
*/
void	render_3d_line(float *p1, float *p2, float *col, float *view_mat, float *project_mat)
{
	static t_render_line	info = {};
	static int				set = 0;

	if (!set)
	{
		setup_3d_line(&info);
		set = 1;
	}
	int i = 0;
	for (; i < 3; i++)
		info.vertices[i] = p1[i];
	for (; i < 6; i++)
		info.vertices[i] = p2[i - 3];

	glUseProgram(info.shader.program);
	glBindVertexArray(info.vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, &info.vertices[0], GL_DYNAMIC_DRAW);

	glUniform3fv(glGetUniformLocation(info.shader.program, "inColor"), 1, col);
	glUniformMatrix4fv(glGetUniformLocation(info.shader.program, "view"), 1, GL_FALSE, &view_mat[0]);
	glUniformMatrix4fv(glGetUniformLocation(info.shader.program, "projection"), 1, GL_FALSE, &project_mat[0]);

	glDrawArrays(GL_LINES, 0, 2);

	int error = glGetError();
	if (error)
		LG_WARN("(%d)", error);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}