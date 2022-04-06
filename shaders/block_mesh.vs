#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in int aTextureID; // 16 bits texture, 4 bits uv, 8 bits of light;

uniform vec3 chunkPos;
uniform mat4 view;
uniform mat4 projection;

vec2	uvs[4] = vec2[4](
	vec2(1.0f, 1.0f),
	vec2(1.0f, 0.0f),
	vec2(0.0f, 0.0f),
	vec2(0.0f, 1.0f)
);

out vec2 inTex;
out float light;

int		texture_in_pack_w = 24;
int		texture_in_pack_h = 16;
float	per_tex_w = 1.0f / texture_in_pack_w;
float	per_tex_h = 1.0f / texture_in_pack_h;

void	main()
{
	int	uv_index = (aTextureID >> 16) & 0x0000000F;
	int	texture_index = aTextureID & 0x0000FFFF;
	int	light_index = (aTextureID >> 20) & 0x000000FF;

	int texture_index_x = texture_index % texture_in_pack_w;
	int texture_index_y = texture_index / texture_in_pack_w;

	inTex =	vec2(uvs[uv_index].x * per_tex_w + (texture_index_x * per_tex_w),
				uvs[uv_index].y * per_tex_h + (texture_index_y * per_tex_h));
	light = light_index / 100.0f;
	gl_Position = projection * view * vec4(aPos + chunkPos, 1.0);
}