#include "shaderpixel.h"

void	init(t_shaderpixel *sp)
{
	//lg_setFdLevel(LEVEL_DEBUG);
	lg_setFdLevel(LEVEL_INFO);

	if (!glfwInit())
		LG_ERROR("Couldn\'t init glfw.");

#ifdef __APPLE__
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	sp->win_w = 1280;
	sp->win_h = 720;
	sp->win = glfwCreateWindow(sp->win_w, sp->win_h, "Shader Pixel", NULL, NULL);

	char *buffer;
	if (!sp->win)
	{
		glfwGetError(&buffer);
		LG_ERROR("Couldn\'t create window. (%s)", buffer);
	}

	glfwMakeContextCurrent(sp->win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		LG_ERROR("Couldn\'t load glad.");
	glViewport(0, 0, sp->win_w, sp->win_h);
	sp->polygon_mode = GL_FILL;
	sp->pilpalpol = 0;

	LG_INFO("Init Done");
}

void	uninit(t_shaderpixel *sp)
{
	(void)sp;
	glfwTerminate();
}

int	main(void)
{
	t_shaderpixel	sp;
	init(&sp);

	t_list	*entity_collision_list = NULL;

	t_key	keys[GLFW_KEY_LAST];

	t_fps	fps;
	new_fps(&fps);

	t_player	player;
	new_player(&player);

	new_vec3(player.camera.pos, 2.5, 0.5, 0.5);
	player.camera.yaw = -126;
	player.camera.viewport_w = sp.win_w;
	player.camera.viewport_h = sp.win_h;

	t_obj		retrotv_obj;
	ft_timer_start();
	obj_load(&retrotv_obj, MODEL_PATH"retrotv/retrotv.obj");
	ft_printf("retro time : %f\n", ft_timer_end());
	t_model		retrotv_model;
	new_model(&retrotv_model, &retrotv_obj);
	t_entity	retrotv;
	new_entity(&retrotv);
	retrotv.model = &retrotv_model;
	new_vec3(retrotv.pos, 0, 0, -2.5);

	add_to_list(&entity_collision_list, &retrotv, 0);

	t_shader	shader1;
	new_shader(&shader1, SHADER_PATH"simple.vs", SHADER_PATH"simple.fs");

	t_obj		dust2_obj;
	ft_timer_start();
	obj_load(&dust2_obj, MODEL_PATH"de_dust2/de_dust2.obj");
	ft_printf("dust time : %f\n", ft_timer_end());
	t_model		dust2_model;
	new_model(&dust2_model, &dust2_obj);
	t_entity	dust2;
	new_entity(&dust2);
	dust2.rot_x_angle = -90;
	dust2.scale_value = 0.005;

	t_obj		display_obj;
	obj_load(&display_obj, MODEL_PATH"display/display.obj");
	t_model		display_model;
	new_model(&display_model, &display_obj);
	t_entity	display;
	new_entity(&display);
	new_vec3(display.pos, 1.2, 0.6, -2.0);
	display.scale_value = 0.1;
	display.rot_x_angle = 0;
	display.rot_y_angle = 90;
	display.rot_z_angle = 0;

	t_shader	mandelbrot_shader;
	new_shader(&mandelbrot_shader, SHADER_PATH"mandelbrot.vs", SHADER_PATH"mandelbrot.fs");

	t_fractal2d	fractal;
	new_fractal2d(&fractal);

	t_shader	crosshair_shader;
	new_crosshair_shader(&crosshair_shader);

	int error = glGetError();
	if (error)
		LG_ERROR("gl errors before while : %d", error);

	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(sp.win))
	{
		glDisable(GL_DEPTH_TEST);
		render_fractal2d(&fractal, &mandelbrot_shader);

		update_all_keys(keys, sp.win);
		glfwPollEvents();
		if (glfwGetKey(sp.win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(sp.win, GLFW_TRUE);
		if (keys[GLFW_KEY_X].state == GLFW_PRESS)
		{
			sp.pilpalpol = (sp.pilpalpol + 1) % 3;
			if (sp.pilpalpol == 0)
				sp.polygon_mode = GL_FILL;
			else if (sp.pilpalpol == 1)
				sp.polygon_mode = GL_LINE;
			else if (sp.pilpalpol == 2)
				sp.polygon_mode = GL_POINT;
		}

		int			rot_amount = 1;
		t_entity	*which_entity = &display;
		if (keys[GLFW_KEY_KP_1].state == GLFW_PRESS)
		{
			if (keys[GLFW_KEY_LEFT_SHIFT].state)
				rot_amount = -rot_amount;
			which_entity->rot_x_angle += rot_amount;
		}
		if (keys[GLFW_KEY_KP_2].state == GLFW_PRESS)
		{
			if (keys[GLFW_KEY_LEFT_SHIFT].state)
				rot_amount = -rot_amount;
			which_entity->rot_y_angle += rot_amount;
		}
		if (keys[GLFW_KEY_KP_3].state == GLFW_PRESS)
		{
			if (keys[GLFW_KEY_LEFT_SHIFT].state)
				rot_amount = -rot_amount;
			which_entity->rot_z_angle += rot_amount;
		}
		if (keys[GLFW_KEY_P].state == GLFW_PRESS)
		{
			player_print(&player);
			entity_print(which_entity);
		}


		update_fps(&fps);
		player_events(&player, keys, sp.win);
		player_movement(&player, sp.win, fps);
		if (player.enabled_mouse)
			player_looking(&player, sp.win, fps);

		entity_collision_detection(entity_collision_list, player.camera.pos);

		player_entity_collision(&player, &retrotv);

		player_apply_velocity(&player);

		glEnable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, sp.polygon_mode);

		update_camera(&player.camera);

		render_entity(&retrotv, &player.camera, &retrotv_model, &shader1);
		render_entity(&dust2, &player.camera, &dust2_model, &shader1);
		render_entity(&display, &player.camera, &display_model, &shader1);

		render_crosshair();

		glfwSwapBuffers(sp.win);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		error = glGetError();
		if (error)
			LG_ERROR("errors in while : %d", error);
	}

	uninit(&sp);
	return (0);
}
