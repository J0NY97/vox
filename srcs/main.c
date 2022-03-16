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

/*
 * From face get nth vertex;
 * res[VEC3_SIZE];
 * vertex_index = 0-3;
 * face_index = 0-index_amount;
*/
float *nth(float *res, unsigned int *indices, float *vertices, int face_index, int vertex_index)
{
	int	vert;	

	vert = indices[face_index * 3 + vertex_index];
	res[0] = vertices[vert + 0];
	res[1] = vertices[vert + 1];
	res[2] = vertices[vert + 2];
	return (res);
}

int	main(void)
{
	t_shaderpixel	sp;
	init(&sp);
	t_scene	scene;
	create_scene(&scene);
	t_key	keys[GLFW_KEY_LAST];
	t_fps	fps;
	new_fps(&fps);

	t_player	player;
	new_player(&player);
	new_vec3(player.camera.pos, 0, 200, 0);
//	new_vec3(player.camera.pos, 16384, 80, 16384);
	player.camera.pitch = -20;
	player.camera.yaw = 50;
	player.camera.viewport_w = sp.win_w;
	player.camera.viewport_h = sp.win_h;

	t_shader	crosshair_shader;
	new_crosshair_shader(&crosshair_shader);

	t_shader	shader1;
	new_shader(&shader1, SHADER_PATH"simple.vs", SHADER_PATH"simple.fs");

	t_obj		retrotv_obj;
	obj_load(&retrotv_obj, MODEL_PATH"retrotv/retrotv.obj");

	t_entity	*retrotv = malloc(sizeof(t_entity));
	new_entity(retrotv);
	new_model(&retrotv->model, &retrotv_obj);
	//new_vec3(retrotv->pos, 0, 0, -2.5);
	new_vec3(retrotv->pos, 0, 90, 0);
	retrotv->collision_detection_enabled = 1;
	retrotv->collision_use_precise = 1;
	retrotv->render_aabb = 0;
	size_t	retrotv_index = add_entity_to_scene(&scene, retrotv);

	t_obj		dust2_obj;
//	obj_load(&dust2_obj, MODEL_PATH"de_dust2/de_dust2.obj");
	obj_load(&dust2_obj, MODEL_PATH"dust2_retry/dust2.obj");

	t_entity	*dust2 = malloc(sizeof(t_entity));
	new_entity(dust2);
	new_model(&dust2->model, &dust2_obj);
	//size_t	dust2_index = add_entity_to_scene(&scene, dust2);
	dust2->collision_detection_enabled = 1;
//	dust2->collision_use_precise = 1;
	dust2->rot_x_angle = -90;
	dust2->scale_value = 0.005;

	t_obj		display_obj;
	obj_load(&display_obj, MODEL_PATH"display/display.obj");

	t_entity	*display = malloc(sizeof(t_entity));
	new_entity(display);
	new_model(&display->model, &display_obj);
	size_t display_index = add_entity_to_scene(&scene, display);
	new_vec3(display->pos, 1.2, 0.6, -2.0);
	display->collision_detection_enabled = 1;
	display->scale_value = 0.1;
	display->rot_x_angle = 0;
	display->rot_y_angle = 90;
	display->rot_z_angle = 0;

	t_shader	mandelbrot_shader;
	new_shader(&mandelbrot_shader, SHADER_PATH"mandelbrot.vs", SHADER_PATH"mandelbrot.fs");
	t_fractal2d	fractal;
	new_fractal2d(&fractal);

	t_skybox	skybox;
	new_skybox(&skybox);

	t_obj		cube_obj;
	obj_load(&cube_obj, MODEL_PATH"cube/cube.obj");

	t_entity	*test_cube = malloc(sizeof(t_entity));
	new_entity(test_cube);
	new_model(&test_cube->model, &cube_obj);
	add_entity_to_scene(&scene, test_cube);
	new_vec3(test_cube->pos, 2, 0.6, -2.0);
	test_cube->scale_value = .1;
	test_cube->rot_x_angle = 0;
	test_cube->rot_y_angle = 0;
	test_cube->rot_z_angle = 0;

//////////////////////////////
	// Instance testing
//////////////////////////////
		t_shader	cube_shader;
		new_shader(&cube_shader, SHADER_PATH"simple_instance.vs", SHADER_PATH"simple_instance.fs");

		t_chunk_info	chunk_info;
		chunk_info.render_distance = 10;
		chunk_info.chunks_loaded = (int)chunk_info.render_distance * (int)chunk_info.render_distance;
		chunk_info.seed = 896868766;
		chunk_info.width = 16;
		chunk_info.breadth = 16;
		chunk_info.block_scale = 1.0f;
		chunk_info.block_size = chunk_info.block_scale * 2;
		chunk_info.chunk_size = chunk_info.width * chunk_info.block_scale * 2;

		int	chunk_reloading[2]; // 0 : reaload_amount, 1: reloaded amount
		chunk_reloading[0] = 0;
		chunk_reloading[1] = 0;
		t_chunk	chunks[chunk_info.chunks_loaded];	
		t_model	cube_model;
		new_model(&cube_model, &cube_obj);
		int		nth_chunk = 0;
		float	player_chunk[VEC2_SIZE];
		int		prev_player_chunk[VEC2_SIZE];
		int		start_coord[VEC2_SIZE];
		
		player_in_chunk(player_chunk, player.camera.pos, &chunk_info);
		start_coord[0] = player_chunk[0] - (chunk_info.render_distance / 2);
		start_coord[1] = player_chunk[1] - (chunk_info.render_distance / 2);
		prev_player_chunk[0] = player_chunk[0];
		prev_player_chunk[1] = player_chunk[1];

		for (int x = start_coord[0], x_amount = 0; x_amount < chunk_info.render_distance; x++, x_amount++)
		{
			for (int z = start_coord[1], z_amount = 0; z_amount < chunk_info.render_distance; z++, z_amount++)
			{
				new_model(&chunks[nth_chunk].model, &cube_obj);
				new_chunk(&chunks[nth_chunk], &chunk_info, (float []){x, 1, z});
				nth_chunk++;
			}
		}
		ft_printf("Chunks created : %d\n", nth_chunk);
//////////////////////////////
	// END Instance testing
//////////////////////////////

	int error = glGetError();
	if (error)
		LG_ERROR("gl errors before while : %d", error);

	float		rot_amount = 1;
	int			toggle_rot_x = 0;
	int			toggle_rot_y = 0;
	int			toggle_rot_z = 0;

	glfwSwapInterval(0);
	glEnable(GL_CULL_FACE);
	while (!glfwWindowShouldClose(sp.win))
	{
		glCullFace(GL_BACK);
		/*
		glDisable(GL_DEPTH_TEST);
		render_fractal2d(&fractal, &mandelbrot_shader);
		*/
//		ft_printf("FPS : %d\n", fps.fps);

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
			glPolygonMode(GL_FRONT_AND_BACK, sp.polygon_mode);
		}

		float		rot_amount_delta = rot_amount * 10.0f * fps.delta_time;
		t_entity	*selected_entity = get_scene_entity(&scene, display_index);

		if (keys[GLFW_KEY_KP_ENTER].state == GLFW_PRESS)
			rot_amount *= -1;

		if (keys[GLFW_KEY_KP_1].state == GLFW_PRESS)
			selected_entity->rot_x_angle += rot_amount;
		if (keys[GLFW_KEY_KP_2].state == GLFW_PRESS)
			selected_entity->rot_y_angle += rot_amount;
		if (keys[GLFW_KEY_KP_3].state == GLFW_PRESS)
			selected_entity->rot_z_angle += rot_amount;

		if (keys[GLFW_KEY_KP_4].state == GLFW_PRESS)
			toggle_rot_x = toggle_rot_x != 1;
		if (toggle_rot_x)
			selected_entity->rot_x_angle += rot_amount_delta;

		if (keys[GLFW_KEY_KP_5].state == GLFW_PRESS)
			toggle_rot_y = toggle_rot_y != 1;
		if (toggle_rot_y)
			selected_entity->rot_y_angle += rot_amount_delta;

		if (keys[GLFW_KEY_KP_6].state == GLFW_PRESS)
			toggle_rot_z = toggle_rot_z != 1;
		if (toggle_rot_z)
			selected_entity->rot_z_angle += rot_amount_delta;
		
		if (keys[GLFW_KEY_N].state == GLFW_PRESS)
			selected_entity->show_normal_map = selected_entity->show_normal_map != 1;

		if (keys[GLFW_KEY_P].state == GLFW_PRESS)
		{
			player_print(&player);
			entity_print(selected_entity);
		}

		if (keys[GLFW_KEY_5].state == GLFW_PRESS)
			vec3_new(player.camera.pos, 0, 200, 0);

		update_fps(&fps);
		player_events(&player, keys, sp.win);
		player_movement(&player, sp.win, fps);
		if (player.enabled_mouse)
			player_looking(&player, sp.win, fps);

		if (0)
		{
		size_t	entities_collisioned = 0;
		for (size_t i = 0; i < scene.entities_allocated && entities_collisioned < scene.entity_amount; i++)
		{
			if (scene.entities[i] && scene.entities[i]->collision_detection_enabled)
			{
				// creates model thingthong
				aabb_create(&scene.entities[i]->aabb,
					scene.entities[i]->model.info->mesh.vertices,
					scene.entities[i]->model.info->mesh.vertex_amount);
				// create vertices of them
				aabb_vertify(&scene.entities[i]->aabb);
				// transform all the vertices
				aabb_transform_new(&scene.entities[i]->aabb,
					scene.entities[i]->model_mat);

				aabb_create(&scene.entities[i]->aabb,
					scene.entities[i]->aabb.vertices, 8);


				// At some point, so it doenst get done somewhere im not sure where....
				// aabb_vertify();
					//player_entity_collision_precise(&player, scene.entities[i]);

				if (scene.entities[i]->collision_use_precise)
					testing_triangle_collision(&player, scene.entities[i]);
				else
					player_entity_collision(&player, scene.entities[i]);
				entities_collisioned += 1;
			}
		}
		}

		player_apply_velocity(&player);
		update_camera(&player.camera);

		glEnable(GL_DEPTH_TEST);
		size_t	entities_rendered = 0;
		for (size_t i = 0; i < scene.entities_allocated && entities_rendered < scene.entity_amount; i++)
		{
			if (scene.entities[i])
			{
				render_entity(scene.entities[i], &player.camera, &scene.entities[i]->model, &shader1);
				entities_rendered += 1;
			}
		}

/////////////////
		// Chunk things
/////////////////
		player_in_chunk(player_chunk, player.camera.pos, &chunk_info);
		if (1 && ((prev_player_chunk[0] != (int)(player_chunk[0]) ||
			prev_player_chunk[1] != (int)(player_chunk[1])) ||
			(chunk_reloading[0] != chunk_reloading[1])))
		{
			prev_player_chunk[0] = player_chunk[0];
			prev_player_chunk[1] = player_chunk[1];

			ft_timer_start();
		//	regenerate_chunks(chunks, &chunk_info, player_chunk);	
			regenerate_chunks_multi(chunk_reloading, chunks, &chunk_info, player_chunk);	
			ft_printf("Vol2 chunk update timer : %f\n", ft_timer_end());
		}

//		glCullFace(GL_BACK);
//		glFrontFace(GL_CCW);
		nth_chunk = 0;
		for (; nth_chunk < chunk_info.chunks_loaded; nth_chunk++)
			render_chunk(&chunks[nth_chunk], &player.camera, &cube_shader);
/////////////////
		// END Chunk things
/////////////////

		render_skybox(&skybox, &player.camera);

		glDisable(GL_DEPTH_TEST);
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
