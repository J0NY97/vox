#include "collision.h"

/*
 * 'point'	: float [3], point to test with;
 * 'start'	: line start point;
 * 'end'	: line end point;
*/
/* MAY NOT WORK, DONT USE BEFORE TESTED */
int	point_on_line(float *point, float *start, float *end)
{
	float	a[3];
	float	b[3];
	float	c[3];
	float	g = -(INFINITY - 1);

	vec3_normalize(a, start);
	vec3_normalize(b, end);
	if (a[0] != b[0] || a[1] != b[1] || a[2] != b[2])
		return (0);
	for (int i = 0; i < 3; i++)
	{
		if (a[i] > g)
			g = a[i];
		if (b[i] > g)
			g = b[i];
	}
	vec3_normalize(c, point);
	vec3_divide_f(a, a, g);
	vec3_divide_f(b, b, g);
	vec3_divide_f(c, c, g);
	vec3_string("a ", a);
	vec3_string("b ", b);
	vec3_string("c ", c);
	return ((a[0] == c[0] && a[1] == c[1] && a[2] == c[2]));
}

/*
 * save intersect point in 'intersect_point';
*/
int	ray_line_intersect(float *orig, float *dir, float *p1, float *p2, float *intersect_point)
{
	float	db[3];
	float	dc[3];
	float	jj[3];

	vec3_sub(db, p2, p1);
	vec3_sub(dc, p1, orig);

	vec3_cross(jj, dir, db);
	if (fabs(vec3_dot(dc, jj)) >= 0.7f)
		return (0);
		
	float	s;
	float	a = vec3_length_squared(jj);
	float	b[3];
	vec3_cross(b, dc, db);
	float	c = vec3_dot(b, jj);
	s = c / a;

	if (s >= 0.0 && s <= 1.0)
	{
		float	intersection[3];
		float	tmp[3];
		vec3_multiply_f(tmp, dir, s);
		vec3_add(intersection, orig, tmp);

		float	p1il[3]; // lenght_squared(intersection - p1)
		float	p2il[3];

		vec3_sub(p1il, intersection, p1);
		vec3_sub(p2il, intersection, p2);
		if (vec3_length_squared(p1il) + vec3_length_squared(p2il) <=
			vec3_length_squared(p1) + vec3_length_squared(p2) + 0.001f)
		{
			vec3_assign(intersect_point, intersection);
			return (1);
		}
	}

	return (0);
}