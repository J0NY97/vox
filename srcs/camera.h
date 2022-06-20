/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 12:14:59 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 14:00:30 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CAMERA_H
# define CAMERA_H

# include "bmath.h"
# include "aabb.h"

typedef struct s_camera	t_camera;

///////////////////
//	Frustum
///////////////////

typedef struct s_plane
{
	float	normal[V3_SIZE];
	float	dist;
}	t_plane;

typedef struct s_frustum
{
	t_plane	top_plane;
	t_plane	bot_plane;
	t_plane	right_plane;
	t_plane	left_plane;
	t_plane	far_plane;
	t_plane	near_plane;
}		t_frustum;

void		frustum_new(t_frustum *frustum, t_camera *camera);
void		frustum_print(t_frustum *frustum);
void		plane_new(t_plane *plane, float *p, float *norm);
void		plane_print(t_plane *plane, char *str);
int			aabb_in_frustum(t_aabb *a, t_frustum *f);
int			aabb_on_plane(t_aabb *a, t_plane *p);

///////////////////
//	Camera
///////////////////

struct s_camera
{
	float		pos[V3_SIZE];
	float		front[V3_SIZE];
	float		up[V3_SIZE];
	float		right[V3_SIZE];
	float		yaw;
	float		pitch;
	float		fov;
	float		aspect;
	float		near_plane;
	float		far_plane;
	int			viewport_w;
	int			viewport_h;

	float		view[M4_SIZE];
	float		projection[M4_SIZE];

	t_frustum	frustum;
};

void		new_camera(t_camera *camera);
void		update_camera(t_camera *camera);
void		camera_print(t_camera *camera);

#endif