#version 330 core

in vec2 bTex;

out vec4 FragColor;

uniform sampler2D bTexture;

void	main()
{
	FragColor = texture(bTexture, bTex);
//	FragColor = vec4(1, 0, 1, 1);
}