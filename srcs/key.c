#include "shaderpixel.h"

void	update_all_keys(t_key *keys, GLFWwindow *win)
{
	int	i;
	int	state;

	i = -1;
	while (++i < GLFW_KEY_LAST)	
	{
		state = glfwGetKey(win, i);
		if (state == GLFW_PRESS)
		{
			if (keys[i].state == 0)
				keys[i].state = 1;
			else
				keys[i].state = 2;
		}
		else if (state == GLFW_RELEASE)
			keys[i].state = 0;
	}
}