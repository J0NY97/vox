#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aCol;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNorm;

out	vec3 inColor;
out	vec2 inTex;
out	vec3 inNorm;
out	vec3 FragPos;
out vec3 inViewPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform	vec3 aViewPos;

void main()
{
	FragPos = vec3(model * vec4(aPos, 1.0));
	gl_Position = projection * view * vec4(FragPos, 1.0);
	inColor = aCol;
	inTex = aTex;
	inNorm = aNorm;
	inViewPos = aViewPos;
}
