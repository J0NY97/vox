#include "bmath.h"

float	*new_vec3(float *src, float x, float y, float z)
{
	src[0] = x;
	src[1] = y;
	src[2] = z;
	return (src);
}

float	*vec3_cpy(float *dest, float *src)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	return (src);
}

void	vec3_string(float *v)
{
	printf("{ %f %f %f }\n", v[0], v[1], v[2]);
}

float *vec3_add(float *result, float *v0, float *v1)
{
	result[0] = v0[0] + v1[0];
	result[1] = v0[1] + v1[1];
	result[2] = v0[2] + v1[2];
	return (result);
}

float *vec3_subtract(float *result, float *v0, float *v1)
{
	result[0] = v0[0] - v1[0];
	result[1] = v0[1] - v1[1];
	result[2] = v0[2] - v1[2];
	return (result);
}

float *vec3_normalize(float *result, float *v0)
{
	float l = sqrt(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
	result[0] = v0[0] / l;
	result[1] = v0[1] / l;
	result[2] = v0[2] / l;
	return (result);
}

float *vec3_cross(float *result, float *v0, float *v1)
{
	float cross[VEC3_SIZE];
	cross[0] = v0[1] * v1[2] - v0[2] * v1[1];
	cross[1] = v0[2] * v1[0] - v0[0] * v1[2];
	cross[2] = v0[0] * v1[1] - v0[1] * v1[0];
	result[0] = cross[0];
	result[1] = cross[1];
	result[2] = cross[2];
	return (result);
}

float	vec3_dot(float *v0, float *v1)
{
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

float	*vec3_multiply_f(float *result, float *v0, float f)
{
	result[0] = v0[0] * f;
	result[1] = v0[1] * f;
	result[2] = v0[2] * f;
	return result;
}

float	vec3_distance(float *v0, float *v1)
{
	float	a;
	float	b;
	float	c;

	a = (v1[0] - v0[0]) * (v1[0] - v0[0]);
	b = (v1[1] - v0[1]) * (v1[1] - v0[1]);
	c = (v1[2] - v0[2]) * (v1[2] - v0[2]);
	return (sqrt(a + b + c));
}

float	vec3_length(float *v0)
{
	return sqrt(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
}

float	*vec3_multiply_mat3(float *res, float *v, float *m)
{
	float	xyz[VEC3_SIZE];

	xyz[0] = v[0];
	xyz[1] = v[1];
	xyz[2] = v[2];
	res[0] = m[0] * xyz[0] + m[3] * xyz[1] + m[6] * xyz[2];
	res[1] = m[1] * xyz[0] + m[4] * xyz[1] + m[7] * xyz[2];
	res[2] = m[2] * xyz[0] + m[5] * xyz[1] + m[8] * xyz[2];
	return (res);
}

float	*vec4_to_vec3(float *res, float *v4)
{
	res[0] = v4[0];
	res[1] = v4[1];
	res[2] = v4[2];
	return (res);
}

float	*mat4_to_mat3(float *res, float *m4)
{
	res[0] = m4[0];
	res[1] = m4[1];
	res[2] = m4[2];
	res[3] = m4[4];
	res[4] = m4[5];
	res[5] = m4[6];
	res[6] = m4[8];
	res[7] = m4[9];
	res[8] = m4[10];
	return (res);
}

//////////////////////////
// VEC4
//////////////////////////

float	*vec4_new(float *res, float x, float y, float z, float w)
{
	res[0] = x;
	res[1] = y;
	res[2] = z;
	res[3] = w;
	return (res);
}

float	*vec4_multiply_mat4(float *res, float *v, float *m)
{
	float	xyzw[VEC4_SIZE];

	xyzw[0] = v[0];
	xyzw[1] = v[1];
	xyzw[2] = v[2];
	xyzw[3] = v[3];
	res[0] = m[0] * xyzw[0] + m[4] * xyzw[1] + m[8] * xyzw[2] + m[12] * xyzw[3];
	res[1] = m[1] * xyzw[0] + m[5] * xyzw[1] + m[9] * xyzw[2] + m[13] * xyzw[3];
	res[2] = m[2] * xyzw[0] + m[6] * xyzw[1] + m[10] * xyzw[2] + m[14] * xyzw[3];
	res[3] = m[3] * xyzw[0] + m[7] * xyzw[1] + m[11] * xyzw[2] + m[15] * xyzw[3];
	return (res);
}

float	to_radians(float degrees)
{
	return (degrees * M_PI / 180.0f);
}

float	to_degrees(float radians)
{
	return (radians * 180.0f / M_PI);
}

float	*mat3_identity(float *result)
{
	result[0] = 1.0f;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
	result[4] = 1.0f;
	result[5] = 0.0f;
	result[6] = 0.0f;
	result[7] = 0.0f;
	result[8] = 1.0f;
	return (result);
}

float	*mat3_assign(float *result, float *m0)
{
	result[0] = m0[0];
	result[1] = m0[1];
	result[2] = m0[2];
	result[3] = m0[3];
	result[4] = m0[4];
	result[5] = m0[5];
	result[6] = m0[6];
	result[7] = m0[7];
	result[8] = m0[8];
	return (result);
}

float	*mat4_identity(float *result)
{
	result[0] = 1.0f;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
	result[4] = 0.0f;
	result[5] = 1.0f;
	result[6] = 0.0f;
	result[7] = 0.0f;
	result[8] = 0.0f;
	result[9] = 0.0f;
	result[10] = 1.0f;
	result[11] = 0.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = 0.0f;
	result[15] = 1.0f;
	return (result);
}

float	*mat4_assign(float *result, float *m0)
{
	result[0] = m0[0];
	result[1] = m0[1];
	result[2] = m0[2];
	result[3] = m0[3];
	result[4] = m0[4];
	result[5] = m0[5];
	result[6] = m0[6];
	result[7] = m0[7];
	result[8] = m0[8];
	result[9] = m0[9];
	result[10] = m0[10];
	result[11] = m0[11];
	result[12] = m0[12];
	result[13] = m0[13];
	result[14] = m0[14];
	result[15] = m0[15];
	return (result);
}


void	mat4_string(float *m)
{
	int	i;

	i = 0;
	printf("{");
	while (i < 16)
	{
		if (i % 4 == 0)
			printf("\n");
		printf("%f ", m[i]);
		i++;
	}
	printf("}\n");
}

float	*mat4_translate(float *result, float *m0, float *v0)
{
	result[0] = m0[0];
	result[1] = m0[1];
	result[2] = m0[2];
	result[3] = m0[3];
	result[4] = m0[4];
	result[5] = m0[5];
	result[6] = m0[6];
	result[7] = m0[7];
	result[8] = m0[8];
	result[9] = m0[9];
	result[10] = m0[10];
	result[11] = m0[11];
	result[12] = m0[12] + v0[0];
	result[13] = m0[13] + v0[1];
	result[14] = m0[14] + v0[2];
	result[15] = m0[15];
	return (result);
}

float	*mat4_scale(float *result, float *m0, float *v0)
{
	result[0] = m0[0] * v0[0];
	result[1] = m0[1];
	result[2] = m0[2];
	result[3] = m0[3];
	result[4] = m0[4];
	result[5] = m0[5] * v0[1];
	result[6] = m0[6];
	result[7] = m0[7];
	result[8] = m0[8];
	result[9] = m0[9];
	result[10] = m0[10] * v0[2];
	result[11] = m0[11];
	result[12] = m0[12];
	result[13] = m0[13];
	result[14] = m0[14];
	result[15] = m0[15];
	return (result);
}

float	*mat4_rotation_x(float *result, float f)
{
	float	c;
	float	s;

	c = cos(f);
	s = sin(f);
	result[5] = c;
	result[6] = s;
	result[9] = -s;
	result[10] = c;
	return (result);
}

float	*mat4_rotation_y(float *result, float f)
{
	float	c = cos(f);
	float	s = sin(f);
	result[0] = c;
	result[2] = -s;
	result[8] = s;
	result[10] = c;
	return result;
}

float	*mat4_rotation_z(float *result, float f)
{
	float	c = cos(f);
	float	s = sin(f);
	result[0] = c;
	result[1] = s;
	result[4] = -s;
	result[5] = c;
	return result;
}

float	*mat4_rotation_axis(float *result, float *v0, float f)
{
	float	c = cos(f);
	float	s = sin(f);
	float	one_c = 1.0f - c;
	float	x = v0[0];
	float	y = v0[1];
	float	z = v0[2];
	float	xx = x * x;
	float	xy = x * y;
	float	xz = x * z;
	float	yy = y * y;
	float	yz = y * z;
	float	zz = z * z;
	float	l = xx + yy + zz;
	float	sqrt_l = sqrt(l);
	result[0] = (xx + (yy + zz) * c) / l;
	result[1] = (xy * one_c + v0[2] * sqrt_l * s) / l;
	result[2] = (xz * one_c - v0[1] * sqrt_l * s) / l;
	result[3] = 0.0f;
	result[4] = (xy * one_c - v0[2] * sqrt_l * s) / l;
	result[5] = (yy + (xx + zz) * c) / l;
	result[6] = (yz * one_c + v0[0] * sqrt_l * s) / l;
	result[7] = 0.0f;
	result[8] = (xz * one_c + v0[1] * sqrt_l * s) / l;
	result[9] = (yz * one_c - v0[0] * sqrt_l * s) / l;
	result[10] = (zz + (xx + yy) * c) / l;
	result[11] = 0.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = 0.0f;
	result[15] = 1.0f;
	return result;
}

float	*mat4_perspective_fov(float *result, float fov, float w, float h, float n, float f)
{
	float	h2 = cos(fov * 0.5f) / sin(fov * 0.5f);
	float	w2 = h2 * h / w;

	result[0] = w2;
	result[1] = 0.0f;
	result[2] = 0.0f;
	result[3] = 0.0f;
	result[4] = 0.0f;
	result[5] = h2;
	result[6] = 0.0f;
	result[7] = 0.0f;
	result[8] = 0.0f;
	result[9] = 0.0f;
	result[10] = f / (n - f);
	result[11] = -1.0f;
	result[12] = 0.0f;
	result[13] = 0.0f;
	result[14] = -(f * n) / (f - n);
	result[15] = 0.0f;
	return (result);
}

float	*mat4_look_at(float *result, float *position, float *target, float *up)
{
	float	tmp_forward[VEC3_SIZE];
	float	tmp_side[VEC3_SIZE];
	float	tmp_up[VEC3_SIZE];

	vec3_subtract(tmp_forward, target, position);
	vec3_normalize(tmp_forward, tmp_forward);
	vec3_cross(tmp_side, tmp_forward, up);
	vec3_normalize(tmp_side, tmp_side);
	vec3_cross(tmp_up, tmp_side, tmp_forward);
	result[0] = tmp_side[0];
	result[1] = tmp_up[0];
	result[2] = -tmp_forward[0];
	result[3] = 0.0f;
	result[4] = tmp_side[1];
	result[5] = tmp_up[1];
	result[6] = -tmp_forward[1];
	result[7] = 0.0f;
	result[8] = tmp_side[2];
	result[9] = tmp_up[2];
	result[10] = -tmp_forward[2];
	result[11] = 0.0f;
	result[12] = -vec3_dot(tmp_side, position);
	result[13] = -vec3_dot(tmp_up, position);
	result[14] = vec3_dot(tmp_forward, position);
	result[15] = 1.0f;
	return (result);
}

float *mat4_multiply(float *result, float *m0, float *m1)
{
	float multiplied[MAT4_SIZE];
	multiplied[0] = m0[0] * m1[0] + m0[4] * m1[1] + m0[8] * m1[2] + m0[12] * m1[3];
	multiplied[1] = m0[1] * m1[0] + m0[5] * m1[1] + m0[9] * m1[2] + m0[13] * m1[3];
	multiplied[2] = m0[2] * m1[0] + m0[6] * m1[1] + m0[10] * m1[2] + m0[14] * m1[3];
	multiplied[3] = m0[3] * m1[0] + m0[7] * m1[1] + m0[11] * m1[2] + m0[15] * m1[3];
	multiplied[4] = m0[0] * m1[4] + m0[4] * m1[5] + m0[8] * m1[6] + m0[12] * m1[7];
	multiplied[5] = m0[1] * m1[4] + m0[5] * m1[5] + m0[9] * m1[6] + m0[13] * m1[7];
	multiplied[6] = m0[2] * m1[4] + m0[6] * m1[5] + m0[10] * m1[6] + m0[14] * m1[7];
	multiplied[7] = m0[3] * m1[4] + m0[7] * m1[5] + m0[11] * m1[6] + m0[15] * m1[7];
	multiplied[8] = m0[0] * m1[8] + m0[4] * m1[9] + m0[8] * m1[10] + m0[12] * m1[11];
	multiplied[9] = m0[1] * m1[8] + m0[5] * m1[9] + m0[9] * m1[10] + m0[13] * m1[11];
	multiplied[10] = m0[2] * m1[8] + m0[6] * m1[9] + m0[10] * m1[10] + m0[14] * m1[11];
	multiplied[11] = m0[3] * m1[8] + m0[7] * m1[9] + m0[11] * m1[10] + m0[15] * m1[11];
	multiplied[12] = m0[0] * m1[12] + m0[4] * m1[13] + m0[8] * m1[14] + m0[12] * m1[15];
	multiplied[13] = m0[1] * m1[12] + m0[5] * m1[13] + m0[9] * m1[14] + m0[13] * m1[15];
	multiplied[14] = m0[2] * m1[12] + m0[6] * m1[13] + m0[10] * m1[14] + m0[14] * m1[15];
	multiplied[15] = m0[3] * m1[12] + m0[7] * m1[13] + m0[11] * m1[14] + m0[15] * m1[15];
	result[0] = multiplied[0];
	result[1] = multiplied[1];
	result[2] = multiplied[2];
	result[3] = multiplied[3];
	result[4] = multiplied[4];
	result[5] = multiplied[5];
	result[6] = multiplied[6];
	result[7] = multiplied[7];
	result[8] = multiplied[8];
	result[9] = multiplied[9];
	result[10] = multiplied[10];
	result[11] = multiplied[11];
	result[12] = multiplied[12];
	result[13] = multiplied[13];
	result[14] = multiplied[14];
	result[15] = multiplied[15];
	return (result);
}
