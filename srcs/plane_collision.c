#include "shaderpixel.h"

float	ray_plane_intersect(float *orig, float *dir, float *pos, float *norm)
{
	float t[3];
	float a = vec3_dot(dir, norm);
	if (a < 0.001)
		return (0);
	float b = vec3_dot(vec3_sub(t, pos, orig), norm) / a;
	if (b <= 0.001)
		return (0);
	return (b);
}
