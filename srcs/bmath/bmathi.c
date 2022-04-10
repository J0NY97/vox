#include "bmath.h"

int	*vec3i_new(int *res, int x, int y, int z)
{
	res[0] = x;
	res[1] = y;
	res[2] = z;
	return (res);
}

int	vec3i_dist(int *v0, int *v1)
{
	int	a;
	int	b;
	int	c;

	a = (v1[0] - v0[0]) * (v1[0] - v0[0]);
	b = (v1[1] - v0[1]) * (v1[1] - v0[1]);
	c = (v1[2] - v0[2]) * (v1[2] - v0[2]);
	return (sqrt(a + b + c));
}