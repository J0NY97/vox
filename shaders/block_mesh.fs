#version 330 core

in vec2 inTex;

out vec4 gl_FragColor;

uniform sampler2D aTexture;

void main()
{
	gl_FragColor = texture(aTexture, inTex);
}
