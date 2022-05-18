#version 330 core

in vec3 inTex;

uniform sampler2D tex;

out vec4 FragColor;

void main()
{
	FragColor = texture(tex, inTex);
}