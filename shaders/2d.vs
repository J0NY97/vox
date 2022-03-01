#version 330 core

in vec3 inPos;

uniform	vec3 inColor;

out vec3 outColor;

void main()
{
	gl_Position = vec4(inPos, 1);
	outColor = inColor;
}
