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

	new_vec3(player.camera.pos, 0, 0, 2.5);
	player.camera.viewport_w = sp.win_w;
	player.camera.viewport_h = sp.win_h;

	t_obj		retrotv_obj;
	obj_load(&retrotv_obj, MODEL_PATH"retrotv/retrotv.obj");
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
	obj_load(&dust2_obj, MODEL_PATH"de_dust2/de_dust2.obj");
	t_model		dust2_model;
	new_model(&dust2_model, &dust2_obj);
	t_entity	dust2;
	new_entity(&dust2);
	dust2.rot_z_angle = -90;
	dust2.scale_value = 0.005;

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
		glfwPollEvents();
		if (glfwGetKey(sp.win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(sp.win, GLFW_TRUE);

		update_all_keys(keys, sp.win);

		update_fps(&fps);
		player_events(&player, keys, sp.win);
		player_movement(&player, sp.win, fps);
		if (player.enabled_mouse)
			player_looking(&player, sp.win, fps);
		entity_collision_detection(entity_collision_list, player.camera.pos);

////////////////////////
		float p1[VEC3_SIZE];
		new_vec3(p1, retrotv.aabb.min[0], retrotv.aabb.max[1], retrotv.aabb.max[2]);
		float p2[VEC3_SIZE];
		new_vec3(p2, retrotv.aabb.max[0], retrotv.aabb.max[1], retrotv.aabb.max[2]);
		float p3[VEC3_SIZE];
		new_vec3(p3, retrotv.aabb.min[0], retrotv.aabb.min[1], retrotv.aabb.max[2]);

		line_triangle_intersect(player.camera.pos, player.camera.front, p1, p2, p3);
////////////////////////

		player_apply_velocity(&player);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);
		render_fractal2d(&fractal, &mandelbrot_shader);

		glEnable(GL_DEPTH_TEST);
		update_camera(&player.camera);

		render_entity(&retrotv, &player.camera, &retrotv_model, &shader1);
		//render_entity(&dust2, &player.camera, &dust2_model, &shader1);

//// 2D / UI /////
		render_2d_line((float []){-1, 0.5, 0}, (float []){0, 0, 0}, (float []){1, 1, 1});

		glfwSwapBuffers(sp.win);

		error = glGetError();
		if (error)
			LG_ERROR("errors in while : %d", error);
	}

	uninit(&sp);
	return (0);
}
