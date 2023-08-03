#include "noise.h"

/*
 * TODO : take in noise settings, for frequency, octaves, amplitude osv....
 * TODO : seed should be given in the settings aswell;
 *  
 * NOTE : make 'noise->map = NULL';
*/
void	noise_create(t_noise *noise, int w, int h, int x_offset, int y_offset, int seed)
{
	t_noise_settings settings;

	settings.width = w;
	settings.height = h;
	settings.x_offset = x_offset;
	settings.y_offset = y_offset;

	settings.seed = seed;

	settings.frequency = 0.01f;
	settings.octaves = 4;
	settings.amplitude = 20.0f;
	settings.persistence = 0.5f;
	settings.lacunarity = 2.0f;

	noise_create_from_settings(noise, &settings);
}

void noise_create_from_settings(t_noise *noise, t_noise_settings *settings)
{
	if (!noise->map)
		noise_new(noise, settings->width, settings->height);
	for (int x = 0; x < noise->width; x++)
	{
		for (int y = 0; y < noise->height; y++)
		{
			/* USING SIMPLEX 2D */
			/*
			noise->map[x * noise->width + y]
				= simp_noise_2d_octave(x + x_offset, y + y_offset,
					amplitude, frequency, octaves, persistence, lacunarity);
					*/
			/* USING PERLIN NOISE 2D */
			noise->map[x * noise->width + y]
				= noise2d_octave(x + settings->x_offset, y + settings->y_offset,
					settings->amplitude, settings->frequency, settings->octaves, settings->persistence, settings->lacunarity);
			/* USING PERLIN NOISE 3D
			noise->map[x * noise->width + y] =
				noise3d_octave(x + x_offset, seed, y + y_offset,
					amplitude, frequency, octaves, persistence, lacunarity);
					*/
		}
	}
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