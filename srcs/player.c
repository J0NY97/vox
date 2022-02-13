#include "shaderpixel.h"

void	new_player(t_player *player)
{
	new_camera(&player->camera);
}

void	player_movement(t_player *player, GLFWwindow *win, t_fps *fps)
{
	float	temp[VEC3_SIZE];
	float	speed = 1.0f;
	float	camera_speed;

	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 2.0f;
	camera_speed = speed * fps->delta_time;

	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
	{
		vec3_multiply_f(temp, player->camera.front, camera_speed);
		vec3_add(player->camera.pos, player->camera.pos, temp);
	}
	else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
	{
		vec3_multiply_f(temp, player->camera.front, camera_speed);
		vec3_subtract(player->camera.pos, player->camera.pos, temp);
	}
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
	{
		vec3_cross(temp, player->camera.front, player->camera.up);
		vec3_normalize(temp, temp);
		vec3_multiply_f(temp, temp, camera_speed);
		vec3_subtract(player->camera.pos, player->camera.pos, temp);
	}
	else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
	{
		vec3_cross(temp, player->camera.front, player->camera.up);
		vec3_normalize(temp, temp);
		vec3_multiply_f(temp, temp, camera_speed);
		vec3_add(player->camera.pos, player->camera.pos, temp);
	}
}