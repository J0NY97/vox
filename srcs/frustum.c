#include "shaderpixel.h"

void	plane_new(t_plane *plane, float *p, float *norm)
{
	vec3_normalize(plane->normal, norm);
	plane->dist = vec3_dot(plane->normal, p);
}

void	frustum_new(t_frustum *frustum, t_camera *camera)
{
	float	half_h_side = camera->far_plane * tanf(to_radians(camera->fov) * 0.5f);
	float	half_v_side = half_h_side * camera->aspect;
	float	front_mult_far[VEC3_SIZE];
	vec3_multiply_f(front_mult_far, camera->front, camera->far_plane);

	float	tmp[3];

	vec3_multiply_f(tmp, camera->front, camera->near_plane);
	vec3_add(tmp, tmp, camera->pos);
	plane_new(&frustum->near_plane, tmp, camera->front);

	vec3_add(tmp, camera->pos, front_mult_far);
	plane_new(&frustum->far_plane, tmp,
		(float []){-camera->front[0], -camera->front[1], -camera->front[2]});

	vec3_multiply_f(tmp, camera->right, half_h_side);
	vec3_add(tmp, tmp, front_mult_far);
	vec3_cross(tmp, camera->up, tmp);
	plane_new(&frustum->right_plane, camera->pos, tmp);

	vec3_multiply_f(tmp, camera->right, half_h_side);
	vec3_sub(tmp, front_mult_far, tmp);
	vec3_cross(tmp, tmp, camera->up);
	plane_new(&frustum->left_plane, camera->pos, tmp);

	vec3_multiply_f(tmp, camera->up, half_v_side);
	vec3_sub(tmp, front_mult_far, tmp);
	vec3_cross(tmp, camera->right, tmp);
	plane_new(&frustum->top_plane, camera->pos, tmp);

	vec3_multiply_f(tmp, camera->up, half_v_side);
	vec3_add(tmp, front_mult_far, tmp);
	vec3_cross(tmp, tmp, camera->right);
	plane_new(&frustum->bot_plane, camera->pos, tmp);
}

int	aabb_on_plane(t_aabb *a, t_plane *p)
{
	float	c[3];
	float	e[3];

	vec3_multiply_f(c, vec3_add(c, a->max, a->min), 0.5f);
	vec3_sub(e, a->max, c);

	float	r = e[0] * fabs(p->normal[0]) +
		e[1] * fabs(p->normal[1]) +
		e[2] * fabs(p->normal[2]);
	float	s = vec3_dot(p->normal, c) - p->dist;


	return (-r <= s);
/*
	return (fabs(s) <= r);
	*/
}

int	aabb_in_frustum(t_aabb *a, t_frustum *f)
{
	return (
	/*	aabb_on_plane(a, &f->top_plane) &&
		aabb_on_plane(a, &f->bot_plane) &&
		aabb_on_plane(a, &f->right_plane) &&
		aabb_on_plane(a, &f->left_plane) &&
		aabb_on_plane(a, &f->far_plane) &&*/
		aabb_on_plane(a, &f->near_plane));
}
