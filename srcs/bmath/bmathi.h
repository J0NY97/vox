#ifndef BMATHI_H
# define BMATHI_H

# include "bmath.h"

// VEC3I
int		*vec3i_new(int *res, int x, int y, int z);
int		*vec3i_add(int *res, int *v0, int *v1);
int		vec3i_dist(int *v0, int *v1);
int		vec3i_dist_sqrd(int *v0, int *v1);
void	vec3i_string(char *str, int *v);

#endif