#include "shaderpixel.h"

int	random_number(int x, int y)
{
	return (rand() % max(1, (x * y)));
}


/* Function to linearly interpolate between a0 and a1
 * Weight w should be in the range [0.0, 1.0]
 */
float interpolate(float a0, float a1, float w) {
    /* // You may want clamping by inserting:
     * if (0.0 > w) return a0;
     * if (1.0 < w) return a1;
     */
	//return (a1 - a0) * w + a0;
    /* // Use this cubic interpolation [[Smoothstep]] instead, for a smooth appearance:
	 */
	//return (a1 - a0) * (3.0 - w * 2.0) * w * w + a0;
	/*
     *
     * // Use [[Smootherstep]] for an even smoother result with a second derivative equal to zero on boundaries:
     */
	return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

/* Create pseudorandom direction vector
 */
float	*randomGradient(float *res_v2, int ix, int iy, unsigned int seed)
{
	// No precomputed gradients mean this works for any number of grid coordinates
	const unsigned int	w = 8 * sizeof(unsigned);
	const unsigned int	s = w / 2; // rotation width

	unsigned int	a = ix;
	unsigned int	b = iy;
	//a *= 3284157443;
	//b *= 1911520717;
	//a *= 2048419325;
	a *= seed;
	b ^= (a << s) | (a >> (w - s));
	b *= seed;
	a ^= (b << s) | (b >> (w - s));
	a *= seed;

	float	random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
	float	v[VEC2_SIZE];
	v[0] = cos(random);
	v[1] = sin(random);

	res_v2[0] = v[0];
	res_v2[1] = v[1];
	return (res_v2);
}

// Computes the dot product of the distance and gradient vectors.
float	dotGridGradient(int ix, int iy, float x, float y, unsigned int seed)
{
	// Get gradient from integer coordinates
	float	gradient[VEC2_SIZE];
	randomGradient(gradient, ix, iy, seed);

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;

	// Compute the dot-product
	return (dx * gradient[0] + dy * gradient[1]);
}

// Compute Perlin noise at coordinates x, y
float perlin(float x, float y, unsigned int seed)
{
    // Determine grid cell coordinates
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x - (float)x0;
    float sy = y - (float)y0;

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, x, y, seed);
    n1 = dotGridGradient(x1, y0, x, y, seed);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, x, y, seed);
    n1 = dotGridGradient(x1, y1, x, y, seed);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return (value);
}

/////////////////////
// 3D
/////////////////////

/* Create pseudorandom direction vector
 */
float	*randomGradient3(float *res_v3, int ix, int iy, int iz, unsigned int seed)
{
	// No precomputed gradients mean this works for any number of grid coordinates
	const unsigned int	w = 8 * sizeof(unsigned);
	const unsigned int	s = w / 2; // rotation width

	unsigned int	a = ix;
	unsigned int	b = iy;
	unsigned int	c = iz;
	//a *= 3284157443;
	//b *= 1911520717;
	//a *= 2048419325;
	a *= seed;
	b ^= (a << s) | (a >> (w - s));

	b *= seed;
	a ^= (b << s) | (b >> (w - s));

	c *= seed;
	c ^= (a << s) | (a >> (w - s));

	a *= seed;

	float	random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
	float	v[VEC3_SIZE];
	v[0] = cos(random);
	v[1] = sin(random);
	v[2] = tan(random);

	res_v3[0] = v[0];
	res_v3[1] = v[1];
	res_v3[2] = v[2];
	return (res_v3);
}
// Computes the dot product of the distance and gradient vectors.
float	dotGridGradient3(int ix, int iy, int iz, float x, float y, float z, unsigned int seed)
{
	// Get gradient from integer coordinates
	float	gradient[VEC3_SIZE];
	randomGradient3(gradient, ix, iy, iz, seed);

	// Compute the distance vector
	float dx = x - (float)ix;
	float dy = y - (float)iy;
	float dz = z - (float)iz;

	// Compute the dot-product
	return (dx * gradient[0] + dy * gradient[1] + dz * gradient[2]);
}

float	perlin3(float x, float y, float z, unsigned int seed)
{ 
	float	ab = perlin(x, y, seed);
	float	bc = perlin(y, z, seed);
	float	ac = perlin(x, z, seed);

	float	ba = perlin(y, x, seed);
	float	cb = perlin(z, y, seed);
	float	ca = perlin(z, x, seed);

	float	abc = ab + bc + ac + ba + cb + ca;
	return (abc / 6.0f);
}