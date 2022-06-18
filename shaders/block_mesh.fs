#version 330 core

in vec2 inTex;
in float light;

in vec3 outPos;

out vec4 FragColor;

uniform sampler2D aTexture;
// TODO : maybe mix the colorTint with the fragColor;
uniform vec3 colorTint;

uniform vec2 fog_dist;

// Fog parameters;
float fog_maxdist = 144.0f;
float fog_mindist = 100.0f;
vec4  fog_colour = vec4(0.0, 193.0 / 255, 212.0 / 255, 0.0);

void main()
{
	// Calculate fog
	float dist = length(outPos.xyz);
//	float fog_factor = (fog_maxdist - dist) / (fog_maxdist - fog_mindist);
	float fog_factor = (fog_dist.x - dist) / (fog_dist.x - fog_dist.y);
	fog_factor = clamp(fog_factor, 0.0, 1.0);

	// Color
	vec4	texCol = texture(aTexture, inTex);
	FragColor = vec4(light * texCol.rgb * colorTint, texCol.a);

	// Final color & fog mix;
	FragColor = mix(fog_colour, FragColor, fog_factor);

	if (FragColor.a == 0)
		discard ;
}
