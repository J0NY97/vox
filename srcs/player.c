#include "shaderpixel.h"

void	new_player(t_player *player)
{
	new_camera(&player->camera);

	player->enabled_mouse = 0;
	player->last_mouse_pos[0] = 0;
	player->last_mouse_pos[1] = 0;
}

void	player_events(t_player *player, t_key *keys, GLFWwindow *win)
{
	if (keys[GLFW_KEY_E].state == 1)
	{
		player->enabled_mouse = player->enabled_mouse == 0;
		glfwSetInputMode(win, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		if (player->enabled_mouse)
		{
			glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			LG_INFO("Mouse Enabled");
		}
		else
		{
			glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			LG_INFO("Mouse Disabled");
		}
	}
}

void	player_movement(t_player *player, GLFWwindow *win, t_fps fps)
{
	float	temp[VEC3_SIZE];
	float	speed = 1.0f;
	float	camera_speed;

	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed = 2.0f;
	camera_speed = speed * fps.delta_time;

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

void	player_looking(t_player *player, GLFWwindow *win, t_fps fps)
{
	float	x_offset;
	float	y_offset;
	float	sensitivity;

	sensitivity = 0.1f;

	glfwGetCursorPos(win, &player->mouse_pos[0], &player->mouse_pos[1]);
	x_offset = player->mouse_pos[0] - player->last_mouse_pos[0];
	y_offset = player->mouse_pos[1] - player->last_mouse_pos[1];
	player->last_mouse_pos[0] = player->mouse_pos[0];
	player->last_mouse_pos[1] = player->mouse_pos[1];

	x_offset *= sensitivity;
	y_offset *= sensitivity;

	player->camera.yaw = fmod((player->camera.yaw + x_offset), 360.0f);
	player->camera.pitch -= y_offset;

	if (player->camera.pitch > 89.0f)
		player->camera.pitch = 89.0f;
	else if (player->camera.pitch < -89.0f)
		player->camera.pitch = -89.0f;
}