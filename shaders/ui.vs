#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aTex;

out vec3 inTex;

void	main()
{
	inTex = aTex;
	gl_Position = aPos.xy;
}