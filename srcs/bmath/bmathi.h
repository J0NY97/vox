#ifndef BMATHI_H
# define BMATHI_H

# include "bmath.h"

// VEC3I
int		*v3i_new(int *res, int x, int y, int z);
int		*v3i_add(int *res, int *v0, int *v1);
int		v3i_dist(int *v0, int *v1);
int		v3i_dist_sqrd(int *v0, int *v1);
void	v3i_string(char *str, int *v);

#endif