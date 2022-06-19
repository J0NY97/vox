#include "shaderpixel.h"

void	new_camera(t_camera *camera)
{
	v3_new(camera->pos, 0.0f, 0.0f, 0.0f);
	v3_new(camera->front, 0.0f, 0.0f, -1.0f);
	v3_new(camera->up, 0.0f, 1.0f, 0.0f);
	v3_normalize(camera->right,
		v3_cross(camera->right, camera->up, camera->front));
	camera->fov = 90.0f;
	camera->pitch = 0.0f;
	camera->yaw = -90.0f;
	camera->near_plane = 0.1f;
	camera->far_plane = 400.0f;
	camera->viewport_w = 1280;
	camera->viewport_h = 720;
	camera->aspect = camera->viewport_w / camera->viewport_h;

	update_camera(camera);
	LG_INFO("new camera made.");
}

/*
 * Re-calculates the matrices (view & projection);
 */
void	update_camera(t_camera *camera)
{
	float	v3[V3_SIZE];
	float	rad_yaw;
	float	rad_pitch;

	rad_yaw = to_radians(camera->yaw);
	rad_pitch = to_radians(camera->pitch);
	v3_new(camera->front, cos(rad_yaw) * cos(rad_pitch),
		sin(rad_pitch), sin(rad_yaw) * cos(rad_pitch));
	v3_normalize(camera->front, camera->front);

	v3_normalize(camera->right,
		v3_cross(camera->right, camera->up, camera->front));

	m4_identity(camera->projection);
	m4_perspective_fov(camera->projection, to_radians(camera->fov),
		camera->viewport_w, camera->viewport_h,
		camera->near_plane, camera->far_plane);

	m4_identity(camera->view);
	m4_look_at(camera->view, camera->pos,
		v3_add(v3, camera->pos, camera->front), camera->up);

	frustum_new(&camera->frustum, camera);
}

void	camera_print(t_camera *camera)
{
	ft_printf("CAMERA :\n");
	v3_string("camera.pos : ", camera->pos);
	v3_string("camera.front : ", camera->front);
	v3_string("camera.up : ", camera->up);
	v3_string("camera.right : ", camera->right);
	ft_printf("camera.pitch : %f\n", camera->pitch);
	ft_printf("camera.yaw : %f\n", camera->yaw);
	ft_printf("camera.fov : %f\n", camera->fov);
	ft_printf("camera.viewport_w : %d\n", camera->viewport_w);
	ft_printf("camera.viewport_h : %d\n", camera->viewport_h);
	ft_printf("camera.near_plane : %.2f\n", camera->near_plane);
	ft_printf("camera.far_plane : %.2f\n", camera->far_plane);
	frustum_print(&camera->frustum);
}
