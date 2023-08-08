#include "noise.h"
#include "fastnoise.h"

void noise_create_from_settings(t_noise *noise, t_noise_settings *settings)
{
	fnl_state fnlState;
	fnlState.seed = 1337;
    fnlState.frequency = 0.01f;
    fnlState.octaves = 4;
    fnlState.lacunarity = 1.0f;
    fnlState.gain = 10.0f;
    fnlState.weighted_strength = 2.5f;

	if (!noise->map)
		noise_new(noise, settings->width, settings->height);
	for (int x = 0; x < noise->width; x++)
	{
		for (int y = 0; y < noise->height; y++)
		{
			/* USING FAST NOISE 2D */
			/*
			noise->map[x * noise->width + y] = fnlGetNoise2D_Octave(&fnlState,
					x + settings->x_offset, y + settings->y_offset);
					*/
			/* USING SIMPLEX 2D */
			/*
					*/
			noise->map[x * noise->width + y] = simplex_noise_2d_octave(
					x + settings->x_offset, y + settings->y_offset,
					settings->amplitude, settings->frequency,
					settings->octaves, settings->persistence,
					settings->lacunarity);
			/* USING PERLIN NOISE 2D */
			/*
			noise->map[x * noise->width + y]
				= noise2d_octave(x + settings->x_offset, y + settings->y_offset,
					settings->amplitude, settings->frequency, settings->octaves, settings->persistence, settings->lacunarity);
					*/
			/* USING PERLIN NOISE 3D
			noise->map[x * noise->width + y] =
				noise3d_octave(x + x_offset, seed, y + y_offset,
					amplitude, frequency, octaves, persistence, lacunarity);
					*/
		}
	}
}

void noise_settings_default(t_noise_settings *settings)
{
	// These settings kind have to be set manually..;
	settings->width = 0;
	settings->height = 0;
	settings->x_offset = 0;
	settings->y_offset = 0;
	settings->seed = 0;

	// ..but not these;
	settings->frequency = 0.01f;
	settings->octaves = 4;
	settings->amplitude = 20.0f;
	settings->persistence = 0.5f;
	settings->lacunarity = 2.0f;
}

/// @brief Allocates the needed stuff;
/// @param noise 
/// @param w 
/// @param h 
void noise_new(t_noise *noise, int w, int h)
{
	noise->width = w;
	noise->height = h;
	noise->map = malloc(sizeof(float) * w * h);
}

void	noise_free(t_noise *noise)
{
	if (noise->map)
		free(noise->map);
}

/*
 * TODO : Check that the x and y are inside the map;
*/
float	noise_get_value(t_noise *noise, int x, int y)
{
	return (noise->map[x * noise->width + y]);
}