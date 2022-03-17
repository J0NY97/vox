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
# define QUAT_SIZE 4

// VEC3
float	*new_vec3(float *src, float x, float y, float z);
float	*vec3_new(float *src, float x, float y, float z);
float	*vec3_assign(float *dest, float *src);
void	vec3_string(char *str, float *v);
float	*vec3_add(float *result, float *v0, float *v1);
float	*vec3_sub(float *result, float *v0, float *v1);
float	*vec3_multiply_f(float *result, float *v0, float f);
float	*vec3_normalize(float *result, float *v0);
float	*vec3_cross(float *result, float *v0, float *v1);
float	vec3_dot(float *v0, float *v1);
float	vec3_dist(float *v0, float *v1);
float	*vec3_multiply_mat3(float *res, float *v, float *m);
float	vec3_length(float *v0);
float	*vec3_to_vec4(float *res, float *v3);

// VEC4
float	*vec4_new(float *res, float x, float y, float z, float w);
void	vec4_string(char *str, float *v);
float	*vec4_multiply_mat4(float *res, float *v, float *m);
float	*vec4_to_vec3(float *res, float *v4);

// HELP
float	to_radians(float degrees);
float	to_degrees(float radians);

// MAT3
float	*mat3_identity(float *result);
float	*mat3_assign(float *result, float *m0);
float	*mat4_to_mat3(float *res, float *m4);
void	mat3_string(char *str, float *m);
float	*mat3_inverse(float *res, float *m);

// MAT4
float	*mat4_assign(float *result, float *m0);
void	mat4_string(char *str, float *m);
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
float	*mat4_inverse(float *result, float *m0);

// QUAT
float	*quat_new(float *res, float x, float y, float z, float w);
float	*euler_to_quat(float *res, float roll, float pitch, float yaw);
float	*mat4_rotation_quat(float *res, float *q0);

#endif
