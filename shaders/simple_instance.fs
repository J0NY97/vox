#version 330 core

in vec3 inColor;
in vec3 inNorm;
in vec2 inTex;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D textures[1];

void main()
{
	FragColor = texture(textures[0], inTex);
}
