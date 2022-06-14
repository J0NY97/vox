#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

uniform mat4 view;
uniform mat4 projection;

out vec2 inTex;

void	main()
{
	inTex = aTex;
	gl_Position = projection * view * aPos;
}