#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 4) out;

out vec3 inColor;

void main()
{
	vec4 offset;

	offset = vec4(-1.0, 0.5, 0, 0);
	gl_Position = offset + gl_in[0].gl_Position;
	inColor = vec3(1, 1, 1);
	EmitVertex();

	offset = vec4(1.0, 0.5, 0, 0);
	gl_Position = offset + gl_in[0].gl_Position;
	inColor = vec3(1, 1, 1);
	EmitVertex();

	EndPrimitive();

	offset = vec4(0.5, -1.0, 0, 0);
	gl_Position = offset + gl_in[0].gl_Position;
	inColor = vec3(1, 1, 1);
	EmitVertex();

	offset = vec4(0.5, 1.0, 0, 0);
	gl_Position = offset + gl_in[0].gl_Position;
	inColor = vec3(1, 1, 1);
	EmitVertex();

	EndPrimitive();
}
