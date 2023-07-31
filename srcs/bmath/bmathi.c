#include "bmath.h"
#include "libpf.h"

/// @brief from 'vN' array print 'N' amount;
/// @param prefix_str 
/// @param vN 
/// @param N 
void vNi_string(char *prefix_str, int *vN, int N)
{
	printf("%s { ", prefix_str);
	for (int i = 0; i < N; i++)
	 	printf("%d ", vN[i]);
	printf("}\n");
}

/// VECTOR 3 ///

int	*v3i_new(int *res, int x, int y, int z)
{
	res[0] = x;
	res[1] = y;
	res[2] = z;
	return (res);
}

int	*v3i_add(int *res, int *v0, int *v1)
{
	res[0] = v0[0] + v1[0];
	res[1] = v0[1] + v1[1];
	res[2] = v0[2] + v1[2];
	return (res);
}

int	v3i_dist(int *v0, int *v1)
{
	int	a;
	int	b;
	int	c;

	a = (v1[0] - v0[0]) * (v1[0] - v0[0]);
	b = (v1[1] - v0[1]) * (v1[1] - v0[1]);
	c = (v1[2] - v0[2]) * (v1[2] - v0[2]);
	return (sqrt(a + b + c));
}

int	v3i_dist_sqrd(int *v0, int *v1)
{
	return ((v0[0] - v1[0]) * (v0[0] - v1[0]) +
		(v0[1] - v1[1]) * (v0[1] - v1[1]) +
		(v0[2] - v1[2]) * (v0[2] - v1[2]));
}

void	v3i_string(char *str, int *v)
{
	ft_printf("%s { %d %d %d }\n", str, v[0], v[1], v[2]);
}