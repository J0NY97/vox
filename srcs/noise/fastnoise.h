#ifndef FASTNOISE_H
# define FASTNOISE_H

typedef struct fnl_state
{
	int seed;
	float frequency;
    //fnl_noise_type noise_type;
    //fnl_rotation_type_3d rotation_type_3d;
    //fnl_fractal_type fractal_type;
    int octaves;
    float lacunarity;
    float gain;
    float weighted_strength;
    float ping_pong_strength;
    //fnl_cellular_distance_func cellular_distance_func;
    //fnl_cellular_return_type cellular_return_type;
    float cellular_jitter_mod;
    //fnl_domain_warp_type domain_warp_type;
    float domain_warp_amp;
}	fnl_state;

float	fnlGetNoise2D(fnl_state *state, float x, float y);
float	fnlGetNoise2D_Octave(fnl_state *state, float x, float y);

float	fnlGetNoise3D(fnl_state *state, float x, float y, float z);

#endif