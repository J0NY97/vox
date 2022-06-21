#include "shaderpixel.h"

void	new_crosshair_shader(GLuint *shader)
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

	*shader = glCreateProgram();
	glAttachShader(*shader, vs_shader);
	glAttachShader(*shader, gs_shader);
	glAttachShader(*shader, fs_shader);
	glLinkProgram(*shader);

	check_program_errors(*shader);
}

void	render_crosshair(void)
{
	GLint	viewport[4];
	float	col[3] = {0.90, 0.90, 0.90};
	int		error;

	error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	glGetIntegerv(GL_VIEWPORT, viewport);
	glLineWidth(2);
	float	len = 0.009;
	float	h_len = (viewport[2] * len) / viewport[3];
	render_2d_line((float []){-len, 0, 0}, (float []){len, 0, 0}, col);
	render_2d_line((float []){0, -h_len, 0}, (float []){0, h_len, 0}, col);
	glLineWidth(1);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

typedef struct s_render_line
{
	float	vertices[6];
	GLuint	shader;
	GLuint	vao;
	GLuint	vbo_pos;
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

	glUseProgram(info.shader);
	glBindVertexArray(info.vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, &info.vertices[0], GL_DYNAMIC_DRAW);
	glUniform3fv(glGetUniformLocation(info.shader, "inColor"), 1, col);

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

	glUseProgram(info.shader);
	glBindVertexArray(info.vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6, &info.vertices[0], GL_DYNAMIC_DRAW);

	glUniform3fv(glGetUniformLocation(info.shader, "inColor"), 1, col);
	glUniformMatrix4fv(glGetUniformLocation(info.shader, "view"), 1, GL_FALSE, &view_mat[0]);
	glUniformMatrix4fv(glGetUniformLocation(info.shader, "projection"), 1, GL_FALSE, &project_mat[0]);

	glDrawArrays(GL_LINES, 0, 2);

	int error = glGetError();
	if (error)
		LG_WARN("(%d)", error);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

//////////////////////////////////
// 3d rectangle
//////////////////////////////////

/*
 * 'p1' & 'p2' should be the 2 opposing points in a rectangle; (min & max);
*/
void	render_3d_rectangle(float *p1, float *p2, float *col, float *view_mat, float *proj_mat)
{
	float	pmin[3];
	float	pmax[3];

	pmin[0] = min(p1[0], p2[0]);
	pmin[1] = min(p1[1], p2[1]);
	pmin[2] = min(p1[2], p2[2]);

	pmax[0] = max(p1[0], p2[0]);
	pmax[1] = max(p1[1], p2[1]);
	pmax[2] = max(p1[2], p2[2]);
	
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	render_3d_line(
		(float []){pmin[0], pmin[1], pmin[2]},
		(float []){pmax[0], pmin[1], pmin[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmax[0], pmin[1], pmin[2]},
		(float []){pmax[0], pmax[1], pmin[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmax[0], pmax[1], pmin[2]},
		(float []){pmin[0], pmax[1], pmin[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmin[0], pmax[1], pmin[2]},
		(float []){pmin[0], pmin[1], pmin[2]},
		col, view_mat, proj_mat);

	render_3d_line(
		(float []){pmin[0], pmin[1], pmax[2]},
		(float []){pmax[0], pmin[1], pmax[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmax[0], pmin[1], pmax[2]},
		(float []){pmax[0], pmax[1], pmax[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmax[0], pmax[1], pmax[2]},
		(float []){pmin[0], pmax[1], pmax[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmin[0], pmax[1], pmax[2]},
		(float []){pmin[0], pmin[1], pmax[2]},
		col, view_mat, proj_mat);

	render_3d_line(
		(float []){pmin[0], pmin[1], pmin[2]},
		(float []){pmin[0], pmin[1], pmax[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmax[0], pmin[1], pmin[2]},
		(float []){pmax[0], pmin[1], pmax[2]},
		col, view_mat, proj_mat);

	render_3d_line(
		(float []){pmin[0], pmax[1], pmin[2]},
		(float []){pmin[0], pmax[1], pmax[2]},
		col, view_mat, proj_mat);
	render_3d_line(
		(float []){pmax[0], pmax[1], pmin[2]},
		(float []){pmax[0], pmax[1], pmax[2]},
		col, view_mat, proj_mat);
}