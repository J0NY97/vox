#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in int aTextureID; // 2 bytes nth texture, 2 bytes of uv index;

uniform vec3 chunkPos;
uniform mat4 view;
uniform mat4 projection;

vec2	uvs[4] = vec2[4](
	vec2(0.0f, 0.0f),
	vec2(0.0f, 1.0f),
	vec2(1.0f, 0.0f),
	vec2(1.0f, 1.0f)
);

out vec2 inTex;

int		texture_in_pack = 12;
float	per_tex_w = 1.0f / texture_in_pack;

void	main()
{
	int	uv_index = (aTextureID >> 16) & 0x0000FFFF;	
	int	texture_index = aTextureID & 0x0000FFFF;	

	inTex =	vec2(uvs[uv_index].x * per_tex_w + (texture_index * per_tex_w),
				uvs[uv_index].y);

	gl_Position = projection * view * vec4(aPos + chunkPos, 1.0);
}