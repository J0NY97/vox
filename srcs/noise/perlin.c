#include "stdio.h"

static int fastfloor(double x)
{
	return x > 0 ? (int)x : (int)x - 1;
}

int	g_p[] = {
	151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
	36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247, 120,
	234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71,
	134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133,
	230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161,
	1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135, 130,
	116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250,
	124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227,
	47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44,
	154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
	108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34,
	242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14,
	239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121,
	50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243,
	141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160, 137, 91, 90, 15, 131,
	13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37,
	240, 21, 10, 23, 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252,
	219, 203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125,
	136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158,
	231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245,
	40, 244, 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187,
	208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198,
	173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126,
	255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223,
	183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167,
	43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185,
	112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179,
	162, 241, 81, 51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199,
	106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236,
	205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156,
	180
};

float	fade(float t)
{
	return (t * t * t * (t * (t * 6 - 15) + 10));
}

float	grad(int hash, float x, float y, float z)
{
	int	h = hash & 15;
	double	u = h < 8 ? x : y;
	double	v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float	grad2d(int hash, float x, float y)
{
	return ((hash & 1) ? x : -x) + ((hash & 2) ? y : -y);
}

float	lerp(float t, float a, float b)
{
	return (a + t * (b - a));
}

// YOINKKELI KOINKKEL
float	noise2d(float x, float y)
{
	int xi = fastfloor(x) & 255;
	int yi = fastfloor(y) & 255;
	int g1 = g_p[g_p[xi] + yi];
	int g2 = g_p[g_p[xi + 1] + yi];
	int g3 = g_p[g_p[xi] + yi + 1];
	int g4 = g_p[g_p[xi + 1] + yi + 1];

	float xf = x - fastfloor(x);
	float yf = y - fastfloor(y);

	float d1 = grad2d(g1, xf, yf);
	float d2 = grad2d(g2, xf - 1, yf);
	float d3 = grad2d(g3, xf, yf - 1);
	float d4 = grad2d(g4, xf - 1, yf - 1);

	float u = fade(xf);
	float v = fade(yf);

	float x1Inter = lerp(u, d1, d2);
	float x2Inter = lerp(u, d3, d4);
	float yInter = lerp(v, x1Inter, x2Inter);

	return (yInter);
}
 
// Noise 2d octave
float	noise2d_octave(float x, float y, float amplitude, float frequency, int octaves, float persistence, float lacunarity)
{
	float	value;

	value = 0;
	for (int i = 0; i < octaves; i++)
	{
		value += amplitude * noise2d(x * frequency, y * frequency);
		amplitude *= persistence;
		frequency *= lacunarity;
	}
	return (value);
}

float	noise3d(float x, float y, float z)
{
	static int times_called = -1;
	times_called += 1;
	if (times_called % 100000 == 0)
		printf("noise3d() times called %d\n", times_called);

	int	X = fastfloor(x) & 255;
	int	Y = fastfloor(y) & 255;
	int	Z = fastfloor(z) & 255;

	x -= fastfloor(x);                                
	y -= fastfloor(y);                                
	z -= fastfloor(z);

	float	u = fade(x);
	float	v = fade(y);
	float	w = fade(z);

	int	A = g_p[X  ]+Y;
	int	AA = g_p[A]+Z;
	int	AB = g_p[A+1]+Z;
	int	B = g_p[X+1]+Y;
	int	BA = g_p[B]+Z;
	int	BB = g_p[B+1]+Z;

	return (lerp(w, lerp(v,
		lerp(u, grad(g_p[AA], x, y, z), grad(g_p[BA], x - 1, y, z)),
		lerp(u, grad(g_p[AB  ], x, y - 1, z), grad(g_p[BB], x - 1, y - 1, z))),
		lerp(v,
			lerp(u, grad(g_p[AA + 1], x, y, z - 1),
				grad(g_p[BA + 1], x - 1, y, z - 1)), 
			lerp(u, grad(g_p[AB + 1], x, y - 1, z - 1),
				grad(g_p[BB + 1], x - 1, y - 1, z - 1)))));
}

/*
 * 'persistence' : range 0 - 1, controls how quickly each octave die out;
 * 'lacucaracha' : > 1, controls how much fine scale each subsequent octave should use;
*/
float	noise3d_octave(float x, float y, float z, float amplitude, float frequency, int octaves, float persistence, float lacunarity)
{
	float	value;

	value = 0;
	for (int i = 0; i < octaves; i++)
	{
		value += amplitude * noise3d(x * frequency, y * frequency, z * frequency);
		amplitude *= persistence;
		frequency *= lacunarity;
	}
	return (value);
}