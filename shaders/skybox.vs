#version 330 core

layout (location = 0) in vec3 aPos;

out	vec3 inTex;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	inTex = vec3(aPos.x, -aPos.y, aPos.z);
	vec4 pos = projection * view * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}