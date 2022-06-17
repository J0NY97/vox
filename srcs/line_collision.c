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

	v3_normalize(a, start);
	v3_normalize(b, end);
	if (a[0] != b[0] || a[1] != b[1] || a[2] != b[2])
		return (0);
	for (int i = 0; i < 3; i++)
	{
		if (a[i] > g)
			g = a[i];
		if (b[i] > g)
			g = b[i];
	}
	v3_normalize(c, point);
	v3_divide_f(a, a, g);
	v3_divide_f(b, b, g);
	v3_divide_f(c, c, g);
	v3_string("a ", a);
	v3_string("b ", b);
	v3_string("c ", c);
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

	v3_sub(db, p2, p1);
	v3_sub(dc, p1, orig);

	v3_cross(jj, dir, db);
	if (fabs(v3_dot(dc, jj)) >= 0.7f)
		return (0);
		
	float	s;
	float	a = v3_length_sqrd(jj);
	float	b[3];
	v3_cross(b, dc, db);
	float	c = v3_dot(b, jj);
	s = c / a;

	if (s >= 0.0 && s <= 1.0)
	{
		float	intersection[3];
		float	tmp[3];
		v3_multiply_f(tmp, dir, s);
		v3_add(intersection, orig, tmp);

		float	p1il[3]; // lenght_squared(intersection - p1)
		float	p2il[3];

		v3_sub(p1il, intersection, p1);
		v3_sub(p2il, intersection, p2);
		if (v3_length_sqrd(p1il) + v3_length_sqrd(p2il) <=
			v3_length_sqrd(p1) + v3_length_sqrd(p2) + 0.001f)
		{
			v3_assign(intersect_point, intersection);
			return (1);
		}
	}

	return (0);
}