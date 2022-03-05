#include "shaderpixel.h"

void	new_player(t_player *player)
{
	new_camera(&player->camera);

	player->enabled_mouse = 0;
	player->last_mouse_pos[0] = 0;
	player->last_mouse_pos[1] = 0;

	new_vec3(player->velocity, 0, 0, 0);

	player->moving = 0;
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

	player->aabb.min[0] = player->camera.pos[0] - 1.0f;
	player->aabb.min[1] = player->camera.pos[1] - 1.0f;
	player->aabb.min[2] = player->camera.pos[2] - 1.0f;
	player->aabb.max[0] = player->camera.pos[0] + 1.0f;
	player->aabb.max[1] = player->camera.pos[1] + 1.0f;
	player->aabb.max[2] = player->camera.pos[2] + 1.0f;

	player->colliding = 0;
}

/*
 * Calculates the wish velocity;
*/
void	player_movement(t_player *player, GLFWwindow *win, t_fps fps)
{
	float	temp0[VEC3_SIZE];
	float	temp1[VEC3_SIZE];
	float	crossed[VEC3_SIZE];
	float	speed_multiplier = 1.0f;
	float	speed;

	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed_multiplier = 2.0f;
	speed = speed_multiplier * fps.delta_time;

	new_vec3(temp0, 0, 0, 0);
	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
		vec3_multiply_f(temp0, player->camera.front, speed);
	else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
		vec3_multiply_f(temp0, player->camera.front, -speed);

	new_vec3(temp1, 0, 0, 0);
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
	{
		vec3_cross(temp1, player->camera.front, player->camera.up);
		vec3_normalize(temp1, temp1);
		vec3_multiply_f(temp1, temp1, -speed);
	}
	else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
	{
		vec3_cross(temp1, player->camera.front, player->camera.up);
		vec3_normalize(temp1, temp1);
		vec3_multiply_f(temp1, temp1, speed);
	}

	vec3_add(player->velocity, player->velocity, temp0);
	vec3_add(player->velocity, player->velocity, temp1);

	player->moving = (player->velocity[0] || player->velocity[1] || player->velocity[2]);
}

void	player_apply_velocity(t_player *player)
{
	vec3_add(player->camera.pos, player->camera.pos, player->velocity);
	new_vec3(player->velocity, 0, 0, 0);
}

void	player_movement_old_basically_noclip(t_player *player, GLFWwindow *win, t_fps fps)
{
	float	temp[VEC3_SIZE];
	float	speed_multiplier = 1.0f;
	float	speed;

	if (glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		speed_multiplier = 2.0f;
	speed = speed_multiplier * fps.delta_time;

	if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS)
	{
		vec3_multiply_f(temp, player->camera.front, speed);
		vec3_add(player->camera.pos, player->camera.pos, temp);
	}
	else if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS)
	{
		vec3_multiply_f(temp, player->camera.front, speed);
		vec3_sub(player->camera.pos, player->camera.pos, temp);
	}
	if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS)
	{
		vec3_cross(temp, player->camera.front, player->camera.up);
		vec3_normalize(temp, temp);
		vec3_multiply_f(temp, temp, speed);
		vec3_sub(player->camera.pos, player->camera.pos, temp);
	}
	else if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS)
	{
		vec3_cross(temp, player->camera.front, player->camera.up);
		vec3_normalize(temp, temp);
		vec3_multiply_f(temp, temp, speed);
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

void	player_print(t_player *player)
{
	ft_printf("PLAYER :\n");
	vec3_string("player.velocity : ", player->velocity);
	ft_printf("player.moving : %d\n", player->moving);
	ft_printf("player.colliding : %d\n", player->colliding);
	camera_print(&player->camera);
}
