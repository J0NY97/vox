#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNorm;
layout (location = 4) in mat4 aModel;
// 5, 6, 7 reserved for aModel
layout (location = 8) in int aTextureID;

out	vec3 inColor;
out	vec2 inTex;
out	vec3 inNorm;
out	vec3 FragPos;
out vec3 inViewPos;

uniform mat4 view;
uniform mat4 projection;

int		texture_in_pack = 2;
float	per_tex_w = 1.0f / texture_in_pack;

int		nth_texture = 0;
float	this_texture = nth_texture * per_tex_w;

void main()
{
	FragPos = vec3(aModel * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(FragPos, 1.0);
	inColor = aCol;
//	inTex = aTex;
	inTex =	vec2(aTex.x * per_tex_w + (aTextureID * per_tex_w), aTex.y);
	inNorm = aNorm;
}
