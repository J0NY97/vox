#include "shaderpixel.h"

int	create_shader(GLuint *shader, const char *content, GLenum type)
{
	GLuint	shdr;
	char	buffer[512];
	GLint	status;

	shdr = glCreateShader(type);
	glShaderSource(shdr, 1, &content, NULL);
	glCompileShader(shdr);
	glGetShaderiv(shdr, GL_COMPILE_STATUS, &status);
	glGetShaderInfoLog(shdr, 512, NULL, buffer);
	if (status != GL_TRUE)
	{
		LG_WARN("Compilation error => %s", buffer);
		return (0);
	}
	*shader = shdr;
	return (1);
}

void	check_program_errors(GLuint program)
{
	char	buffer[512];
	GLint	status;

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	glGetProgramInfoLog(program, 512, NULL, buffer);
	if (status != GL_TRUE)
		LG_WARN("Creation error => %s", buffer);
}

int	create_program(GLuint *result, GLuint vs, GLuint fs)
{
	GLuint	program;
	char	buffer[512];
	GLint	status;

	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	glGetProgramInfoLog(program, 512, NULL, buffer);
	if (status != GL_TRUE)
	{
		LG_WARN("Creation error => %s", buffer);
		return (0);
	}
	*result = program;
	return (1);
}

void	new_shader(t_shader *shader, const char *vs, const char *fs)
{
	char	*vs_cont;
	char	*fs_cont;
	GLuint	v_shader;
	GLuint	f_shader;
	int		error;

	error = glGetError();
	if (error)
		LG_ERROR("before this func <%s> <%s>", vs, fs);


	vs_cont = get_file_content((char *)vs);
	fs_cont = get_file_content((char *)fs);
	if (!vs_cont || !fs_cont)
	{
		if (!vs_cont)
			LG_WARN("Couldnt get file content <%s>", vs);
		else if (!fs_cont)
			LG_WARN("Couldnt get file content <%f>", fs);
		return ;
	}
	if (!create_shader(&v_shader, vs_cont, GL_VERTEX_SHADER))
	{
		LG_WARN("Couldnt create shaders from vertex shader content.");
		return ;
	}
	if (!create_shader(&f_shader, fs_cont, GL_FRAGMENT_SHADER))
	{
		LG_WARN("Couldnt create shaders from fragment shader content.");
		return ;
	}
	if (!create_program(&shader->program, v_shader, f_shader))
		LG_WARN("Couldnt create program.");
	free(vs_cont);
	free(fs_cont);
	glDeleteShader(v_shader);
	glDeleteShader(f_shader);

	error = glGetError();
	LG_INFO("new shader made. (%d)", error);
	if (error)
		LG_ERROR("<%s> <%s>", vs, fs);
}
