#version 330 core

in vec2 inTex;
in float light;

out vec4 gl_FragColor;

uniform sampler2D aTexture;

void main()
{
	vec4	texCol = texture(aTexture, inTex);
	gl_FragColor = vec4(light * texCol.rgb, texCol.a);
	if (gl_FragColor.a == 0)
		discard ;
}
