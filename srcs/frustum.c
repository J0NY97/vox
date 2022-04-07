#include "shaderpixel.h"

void	plane_new(t_plane *plane, float *p, float *norm)
{
	vec3_normalize(plane->normal, norm);
	plane->dist = vec3_dot(plane->normal, p);
}

void	plane_print(t_plane *plane, char *str)
{
	if (str)
		ft_printf("[Plane] : %s\n", str);
	else
	ft_printf("[Plane]\n");
	vec3_string("\tnormal :", plane->normal);
	ft_printf("\tdist : %f\n", plane->dist);
}

void	frustum_print(t_frustum *frustum)
{
	plane_print(&frustum->near_plane, "near");
	plane_print(&frustum->far_plane, "far");
	plane_print(&frustum->top_plane, "top");
	plane_print(&frustum->bot_plane, "bot");
	plane_print(&frustum->left_plane, "left");
	plane_print(&frustum->right_plane, "right");
}

void	frustum_new_2(t_frustum *frustum, t_camera *camera)
{
	float	half_h_side = camera->far_plane * tanf(camera->fov * 0.5f);
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

int	mat_ind(int col, int row)
{
	return (4 * row + col);
}

void	frustum_new(t_frustum *frustum, t_camera *camera)
{
	float	combo[MAT4_SIZE];

	mat4_multiply(combo, camera->projection, camera->view);

	frustum->left_plane.normal[0] = combo[mat_ind(0, 3)] + combo[mat_ind(0, 0)];
	frustum->left_plane.normal[1] = combo[mat_ind(1, 3)] + combo[mat_ind(1, 0)];
	frustum->left_plane.normal[2] = combo[mat_ind(2, 3)] + combo[mat_ind(2, 0)];
	frustum->left_plane.dist = combo[mat_ind(3, 3)] + combo[mat_ind(3, 0)];

	frustum->right_plane.normal[0] = combo[mat_ind(0, 3)] - combo[mat_ind(0, 0)];
	frustum->right_plane.normal[1] = combo[mat_ind(1, 3)] - combo[mat_ind(1, 0)];
	frustum->right_plane.normal[2] = combo[mat_ind(2, 3)] - combo[mat_ind(2, 0)];
	frustum->right_plane.dist = combo[mat_ind(3, 3)] - combo[mat_ind(3, 0)];


	frustum->bot_plane.normal[0] = combo[mat_ind(0, 3)] + combo[mat_ind(0, 1)];
	frustum->bot_plane.normal[1] = combo[mat_ind(1, 3)] + combo[mat_ind(1, 1)];
	frustum->bot_plane.normal[2] = combo[mat_ind(2, 3)] + combo[mat_ind(2, 1)];
	frustum->bot_plane.dist = combo[mat_ind(3, 3)] + combo[mat_ind(3, 1)];

	frustum->top_plane.normal[0] = combo[mat_ind(0, 3)] - combo[mat_ind(0, 1)];
	frustum->top_plane.normal[1] = combo[mat_ind(1, 3)] - combo[mat_ind(1, 1)];
	frustum->top_plane.normal[2] = combo[mat_ind(2, 3)] - combo[mat_ind(2, 1)];
	frustum->top_plane.dist = combo[mat_ind(3, 3)] - combo[mat_ind(3, 1)];


	frustum->near_plane.normal[0] = combo[mat_ind(0, 3)] + combo[mat_ind(0, 2)];
	frustum->near_plane.normal[1] = combo[mat_ind(1, 3)] + combo[mat_ind(1, 2)];
	frustum->near_plane.normal[2] = combo[mat_ind(2, 3)] + combo[mat_ind(2, 2)];
	frustum->near_plane.dist = combo[mat_ind(3, 3)] + combo[mat_ind(3, 2)];

	frustum->far_plane.normal[0] = combo[mat_ind(0, 3)] - combo[mat_ind(0, 2)];
	frustum->far_plane.normal[1] = combo[mat_ind(1, 3)] - combo[mat_ind(1, 2)];
	frustum->far_plane.normal[2] = combo[mat_ind(2, 3)] - combo[mat_ind(2, 2)];
	frustum->far_plane.dist = combo[mat_ind(3, 3)] - combo[mat_ind(3, 2)];

	vec3_normalize(frustum->left_plane.normal, frustum->left_plane.normal);
	vec3_normalize(frustum->right_plane.normal, frustum->right_plane.normal);
	vec3_normalize(frustum->top_plane.normal, frustum->top_plane.normal);
	vec3_normalize(frustum->bot_plane.normal, frustum->bot_plane.normal);
	vec3_normalize(frustum->near_plane.normal, frustum->near_plane.normal);
	vec3_normalize(frustum->far_plane.normal, frustum->far_plane.normal);
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
	float	c[3];
	float	e[3];
	float	r;
	float	s;

	vec3_multiply_f(c, vec3_add(c, a->max, a->min), 0.5f);
	vec3_sub(e, a->max, c);
	r = e[0] * fabs(p->normal[0]) +
		e[1] * fabs(p->normal[1]) +
		e[2] * fabs(p->normal[2]);
	s = vec3_dot(p->normal, c) + p->dist;
	return (s + r > 0);
	*/
}

int	aabb_in_frustum(t_aabb *a, t_frustum *f)
{
	return (
	/*	aabb_on_plane(a, &f->top_plane) &&
		aabb_on_plane(a, &f->bot_plane) &&
		aabb_on_plane(a, &f->right_plane) &&
		aabb_on_plane(a, &f->left_plane) &&*/
		aabb_on_plane(a, &f->far_plane) &&
		aabb_on_plane(a, &f->near_plane));
}
