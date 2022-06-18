#ifndef BMATHF_H
# define BMATHF_H

# include "bmath.h"

# define _PI_180 0.01745329251
# define _180_PI 57.2957795131

// OTHER EXTRAS
int		mod(int a, int b);

// VEC3
float	*v3_new(float *src, float x, float y, float z);
float	*v3_assign(float *dest, float *src);
void	v3_string(char *str, float *v);
float	*v3_add(float *result, float *v0, float *v1);
float	*v3_sub(float *result, float *v0, float *v1);
float	*v3_multiply(float *res, float *v0, float *v1);
float	*v3_multiply_f(float *result, float *v0, float f);
float	*v3_divide_f(float *res, float *v0, float f);
float	*v3_normalize(float *result, float *v0);
float	*v3_cross(float *result, float *v0, float *v1);
float	v3_dot(float *v0, float *v1);
float	v3_dist(float *v0, float *v1);
float	v3_dist_sqrd(float *v0, float *v1);
float	*v3_multiply_m3(float *res, float *v, float *m);
float	v3_length(float *v0);
float	v3_length_sqrd(float *v0);
float	*v3_to_v4(float *res, float *v3);

// VEC4
float	*v4_new(float *res, float x, float y, float z, float w);
void	v4_string(char *str, float *v);
float	*v4_multiply_m4(float *res, float *v, float *m);
float	*v4_to_v3(float *res, float *v4);

// HELP
float	to_radians(float degrees);
float	to_degrees(float radians);

// MAT3
float	*m3_identity(float *result);
float	*m3_assign(float *result, float *m0);
float	*m4_to_m3(float *res, float *m4);
void	m3_string(char *str, float *m);
float	*m3_inverse(float *res, float *m);

// MAT4
float	*m4_assign(float *result, float *m0);
void	m4_string(char *str, float *m);
float	*m4_identity(float *result);
float	*m4_translate(float *result, float *m0, float *v0);
float	*m4_scale(float *result, float *m0, float *v0);
float	*m4_rotation_x(float *result, float f);
float	*m4_rotation_y(float *result, float f);
float	*m4_rotation_z(float *result, float f);
float	*m4_rotation_axis(float *result, float *v0, float f);
float	*m4_perspective_fov(float *result, float fov, float w, float h, float n, float f);
float	*m4_look_at(float *result, float *position, float *target, float *up);
float	*m4_multiply(float *result, float *m0, float *m1);
float	*m4_inverse(float *result, float *m0);

// QUAT
float	*quat_new(float *res, float x, float y, float z, float w);
float	*euler_to_quat(float *res, float roll, float pitch, float yaw);
float	*m4_rotation_quat(float *res, float *q0);

#endif
