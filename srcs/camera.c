#include "shaderpixel.h"

void	new_camera(t_camera *camera)
{
	new_vec3(camera->pos, 0.0f, 0.0f, 0.0f);
	new_vec3(camera->front, 0.0f, 0.0f, -1.0f);
	new_vec3(camera->up, 0.0f, 1.0f, 0.0f);
	vec3_normalize(camera->right,
		vec3_cross(camera->right, camera->up, camera->front));
	camera->fov = 90.0f;
	camera->pitch = 0.0f;
	camera->yaw = -90.0f;
	camera->near_plane = 0.1f;
	camera->far_plane = 200.0f;
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
	float	temp_vec3[VEC3_SIZE];

	new_vec3(temp_vec3,
		cos(to_radians(camera->yaw)) * cos(to_radians(camera->pitch)),
		sin(to_radians(camera->pitch)),
		sin(to_radians(camera->yaw)) * cos(to_radians(camera->pitch)));
	vec3_normalize(camera->front, temp_vec3);

	vec3_normalize(camera->right,
		vec3_cross(camera->right, camera->up, camera->front));

	mat4_identity(camera->projection);
	mat4_perspective_fov(camera->projection, to_radians(camera->fov),
		camera->viewport_w, camera->viewport_h,
		camera->near_plane, camera->far_plane);

	mat4_identity(camera->view);
	mat4_look_at(camera->view, camera->pos,
		vec3_add(temp_vec3, camera->pos, camera->front), camera->up);

	frustum_new(&camera->frustum, camera);
}

void	camera_print(t_camera *camera)
{
	ft_printf("CAMERA :\n");
	vec3_string("camera.pos : ", camera->pos);
	vec3_string("camera.front : ", camera->front);
	vec3_string("camera.up : ", camera->up);
	vec3_string("camera.right : ", camera->right);
	ft_printf("camera.pitch : %f\n", camera->pitch);
	ft_printf("camera.yaw : %f\n", camera->yaw);
	ft_printf("camera.fov : %f\n", camera->fov);
	ft_printf("camera.viewport_w : %d\n", camera->viewport_w);
	ft_printf("camera.viewport_h : %d\n", camera->viewport_h);
}
