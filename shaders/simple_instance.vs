#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in mat4 aModel;
// 3, 4, 5 reserved for aModel
layout (location = 6) in int aTextureID;

out	vec2 inTex;

uniform mat4 view;
uniform mat4 projection;

int		texture_in_pack = 4;
float	per_tex_w = 1.0f / texture_in_pack;

int		nth_texture = 0;
float	this_texture = nth_texture * per_tex_w;

void main()
{
	vec3 FragPos = vec3(aModel * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(FragPos, 1.0);

	inTex =	vec2(aTex.x * per_tex_w + (aTextureID * per_tex_w), aTex.y);
}
