#include "noise.h"

/*
 * TODO : take in noise settings, for frequency, octaves, amplitude osv....
 * TODO :  seed should be given in the settings aswell;
 *  
 * NOTE : make 'noise->map = NULL';
*/
void	noise_create(t_noise *noise, int w, int h, int x_offset, int y_offset, int seed)
{
	float	frequency = 0.01f;
	int		octaves = 4;
	float	amplitude = 20.0f;
	float	persistence = 0.5f;
	float	lacunarity = 2.0f;

	noise->width = w;
	noise->height = h;
	if (!noise->map)
		noise->map = malloc(sizeof(float) * (noise->width * noise->height));
	for (int x = 0; x < noise->width; x++)
	{
		for (int y = 0; y < noise->height; y++)
		{
			noise->map[x * noise->width + y] =
				noise3d_octave(x + x_offset, seed, y + y_offset,
					amplitude, frequency, octaves, persistence, lacunarity);
		}
	}
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