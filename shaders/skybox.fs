#version 330 core
in vec3 inTex;

uniform	samplerCube skybox;

out vec4 FragColor;

void main()
{
	FragColor = texture(skybox, inTex);
}
