#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 bTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void	main()
{
	vec3 FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(FragPos, 1.0);
	bTex = aTex;
}