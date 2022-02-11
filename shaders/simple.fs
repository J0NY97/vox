#version 330 core

in vec3 inColor;
in vec3 inNorm;
in vec2 inTex;
//in vec3 FragPos;

out vec4 FragColor;

int max_texture_amount = 1;
uniform int show_normal_map = 1;
uniform int texture_amount = 0;
uniform sampler2D textures[1];

void main()
{
	if (texture_amount > 0)
	{
		FragColor = texture(textures[0], inTex);
	// When we have the ability to have multiple textures per mesh;
	//	for (int i = 1; i < texture_amount && i < max_texture_amount; i++)
	//		FragColor *= texture(textures[i], inTex);
	}
	else if (show_normal_map == 1)
		FragColor = vec4(inNorm, 1);
	else
		FragColor = vec4(inColor, 1);
}
