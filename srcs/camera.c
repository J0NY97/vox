#include "shaderpixel.h"

void	new_camera(t_camera *camera)
{
	new_vec3(camera->pos, 0.0f, 0.0f, 0.0f);
	new_vec3(camera->front, 0.0f, 0.0f, -1.0f);
	new_vec3(camera->up, 0.0f, 1.0f, 0.0f);
	camera->fov = 90.0f;
	camera->pitch = 0.0f;
	camera->yaw = -90.0f;
	camera->viewport_w = 1280;
	camera->viewport_h = 720;

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

	mat4_identity(camera->projection);
	mat4_perspective_fov(camera->projection, to_radians(camera->fov), camera->viewport_w, camera->viewport_h, 0.1f, 100.0f);

	mat4_identity(camera->view);
	mat4_look_at(camera->view, camera->pos, vec3_add(temp_vec3, camera->pos, camera->front), camera->up);
}

void	camera_print(t_camera *camera)
{
	ft_printf("CAMERA :\n");
	vec3_string("camera.pos : ", camera->pos);
	vec3_string("camera.front : ", camera->front);
	vec3_string("camera.up : ", camera->up);
	ft_printf("camera.pitch : %f\n", camera->pitch);
	ft_printf("camera.yaw : %f\n", camera->yaw);
	ft_printf("camera.fov : %f\n", camera->fov);
	ft_printf("camera.viewport_w : %f\n", camera->viewport_w);
	ft_printf("camera.viewport_h : %f\n", camera->viewport_h);
}
