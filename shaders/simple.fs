#version 330 core

in vec3 inColor;
in vec3 inNorm;
in vec2 inTex;
in vec3 FragPos;

out vec4 FragColor;

uniform int show_normal_map = 0;
uniform int texture_amount = 1;
uniform int useColor = 0;
uniform sampler2D textures[1];

void main()
{
	if (texture_amount > 0)
		FragColor = texture(textures[0], inTex);
	else if (show_normal_map == 1)
		FragColor = vec4(inNorm, 1);
	if (useColor == 1)
		FragColor = mix(FragColor, vec4(inColor, 1), 0.1);
}
