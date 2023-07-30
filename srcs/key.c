/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 12:06:12 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 12:21:17 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vox.h"
#include "key.h"

void	update_all_keys(t_key *keys, t_key *mouse, GLFWwindow *win)
{
	int	i;
	int	state;

	// KEY
	i = 31; // apparently there are no keys in 0 - 31;
	while (++i < GLFW_KEY_LAST)	
	{
		state = glfwGetKey(win, i);
		if (state == GLFW_PRESS)
		{
			if (keys[i].state == BUTTON_NONE || keys[i].state == BUTTON_RELEASE)
				keys[i].state = BUTTON_PRESS;
			else
				keys[i].state = BUTTON_HOLD;
		}
		else if (state == GLFW_RELEASE)
		{
			if (keys[i].state == BUTTON_PRESS || keys[i].state == BUTTON_HOLD)
				keys[i].state = BUTTON_RELEASE;
			else
				keys[i].state = BUTTON_NONE;
		}
	}

	// MOUSE
	i = -1;
	while (++i < GLFW_MOUSE_BUTTON_LAST)
	{
		state = glfwGetMouseButton(win, i);
		if (state == GLFW_PRESS)
		{
			if (mouse[i].state == BUTTON_NONE || mouse[i].state == BUTTON_RELEASE)
				mouse[i].state = BUTTON_PRESS;
			else
				mouse[i].state = BUTTON_HOLD;
		}
		else if (state == GLFW_RELEASE)
		{
			if (mouse[i].state == BUTTON_PRESS || mouse[i].state == BUTTON_HOLD)
				mouse[i].state = BUTTON_RELEASE;
			else
				mouse[i].state = BUTTON_NONE;
		}
	}
}