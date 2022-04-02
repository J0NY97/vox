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
	new_vec3(player.camera.pos, 5, 85, 5);
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
//	size_t	retrotv_index = add_entity_to_scene(&scene, retrotv);
	retrotv->collision_detection_enabled = 0;
	retrotv->collision_use_precise = 0;
	retrotv->render_aabb = 0;

	t_obj		dust2_obj;
//	obj_load(&dust2_obj, MODEL_PATH"de_dust2/de_dust2.obj");
	obj_load(&dust2_obj, MODEL_PATH"dust2_retry/dust2.obj");

	t_entity	*dust2 = malloc(sizeof(t_entity));
	new_entity(dust2);
	new_model(&dust2->model, &dust2_obj);
//	size_t	dust2_index = add_entity_to_scene(&scene, dust2);
	dust2->collision_detection_enabled = 0;
//	dust2->collision_use_precise = 1;
	dust2->rot_x_angle = -90;
	dust2->scale_value = 0.005;

	t_obj		display_obj;
	obj_load(&display_obj, MODEL_PATH"display/display.obj");

	t_entity	*display = malloc(sizeof(t_entity));
	new_entity(display);
	new_model(&display->model, &display_obj);
	size_t display_index;
//	display_index = add_entity_to_scene(&scene, display);
	new_vec3(display->pos, 1.2, 90, -2.0);
	display->collision_detection_enabled = 0;
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
	new_vec3(test_cube->pos, 2, 90, -2.0);
	test_cube->scale_value = 0.1f;
	test_cube->rot_x_angle = 0;
	test_cube->rot_y_angle = 0;
	test_cube->rot_z_angle = 0;
	test_cube->collision_detection_enabled = 1;
	test_cube->collision_use_precise = 1;
	test_cube->render_aabb = 0;

//////////////////////////////
	// Instance testing
//////////////////////////////
		t_shader	cube_shader;
		new_shader(&cube_shader, SHADER_PATH"simple_instance.vs", SHADER_PATH"simple_instance.fs");
		t_shader	cube_shader_v2;
		new_shader(&cube_shader_v2, SHADER_PATH"block_mesh.vs", SHADER_PATH"block_mesh.fs");

		t_thread_manager	tm;
		thread_manager_new(&tm, 64);

		int	regen_chunks = 1;

		t_chunk_info	chunk_info;

		chunk_info.render_distance = 20;
//		chunk_info.seed = 896868766;
		chunk_info.seed = 596547633;
		chunk_info.width = 16;
		chunk_info.breadth = 16;
		chunk_info.height = 16;
		chunk_info.y_chunk_amount = 256 / chunk_info.height;
		chunk_info.chunks_loaded = chunk_info.y_chunk_amount * ((int)chunk_info.render_distance * (int)chunk_info.render_distance);
		chunk_info.block_scale = 0.5f;
		chunk_info.block_size = chunk_info.block_scale * 2;
		chunk_info.chunk_size[0] = chunk_info.width * chunk_info.block_size;
		chunk_info.chunk_size[1] = chunk_info.height * chunk_info.block_size;
		chunk_info.chunk_size[2] = chunk_info.breadth * chunk_info.block_size;

/*
		chunk_info.hash_table_size = chunk_info.chunks_loaded;
		chunk_info.hash_table = malloc(sizeof(t_hash_item) * chunk_info.hash_table_size);
		hash_table_clear(chunk_info.hash_table, chunk_info.hash_table_size);
		*/

		chunk_info.chunk_collision_enabled = 0;

		glGenTextures(1, &chunk_info.texture);
		new_texture(&chunk_info.texture, MODEL_PATH"cube/version_3_texture.bmp");

		t_chunk	*chunks;
		chunks = malloc(sizeof(t_chunk) * chunk_info.chunks_loaded);

		chunk_info.chunks = chunks;

		int		nth_chunk = 0;
		float	player_chunk[VEC3_SIZE];
		
		player_in_chunk(player_chunk, player.camera.pos, &chunk_info);

		LG_INFO("Inits done, lets create some chunks (%d wanted)\n", chunk_info.chunks_loaded);
		for (; nth_chunk < chunk_info.chunks_loaded; nth_chunk++)
		{
			new_chunk(&chunks[nth_chunk], &chunk_info, nth_chunk);
			chunks[nth_chunk].mesh.texture = chunk_info.texture;
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
	glEnable(GL_LINE_SMOOTH);

	char	fps_str[10];
	while (!glfwWindowShouldClose(sp.win))
	{
		glCullFace(GL_BACK);
		/*
		glDisable(GL_DEPTH_TEST);
		render_fractal2d(&fractal, &mandelbrot_shader);
		*/
//		ft_printf("FPS : %d\n", fps.fps);
		if (fps.count % 60 == 0)
		{
			ft_b_itoa(fps.fps, fps_str);
			glfwSetWindowTitle(sp.win, fps_str);
		}

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
//			entity_print(selected_entity);
		}

		if (keys[GLFW_KEY_5].state == GLFW_PRESS)
			vec3_new(player.camera.pos, 0, 200, 0);
		if (keys[GLFW_KEY_LEFT].state == GLFW_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){-10, 0, 0});
		if (keys[GLFW_KEY_RIGHT].state == GLFW_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){10, 0, 0});
		if (keys[GLFW_KEY_UP].state == GLFW_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){0, -10, 0});
		if (keys[GLFW_KEY_DOWN].state == GLFW_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){0, 10, 0});

		if (keys[GLFW_KEY_C].state == GLFW_PRESS)
		{
			chunk_info.chunk_collision_enabled = chunk_info.chunk_collision_enabled != 1;
			if (chunk_info.chunk_collision_enabled)
				LG_INFO("Collision detection of chunks turned ON.");
			else
				LG_INFO("Collision detection of chunks turned OFF.");

		}

		if (keys[GLFW_KEY_G].state == GLFW_PRESS)
		{
			regen_chunks = regen_chunks != 1;
			if (regen_chunks)
				LG_INFO("Regeneration of chunks turned ON.");
			else
				LG_INFO("Regeneration of chunks turned OFF.");

			// TESTING ///
			int	most_vertices = 0;
			int	most_textures = 0;
			int	most_indices = 0;
			for (int j = 0; j < chunk_info.chunks_loaded; j++)
			{
				if (chunks[j].mesh.vertices_amount > most_vertices)
					most_vertices = chunks[j].mesh.vertices_amount;
				if (chunks[j].mesh.texture_id_amount > most_textures)
					most_textures = chunks[j].mesh.texture_id_amount;
				if (chunks[j].mesh.indices_amount > most_indices)
					most_indices = chunks[j].mesh.indices_amount;
			}
			LG_INFO("Most vertices : %d", most_vertices);
			LG_INFO("Most texture ids : %d", most_textures);
			LG_INFO("Most indices : %d", most_indices);
			// TESTING ///
		}

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

				if (scene.entities[i]->collision_use_precise)
					player_entity_mesh_collision(&player, scene.entities[i]);
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

		if (regen_chunks)
		{
		//	regenerate_chunks(chunk_reloading, chunks, &chunk_info, player_chunk);	
			regenerate_chunks_v32(chunks, &chunk_info, player_chunk);	
		}

		thread_manager_check_threadiness(&tm);

		nth_chunk = 0;
		int sent_to_gpu = 0;
		for (; nth_chunk < chunk_info.chunks_loaded; nth_chunk++)
		{
			if (chunks[nth_chunk].needs_to_update)
			{
				update_chunk_visible_blocks(&chunks[nth_chunk]);
				update_chunk_mesh(&chunks[nth_chunk]);
				chunk_aabb_update(&chunks[nth_chunk]);
				chunks[nth_chunk].needs_to_update = 0;

				// Update all 6 neighbors of the chunk;
				t_chunk *neighbor;
				for (int i = 0; i < 6; i++)
				{
					neighbor = get_adjacent_chunk(&chunks[nth_chunk], chunk_info.chunks, (int *)g_neighbors[i]);
					if (neighbor)
					{
						update_chunk_visible_blocks(neighbor);
						update_chunk_mesh(neighbor);
						chunk_aabb_update(neighbor);
					}
				}
			}
			if (chunks[nth_chunk].blocks_visible_amount > 0)
			{
				// Dont render chunk if the chunk is further away than the farplane of the camear;
				// Dont render if the chunk is outside the view fustrum;
				if (vec3_dist(player.camera.pos, chunks[nth_chunk].world_coordinate) <
					player.camera.far_plane + chunks[nth_chunk].info->chunk_size[0] &&
					aabb_in_frustum(&chunks[nth_chunk].aabb, &player.camera.frustum))
				{
					render_chunk_mesh(&chunks[nth_chunk], &player.camera, &cube_shader_v2);
					sent_to_gpu++;
				}

				// Collision Detection
				if (chunk_info.chunk_collision_enabled &&
					vec3_dist(player_chunk, (float []){chunks[nth_chunk].coordinate[0], chunks[nth_chunk].coordinate[1], chunks[nth_chunk].coordinate[2]}) < 2)
				{
					float	intersect_point[3];
					int		block_pos[3];
					int		face = -1; // -1 is no face;
					int		collision_result = 0;
					show_chunk_borders(&chunks[nth_chunk], &player.camera, (float []){1, 0, 0});
					collision_result = chunk_mesh_collision(player.camera.pos, player.camera.front, &chunks[nth_chunk], intersect_point);
						/*
					if (player.moving)
						collision_result = chunk_mesh_collision(player.camera.pos, player.velocity, &chunks[nth_chunk], intersect_point);
						*/
					if (collision_result)
					{
						t_block *hovered_block = NULL;
						int		clicked = 0;
						if (glfwGetMouseButton(sp.win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
							glfwGetMouseButton(sp.win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
							clicked = 1;
						hovered_block = get_block_from_chunk_mesh(&chunks[nth_chunk], intersect_point, block_pos, &face);
						render_block_outline(block_pos, (float []){0, 0, 0}, player.camera.view, player.camera.projection);
						if (hovered_block && clicked)
						{
							if (glfwGetMouseButton(sp.win, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
								hovered_block->type = BLOCK_AIR;
							else if (glfwGetMouseButton(sp.win, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
								hovered_block->type = BLOCK_STONE;
							chunks[nth_chunk].needs_to_update = 1;
						}
					}
				}
			}
			if ((int)player_chunk[0] == chunks[nth_chunk].coordinate[0] &&
				(int)player_chunk[1] == chunks[nth_chunk].coordinate[1] &&
				(int)player_chunk[2] == chunks[nth_chunk].coordinate[2])
			{
				/*
				show_chunk_borders(&chunks[nth_chunk], &player.camera, (float []){1, 0, 0});

				t_chunk	*north_chunk = get_adjacent_chunk(&chunks[nth_chunk], chunks, (int []){0, 0, -1});
				if (north_chunk)
					show_chunk_borders(north_chunk, &player.camera, (float []){0, 0, 1});
				t_chunk	*south_chunk = get_adjacent_chunk(&chunks[nth_chunk], chunks, (int []){0, 0, 1});
				if (south_chunk)
					show_chunk_borders(south_chunk, &player.camera, (float []){0, 1, 0});
				t_chunk	*east_chunk = get_adjacent_chunk(&chunks[nth_chunk], chunks, (int []){1, 0, 0});
				if (east_chunk)
					show_chunk_borders(east_chunk, &player.camera, (float []){0, 1, 1});
				t_chunk	*west_chunk = get_adjacent_chunk(&chunks[nth_chunk], chunks, (int []){-1, 0, 0});
				if (west_chunk)
					show_chunk_borders(west_chunk, &player.camera, (float []){1, 1, 0});
				*/
			}
		}
//		ft_printf("CPU : %d, GPU : %d\n", chunk_info.chunks_loaded, sent_to_gpu);

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
