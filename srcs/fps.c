#include "vox.h"

void	new_fps(t_fps *fps)
{
	fps->fps = 1;
	fps->count = 0;
	fps->delta_time = 0.1;
	fps->last_time = glfwGetTime();
}

void	update_fps(t_fps *fps)
{
	fps->curr_time = glfwGetTime();
	fps->delta_time = 1.0f / fps->fps;
	fps->count++;
	if (fps->curr_time - fps->last_time >= 1)
	{
		fps->last_time = fps->curr_time;
		fps->fps = fps->count;
		fps->count = 0;
	}
}
