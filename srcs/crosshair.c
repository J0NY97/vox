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

/*
void	render_crosshair(t_shader *shader)
{
	float	vertices[] = {
		0, 0, 0
	};

	GLuint	vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);

	GLuint	vbo;
	glGenBuffers(1, &vbo);



	glUseProgram(shader->program);
}
*/

typedef struct s_render_line
{
	float		vertices[6];
	t_shader	shader;
	GLuint		vao;
	GLuint		vbo_pos;
}				t_render_line;

void	setup_2d_line(t_render_line *info)
{
	new_shader(&info->shader, ROOT_PATH"shaders/2d.vs", ROOT_PATH"shaders/2d.fs");

	glGenVertexArrays(1, &info->vao);
	glBindVertexArray(info->vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

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
	glUseProgram(info.shader.program);
	int i = 0;
	for (; i < 3; i++)
		info.vertices[i] = p1[i];
	for (; i < 6; i++)
		info.vertices[i] = p2[i - 3];

	for (int j = 0; j < 6; j++)
		ft_printf("%.2f ", info.vertices[j]);
	ft_printf("\n");
	for (int j = 0; j < 3; j++)
		ft_printf("%.2f ", col[j]);
	ft_printf("\n");

	glBindVertexArray(info.vao);
	glBindBuffer(GL_ARRAY_BUFFER, info.vbo_pos);
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
