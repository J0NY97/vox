#include "bmath.h"

int	mod(int a, int b)
{
	int	r;

	r = a % b;
    return (r < 0 ? r + b : r);
}

float	*v3_new(float *src, float x, float y, float z)
{
	src[0] = x;
	src[1] = y;
	src[2] = z;
	return (src);
}

float	*v3_assign(float *dest, float *src)
{
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	return (src);
}

void	v3_string(char *str, float *v)
{
	printf("%s { %f %f %f }\n", str, v[0], v[1], v[2]);
}

float *v3_add(float *result, float *v0, float *v1)
{
	result[0] = v0[0] + v1[0];
	result[1] = v0[1] + v1[1];
	result[2] = v0[2] + v1[2];
	return (result);
}

float *v3_sub(float *result, float *v0, float *v1)
{
	result[0] = v0[0] - v1[0];
	result[1] = v0[1] - v1[1];
	result[2] = v0[2] - v1[2];
	return (result);
}

float *v3_normalize(float *result, float *v0)
{
	float l = sqrt(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
	result[0] = v0[0] / l;
	result[1] = v0[1] / l;
	result[2] = v0[2] / l;
	return (result);
}

float *v3_cross(float *res, float *v0, float *v1)
{
	float	c[V3_SIZE];

	c[0] = v0[1] * v1[2] - v0[2] * v1[1];
	c[1] = v0[2] * v1[0] - v0[0] * v1[2];
	c[2] = v0[0] * v1[1] - v0[1] * v1[0];
	res[0] = c[0];
	res[1] = c[1];
	res[2] = c[2];
	return (res);
}

float	*v3_multiply(float *res, float *v0, float *v1)
{
	res[0] = v0[0] * v1[0];
	res[1] = v0[1] * v1[1];
	res[2] = v0[2] * v1[2];
	return (res);
}

float	v3_dot(float *v0, float *v1)
{
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

float	*v3_multiply_f(float *result, float *v0, float f)
{
	result[0] = v0[0] * f;
	result[1] = v0[1] * f;
	result[2] = v0[2] * f;
	return result;
}

float	*v3_divide_f(float *res, float *v0, float f)
{
	res[0] = v0[0] / f;
	res[1] = v0[1] / f;
	res[2] = v0[2] / f;
	return (res);
}

float	v3_dist(float *v0, float *v1)
{
	float	a;
	float	b;
	float	c;

	a = (v1[0] - v0[0]) * (v1[0] - v0[0]);
	b = (v1[1] - v0[1]) * (v1[1] - v0[1]);
	c = (v1[2] - v0[2]) * (v1[2] - v0[2]);
	return (sqrt(a + b + c));
}

float	v3_dist_sqrd(float *v0, float *v1)
{
	return ((v0[0] - v1[0]) * (v0[0] - v1[0]) +
		(v0[1] - v1[1]) * (v0[1] - v1[1]) +
		(v0[2] - v1[2]) * (v0[2] - v1[2]));
}

float	v3_length(float *v0)
{
	return sqrt(v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
}

float	v3_length_sqrd(float *v0)
{
	return (v0[0] * v0[0] + v0[1] * v0[1] + v0[2] * v0[2]);
}

float	*v3_multiply_m3(float *res, float *v, float *m)
{
	float	xyz[V3_SIZE];

	xyz[0] = v[0];
	xyz[1] = v[1];
	xyz[2] = v[2];
	res[0] = m[0] * xyz[0] + m[3] * xyz[1] + m[6] * xyz[2];
	res[1] = m[1] * xyz[0] + m[4] * xyz[1] + m[7] * xyz[2];
	res[2] = m[2] * xyz[0] + m[5] * xyz[1] + m[8] * xyz[2];
	return (res);
}

float	*v3_to_v4(float *res, float *v3)
{
	res[0] = v3[0];
	res[1] = v3[1];
	res[2] = v3[2];
	res[3] = 1;
	return (res);
}

float	*v4_to_v3(float *res, float *v4)
{
	res[0] = v4[0];
	res[1] = v4[1];
	res[2] = v4[2];
	return (res);
}

float	*m4_to_m3(float *res, float *m4)
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

float	*v4_new(float *res, float x, float y, float z, float w)
{
	res[0] = x;
	res[1] = y;
	res[2] = z;
	res[3] = w;
	return (res);
}

void	v4_string(char *str, float *v)
{
	printf("%s { %f %f %f %f }\n", str, v[0], v[1], v[2], v[3]);
}

float	*v4_multiply_m4(float *res, float *v, float *m)
{
	float	xyzw[V4_SIZE];

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
	return (degrees * _PI_180);
}

float	to_degrees(float radians)
{
	return (radians * _180_PI);
}

////////////////////////////////
// MAT2
////////////////////////////////

float	mat2_determinant(float *m)
{
	return (m[0] * m[3] - m[2] * m[1]);
}

////////////////////////////////
// MAT3
////////////////////////////////

float	*m3_identity(float *result)
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

float	*m3_assign(float *result, float *m0)
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

void	m3_string(char *str, float *m)
{
	int	i;

	i = 0;
	printf("%s {", str);
	while (i < M3_SIZE)
	{
		if (i % 3 == 0)
			printf("\n");
		printf("%f ", m[i]);
		i++;
	}
	printf("}\n");
}

float	*mat3_multiply_f(float *result, float *m0, float f)
{
	result[0] = m0[0] * f;
	result[1] = m0[1] * f;
	result[2] = m0[2] * f;
	result[3] = m0[3] * f;
	result[4] = m0[4] * f;
	result[5] = m0[5] * f;
	result[6] = m0[6] * f;
	result[7] = m0[7] * f;
	result[8] = m0[8] * f;
	return (result);
}

float *mat3_cofactor(float *result, float *m0)
{
	float	cofactor[M3_SIZE];
	float	minor[M2_SIZE];
	minor[0] = m0[4];
	minor[1] = m0[5];
	minor[2] = m0[7];
	minor[3] = m0[8];
	cofactor[0] = mat2_determinant(minor);
	minor[0] = m0[3];
	minor[1] = m0[5];
	minor[2] = m0[6];
	minor[3] = m0[8];
	cofactor[1] = -mat2_determinant(minor);
	minor[0] = m0[3];
	minor[1] = m0[4];
	minor[2] = m0[6];
	minor[3] = m0[7];
	cofactor[2] = mat2_determinant(minor);
	minor[0] = m0[1];
	minor[1] = m0[2];
	minor[2] = m0[7];
	minor[3] = m0[8];
	cofactor[3] = -mat2_determinant(minor);
	minor[0] = m0[0];
	minor[1] = m0[2];
	minor[2] = m0[6];
	minor[3] = m0[8];
	cofactor[4] = mat2_determinant(minor);
	minor[0] = m0[0];
	minor[1] = m0[1];
	minor[2] = m0[6];
	minor[3] = m0[7];
	cofactor[5] = -mat2_determinant(minor);
	minor[0] = m0[1];
	minor[1] = m0[2];
	minor[2] = m0[4];
	minor[3] = m0[5];
	cofactor[6] = mat2_determinant(minor);
	minor[0] = m0[0];
	minor[1] = m0[2];
	minor[2] = m0[3];
	minor[3] = m0[5];
	cofactor[7] = -mat2_determinant(minor);
	minor[0] = m0[0];
	minor[1] = m0[1];
	minor[2] = m0[3];
	minor[3] = m0[4];
	cofactor[8] = mat2_determinant(minor);
	result[0] = cofactor[0];
	result[1] = cofactor[1];
	result[2] = cofactor[2];
	result[3] = cofactor[3];
	result[4] = cofactor[4];
	result[5] = cofactor[5];
	result[6] = cofactor[6];
	result[7] = cofactor[7];
	result[8] = cofactor[8];
	return (result);
}

float mat3_determinant(float *m)
{
	float	m11 = m[0];
	float	m21 = m[1];
	float	m31 = m[2];
	float	m12 = m[3];
	float	m22 = m[4];
	float	m32 = m[5];
	float	m13 = m[6];
	float	m23 = m[7];
	float	m33 = m[8];
	float	determinant = m11 * m22 * m33
		+ m12 * m23 * m31
		+ m13 * m21 * m32
		- m11 * m23 * m32
		- m12 * m21 * m33
		- m13 * m22 * m31;
	return (determinant);
}

float	*m3_inverse(float *res, float *m)
{
	float	inverse[M3_SIZE];
	float	det;

	det = mat3_determinant(m);
	mat3_cofactor(inverse, m);
	mat3_multiply_f(inverse, inverse, 1.0f / det);
	for (int i = 0; i < M3_SIZE; i++)
		res[i] = inverse[i];
	return (res);
}

///////////////////
// MAT4
///////////////////

float	*m4_identity(float *result)
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

float	*m4_assign(float *result, float *m0)
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

void	m4_string(char *str, float *m)
{
	int	i;

	i = 0;
	printf("%s : {", str);
	while (i < 16)
	{
		if (i % 4 == 0)
			printf("\n");
		printf("%f ", m[i]);
		i++;
	}
	printf("}\n");
}

float	*m4_translate(float *result, float *m0, float *v0)
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

float	*m4_scale(float *result, float *m0, float *v0)
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

float	*m4_rotation_x(float *result, float f)
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

float	*m4_rotation_y(float *result, float f)
{
	float	c = cos(f);
	float	s = sin(f);
	result[0] = c;
	result[2] = -s;
	result[8] = s;
	result[10] = c;
	return result;
}

float	*m4_rotation_z(float *result, float f)
{
	float	c = cos(f);
	float	s = sin(f);
	result[0] = c;
	result[1] = s;
	result[4] = -s;
	result[5] = c;
	return result;
}

float	*m4_rotation_axis(float *result, float *v0, float f)
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

float	*m4_perspective_fov(float *result, float fov, float w, float h, float n, float f)
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

float	*m4_look_at(float *result, float *position, float *target, float *up)
{
	float	tmp_forward[V3_SIZE];
	float	tmp_side[V3_SIZE];
	float	tmp_up[V3_SIZE];

	v3_sub(tmp_forward, target, position);
	v3_normalize(tmp_forward, tmp_forward);
	v3_cross(tmp_side, tmp_forward, up);
	v3_normalize(tmp_side, tmp_side);
	v3_cross(tmp_up, tmp_side, tmp_forward);

/*
	float t[3];
	v3_string("look at", v3_normalize(t, v3_add(t, tmp_forward, v3_add(t, tmp_up, tmp_side))));
	*/

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
	result[12] = -v3_dot(tmp_side, position);
	result[13] = -v3_dot(tmp_up, position);
	result[14] = v3_dot(tmp_forward, position);
	result[15] = 1.0f;
	return (result);
}

/*
 * NOTE: We dont want to save the calculation directly into result, since
 * result could be one of the matrices m0 or m1;
 * 
 * There is a difference in which matrix you multiplicate by;
 * why?
*/
float	*m4_multiply(float *result, float *m0, float *m1)
{
	float	multiplied[M4_SIZE];
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

float	*m4_inverse(float *result, float *m0)
{
	float inverse[M4_SIZE];
	float inverted_determinant;
	float m11 = m0[0];
	float m21 = m0[1];
	float m31 = m0[2];
	float m41 = m0[3];
	float m12 = m0[4];
	float m22 = m0[5];
	float m32 = m0[6];
	float m42 = m0[7];
	float m13 = m0[8];
	float m23 = m0[9];
	float m33 = m0[10];
	float m43 = m0[11];
	float m14 = m0[12];
	float m24 = m0[13];
	float m34 = m0[14];
	float m44 = m0[15];

	inverse[0] = m22 * m33 * m44
		- m22 * m43 * m34
		- m23 * m32 * m44
		+ m23 * m42 * m34
		+ m24 * m32 * m43
		- m24 * m42 * m33;
	inverse[4] = -m12 * m33 * m44
		+ m12 * m43 * m34
		+ m13 * m32 * m44
		- m13 * m42 * m34
		- m14 * m32 * m43
		+ m14 * m42 * m33;
	inverse[8] = m12 * m23 * m44
		- m12 * m43 * m24
		- m13 * m22 * m44
		+ m13 * m42 * m24
		+ m14 * m22 * m43
		- m14 * m42 * m23;
	inverse[12] = -m12 * m23 * m34
		+ m12 * m33 * m24
		+ m13 * m22 * m34
		- m13 * m32 * m24
		- m14 * m22 * m33
		+ m14 * m32 * m23;
	inverse[1] = -m21 * m33 * m44
		+ m21 * m43 * m34
		+ m23 * m31 * m44
		- m23 * m41 * m34
		- m24 * m31 * m43
		+ m24 * m41 * m33;
	inverse[5] =m11 * m33 * m44
		-m11 * m43 * m34
		- m13 * m31 * m44
		+ m13 * m41 * m34
		+ m14 * m31 * m43
		- m14 * m41 * m33;
	inverse[9] = -m11 * m23 * m44
		+m11 * m43 * m24
		+ m13 * m21 * m44
		- m13 * m41 * m24
		- m14 * m21 * m43
		+ m14 * m41 * m23;
	inverse[13] =m11 * m23 * m34
		-m11 * m33 * m24
		- m13 * m21 * m34
		+ m13 * m31 * m24
		+ m14 * m21 * m33
		- m14 * m31 * m23;
	inverse[2] = m21 * m32 * m44
		- m21 * m42 * m34
		- m22 * m31 * m44
		+ m22 * m41 * m34
		+ m24 * m31 * m42
		- m24 * m41 * m32;
	inverse[6] = -m11 * m32 * m44
		+m11 * m42 * m34
		+ m12 * m31 * m44
		- m12 * m41 * m34
		- m14 * m31 * m42
		+ m14 * m41 * m32;
	inverse[10] =m11 * m22 * m44
		-m11 * m42 * m24
		- m12 * m21 * m44
		+ m12 * m41 * m24
		+ m14 * m21 * m42
		- m14 * m41 * m22;
	inverse[14] = -m11 * m22 * m34
		+m11 * m32 * m24
		+ m12 * m21 * m34
		- m12 * m31 * m24
		- m14 * m21 * m32
		+ m14 * m31 * m22;
	inverse[3] = -m21 * m32 * m43
		+ m21 * m42 * m33
		+ m22 * m31 * m43
		- m22 * m41 * m33
		- m23 * m31 * m42
		+ m23 * m41 * m32;
	inverse[7] =m11 * m32 * m43
		-m11 * m42 * m33
		- m12 * m31 * m43
		+ m12 * m41 * m33
		+ m13 * m31 * m42
		- m13 * m41 * m32;
	inverse[11] = -m11 * m22 * m43
		+m11 * m42 * m23
		+ m12 * m21 * m43
		- m12 * m41 * m23
		- m13 * m21 * m42
		+ m13 * m41 * m22;
	inverse[15] =m11 * m22 * m33
		-m11 * m32 * m23
		- m12 * m21 * m33
		+ m12 * m31 * m23
		+ m13 * m21 * m32
		- m13 * m31 * m22;
	inverted_determinant = 1.0f / (m11 * inverse[0] + m21 * inverse[4] + m31 * inverse[8] + m41 * inverse[12]);
	result[0] = inverse[0] * inverted_determinant;
	result[1] = inverse[1] * inverted_determinant;
	result[2] = inverse[2] * inverted_determinant;
	result[3] = inverse[3] * inverted_determinant;
	result[4] = inverse[4] * inverted_determinant;
	result[5] = inverse[5] * inverted_determinant;
	result[6] = inverse[6] * inverted_determinant;
	result[7] = inverse[7] * inverted_determinant;
	result[8] = inverse[8] * inverted_determinant;
	result[9] = inverse[9] * inverted_determinant;
	result[10] = inverse[10] * inverted_determinant;
	result[11] = inverse[11] * inverted_determinant;
	result[12] = inverse[12] * inverted_determinant;
	result[13] = inverse[13] * inverted_determinant;
	result[14] = inverse[14] * inverted_determinant;
	result[15] = inverse[15] * inverted_determinant;
	return (result);
}

////////////////
// QUAT
////////////////

float	*quat_new(float *res, float x, float y, float z, float w)
{
	res[0] = x;
	res[1] = y;
	res[2] = z;
	res[3] = w;
	return (res);
}

float	*euler_to_quat(float *res, float roll, float pitch, float yaw) // roll (X), pitch (Y), yaw (Z)
{
	double	cy = cos(yaw * 0.5);
	double	sy = sin(yaw * 0.5);
	double	cp = cos(pitch * 0.5);
	double	sp = sin(pitch * 0.5);
	double	cr = cos(roll * 0.5);
	double	sr = sin(roll * 0.5);

	res[0] = sr * cp * cy - cr * sp * sy;
	res[1] = cr * sp * cy + sr * cp * sy;
	res[2] = cr * cp * sy - sr * sp * cy;
	res[3] = cr * cp * cy + sr * sp * sy;
	return (res);
}

float	*m4_rotation_quat(float *res, float *q0)
{
	float	xx = q0[0] * q0[0];
	float	yy = q0[1] * q0[1];
	float	zz = q0[2] * q0[2];
	float	xy = q0[0] * q0[1];
	float	zw = q0[2] * q0[3];
	float	xz = q0[0] * q0[2];
	float	yw = q0[1] * q0[3];
	float	yz = q0[1] * q0[2];
	float	xw = q0[0] * q0[3];
	res[0] = 1.0f - (2.0) * (yy + zz);
	res[1] = (2.0) * (xy + zw);
	res[2] = (2.0) * (xz - yw);
	res[3] = (0.0);
	res[4] = (2.0) * (xy - zw);
	res[5] = (1.0) - (2.0) * (xx + zz);
	res[6] = (2.0) * (yz + xw);
	res[7] = (0.0);
	res[8] = (2.0) * (xz + yw);
	res[9] = (2.0) * (yz - xw);
	res[10] = (1.0) - (2.0) * (xx + yy);
	res[11] = (0.0);
	res[12] = (0.0);
	res[13] = (0.0);
	res[14] = (0.0);
	res[15] = (1.0);
	return (res);
}
