#ifndef MATH_H
# define MATH_H

# include "math.h"
# include "stdio.h"

# define VEC2_SIZE 2
# define VEC3_SIZE 3
# define VEC4_SIZE 4
# define MAT2_SIZE 4
# define MAT3_SIZE 9
# define MAT4_SIZE 16

// VEC3
float	*new_vec3(float *src, float x, float y, float z);
float	*vec3_cpy(float *dest, float *src);
void	vec3_string(float *v);
float	*vec3_add(float *result, float *v0, float *v1);
float	*vec3_subtract(float *result, float *v0, float *v1);
float	*vec3_multiply_f(float *result, float *v0, float f);
float	*vec3_normalize(float *result, float *v0);
float	*vec3_cross(float *result, float *v0, float *v1);
float	vec3_dot(float *v0, float *v1);
float	vec3_distance(float *v0, float *v1);

// VEC4
float	*vec4_new(float *res, float x, float y, float z, float w);
float	*vec4_multiply_mat4(float *res, float *v, float *m);

// HELP
float	to_radians(float degrees);
float	to_degrees(float radians);

// MAT3
float	*mat3_identity(float *result);
float	*mat3_assign(float *result, float *m0);

// MAT4
float	*mat4_assign(float *result, float *m0);
void	mat4_string(float *m);
float	*mat4_identity(float *result);
float	*mat4_translate(float *result, float *m0, float *v0);
float	*mat4_scale(float *result, float *m0, float *v0);
float	*mat4_rotation_x(float *result, float f);
float	*mat4_rotation_y(float *result, float f);
float	*mat4_rotation_z(float *result, float f);
float	*mat4_rotation_axis(float *result, float *v0, float f);
float	*mat4_perspective_fov(float *result, float fov, float w, float h, float n, float f);
float	*mat4_look_at(float *result, float *position, float *target, float *up);
float	*mat4_multiply(float *result, float *m0, float *m1);

#endif
