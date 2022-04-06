#version 330 core

in vec2 inTex;
in float light;

out vec4 gl_FragColor;

uniform sampler2D aTexture;

void main()
{
	gl_FragColor = light * texture(aTexture, inTex);
}
