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
	t_key	mouse[GLFW_MOUSE_BUTTON_LAST];
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
	size_t display_index = 0;
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

		t_player_info	player_info;

		player_info.reach = 5;
		player_info.equipped_block = BLOCK_STONE;

		t_chunk_info	chunk_info;

		chunk_info.render_distance = 15;
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

		chunk_info.block_collision_enabled = 0;
		chunk_info.player_collision_enabled = 0;
		chunk_info.fancy_graphics = 0;

		glGenTextures(1, &chunk_info.texture);
	//	new_texture(&chunk_info.texture, MODEL_PATH"cube/version_3_texture.bmp");
		new_texture(&chunk_info.texture, MODEL_PATH"cube/version_3_texture_alpha.bmp");

		t_chunk	*chunks;
		chunks = malloc(sizeof(t_chunk) * chunk_info.chunks_loaded);

		chunk_info.chunks = chunks;

		int		nth_chunk = 0;
		int		player_chunk[VEC3_SIZE];
		
		get_chunk_pos_from_world_pos(player_chunk, player.camera.pos, &chunk_info);

		LG_INFO("Inits done, lets create some chunks (%d wanted)\n", chunk_info.chunks_loaded);
		for (; nth_chunk < chunk_info.chunks_loaded; nth_chunk++)
		{
			new_chunk(&chunks[nth_chunk], &chunk_info, nth_chunk);
			chunks[nth_chunk].mesh.texture = chunk_info.texture;
			chunks[nth_chunk].liquid_mesh.texture = chunk_info.texture;
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
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

		update_all_keys(keys, mouse, sp.win);
		glfwPollEvents();
		if (keys[GLFW_KEY_ESCAPE].state == BUTTON_PRESS)
			glfwSetWindowShouldClose(sp.win, GLFW_TRUE);
		if (keys[GLFW_KEY_X].state == BUTTON_PRESS)
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

		if (keys[GLFW_KEY_KP_ENTER].state == BUTTON_PRESS)
			rot_amount *= -1;

		if (keys[GLFW_KEY_KP_1].state == BUTTON_PRESS)
			selected_entity->rot_x_angle += rot_amount;
		if (keys[GLFW_KEY_KP_2].state == BUTTON_PRESS)
			selected_entity->rot_y_angle += rot_amount;
		if (keys[GLFW_KEY_KP_3].state == BUTTON_PRESS)
			selected_entity->rot_z_angle += rot_amount;

		if (keys[GLFW_KEY_KP_4].state == BUTTON_PRESS)
			toggle_rot_x = toggle_rot_x != 1;
		if (toggle_rot_x)
			selected_entity->rot_x_angle += rot_amount_delta;

		if (keys[GLFW_KEY_KP_5].state == BUTTON_PRESS)
			toggle_rot_y = toggle_rot_y != 1;
		if (toggle_rot_y)
			selected_entity->rot_y_angle += rot_amount_delta;

		if (keys[GLFW_KEY_KP_6].state == BUTTON_PRESS)
			toggle_rot_z = toggle_rot_z != 1;
		if (toggle_rot_z)
			selected_entity->rot_z_angle += rot_amount_delta;
		
		if (keys[GLFW_KEY_N].state == BUTTON_PRESS)
			selected_entity->show_normal_map = selected_entity->show_normal_map != 1;

		if (keys[GLFW_KEY_P].state == BUTTON_PRESS)
		{
			player_print(&player);
//			entity_print(selected_entity);
		}

		if (keys[GLFW_KEY_R].state == BUTTON_PRESS)
			vec3_new(player.camera.pos, 0, 200, 0);
		if (keys[GLFW_KEY_LEFT].state == BUTTON_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){-10, 0, 0});
		if (keys[GLFW_KEY_RIGHT].state == BUTTON_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){10, 0, 0});
		if (keys[GLFW_KEY_UP].state == BUTTON_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){0, -10, 0});
		if (keys[GLFW_KEY_DOWN].state == BUTTON_PRESS)
			vec3_add(player.camera.pos, player.camera.pos, (float []){0, 10, 0});

		if (keys[GLFW_KEY_C].state == BUTTON_PRESS)
		{
			chunk_info.block_collision_enabled = chunk_info.block_collision_enabled != 1;
			if (chunk_info.block_collision_enabled)
				LG_INFO("Block collision => ON");
			else
				LG_INFO("Block collision => OFF");
		}

		if (keys[GLFW_KEY_V].state == BUTTON_PRESS)
		{
			chunk_info.player_collision_enabled = chunk_info.player_collision_enabled != 1;
			if (chunk_info.player_collision_enabled)
				LG_INFO("Player collision detection => ON");
			else
				LG_INFO("Player collision detection => OFF");
		}

		if (keys[GLFW_KEY_B].state == BUTTON_PRESS)
		{
			if (player.gravity == 0)
			{
				player.gravity = 0.98;
				LG_INFO("Gravity => ON");
			}
			else
			{
				player.gravity = 0;
				LG_INFO("Gravity => OFF");
			}
		}

		if (keys[GLFW_KEY_G].state == BUTTON_PRESS)
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

		// Force update all chunks
		if (keys[GLFW_KEY_T].state == BUTTON_PRESS)
		{
			LG_INFO("Force updating all chunks. (Chunks Loaded : %d)", chunk_info.chunks_loaded);
			for (int i = 0; i < chunk_info.chunks_loaded; i++)
				chunks[i].needs_to_update = 1;
		}

		// Toggle fancy graphics
		if (keys[GLFW_KEY_F].state == BUTTON_PRESS)
		{
			chunk_info.fancy_graphics = chunk_info.fancy_graphics != 1;
			if (chunk_info.fancy_graphics)
				LG_INFO("Fancy graphics => ON.");
			else
				LG_INFO("Fancy graphics => OFF.");
		}

// Select equipped block;
		if (keys[GLFW_KEY_1].state == BUTTON_PRESS)
			player_info.equipped_block = BLOCK_DIRT;
		if (keys[GLFW_KEY_2].state == BUTTON_PRESS)
			player_info.equipped_block = BLOCK_STONE;
		if (keys[GLFW_KEY_3].state == BUTTON_PRESS)
			player_info.equipped_block = BLOCK_SAND;
		if (keys[GLFW_KEY_4].state == BUTTON_PRESS)
			player_info.equipped_block = BLOCK_OAK_LOG;
		if (keys[GLFW_KEY_5].state == BUTTON_PRESS)
			player_info.equipped_block = BLOCK_OAK_PLANK;
		if (keys[GLFW_KEY_6].state == BUTTON_PRESS)
			player_info.equipped_block = BLOCK_OAK_LEAF;
		if (keys[GLFW_KEY_0].state == BUTTON_PRESS)
			player_info.equipped_block = ITEM_TREE_PLACER;

		update_fps(&fps);
		player_events(&player, keys, sp.win);
//		player_movement(&player, sp.win, fps);
		if (player.gravity == 0)
			player_movement_creative(&player, sp.win, fps);
		if (player.gravity != 0)
			player_movement_survival(&player, sp.win, fps);
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

/*
		player_apply_velocity(&player);
		update_camera(&player.camera);
		*/

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
		get_chunk_pos_from_world_pos(player_chunk, player.camera.pos, &chunk_info);

		int	tobegen = 0;
		if (regen_chunks)
		{
		//	regenerate_chunks_v3(chunks, &chunk_info, player_chunk, &tm);
			tobegen = regenerate_chunks(chunks, &chunk_info, player_chunk);	
		}
		// If we dont have to generate more chunks, we will generate the trees for them;
		if (!tobegen)
			for (int c = 0; c < chunk_info.chunks_loaded; c++)
				if (!chunks[c].has_tree)
					tree_gen(&chunks[c]);

		thread_manager_check_threadiness(&tm);

		for (int ent= 0; ent < chunk_info.chunks_loaded; ++ent)
		{
			if (chunks[ent].needs_to_update)
			{
				update_chunk_visible_blocks(&chunks[ent]);
				update_chunk_mesh(&chunks[ent].mesh);
				update_chunk_mesh(&chunks[ent].liquid_mesh);
				chunk_aabb_update(&chunks[ent]);
				chunks[ent].needs_to_update = 0;

				// Update all 6 neighbors of the chunk;
				t_chunk *neighbor;
				for (int i = 0; i < 6; i++)
				{
					neighbor = get_adjacent_chunk(&chunks[ent], chunk_info.chunks, (int *)g_neighbors[i]);
					if (neighbor)
					{
						update_chunk_visible_blocks(neighbor);
						update_chunk_mesh(&neighbor->mesh);
						update_chunk_mesh(&neighbor->liquid_mesh);
						chunk_aabb_update(neighbor);
					}
				}
			}
		}

		player_terrain_collision(player.velocity, player.camera.pos, player.velocity, chunks);

		player_apply_velocity(&player);
		update_camera(&player.camera);

	// Used for block collision
		float	intersect_point[5][3];
		int		intersect_chunk_index[5]; // correspond with the index in 'intersect_point';
		float	closest_point[3];
		float	closest_dist;
		int		closest_index = -1; // the index of the chunk that has the closest collision;
		float	block_pos[3];
		int		face = -1; // -1 is no face;
		int		collision_result = 0; // will be the amount of collisions that has happened;
	
		nth_chunk = 0;
		int sent_to_gpu = 0;
		glDisable(GL_BLEND);
		for (; nth_chunk < chunk_info.chunks_loaded; ++nth_chunk)
		{
			// Decide if we want to render the chunk or not;
			// Dont render chunk if the chunk is further away than the farplane of the camear;
			// Dont render if the chunk is outside the view fustrum;
			if (vec3_dist(player.camera.pos, chunks[nth_chunk].world_coordinate) <
				player.camera.far_plane + chunks[nth_chunk].info->chunk_size[0] &&
				aabb_in_frustum(&chunks[nth_chunk].aabb, &player.camera.frustum))
			{
				if (chunks[nth_chunk].blocks_solid_amount > 0)
					render_chunk_mesh(&chunks[nth_chunk].mesh, chunks[nth_chunk].world_coordinate, &player.camera, &cube_shader_v2);
				chunks[nth_chunk].render = 1;
			}
			else
				chunks[nth_chunk].render = 0;

			if (chunks[nth_chunk].blocks_solid_amount > 0 ||
				chunks[nth_chunk].blocks_liquid_amount > 0)
			{
				// Collision Detection
				if ((chunk_info.block_collision_enabled ||
					chunk_info.player_collision_enabled) &&
					chunks[nth_chunk].blocks_solid_amount > 0 &&
					vec3i_dist(player_chunk, chunks[nth_chunk].coordinate) < 2)
				{
					show_chunk_borders(&chunks[nth_chunk], &player.camera, (float []){1, 0, 0});
					// Place Blocking and Removing;
					// Go through all chunks and check for collision on blocks,
					// store intersections and indices of the chunk the intersection
					// is in;
					if (chunk_info.block_collision_enabled)
					{
						int collisions_on_chunk = chunk_mesh_collision(player.camera.pos, player.camera.front, &chunks[nth_chunk], player_info.reach, intersect_point + collision_result);
						for (int i = 0; i < collisions_on_chunk; i++)
							intersect_chunk_index[collision_result + i] = nth_chunk;
						collision_result += collisions_on_chunk;
					}
				}
			}
		}

		/* START OF COLLISION */
		// Save the closest point, of a maximum 16 points
		//	gotten from chunk_mesh_collision, in the closest_point var;
		// Also the index of which chunk the collision is in;
		if (collision_result > 0)
		{
			float	distancione = -1;
			closest_dist = INFINITY;
			for (int colli = 0; colli < collision_result; ++colli)
			{
				distancione = vec3_dist(player.camera.pos, intersect_point[colli]);
				if (distancione < closest_dist)
				{
					closest_dist = distancione;
					vec3_assign(closest_point, intersect_point[colli]);
					closest_index = intersect_chunk_index[colli];
				}
			}

			t_block *hovered_block = NULL;
			int		clicked = 0;
			if (mouse[GLFW_MOUSE_BUTTON_LEFT].state == BUTTON_PRESS ||
				mouse[GLFW_MOUSE_BUTTON_RIGHT].state == BUTTON_PRESS)
				clicked = 1;
			hovered_block = get_block_from_chunk_mesh(&chunks[closest_index], closest_point, block_pos, &face);
			if (hovered_block)
			{
				render_block_outline(block_pos, (float []){0, 0, 0}, player.camera.view, player.camera.projection);
			}
			if (hovered_block && clicked)
			{
				if (mouse[GLFW_MOUSE_BUTTON_LEFT].state == BUTTON_PRESS)
					hovered_block->type = BLOCK_AIR;
				else if (mouse[GLFW_MOUSE_BUTTON_RIGHT].state == BUTTON_PRESS)
				{
					float	block_world[3];
					vec3_assign(block_world, block_pos);
					if (face == FACE_FRONT)
						block_world[2] += 1.0f;
					else if (face == FACE_BACK)
						block_world[2] -= 1.0f;
					else if (face == FACE_LEFT)
						block_world[0] -= 1.0f;
					else if (face == FACE_RIGHT)
						block_world[0] += 1.0f;
					else if (face == FACE_TOP)
						block_world[1] += 1.0f;
					else if (face == FACE_BOT)
						block_world[1] -= 1.0f;

					int		chunk_pos[3];
					int		local_pos[3];
					int		index;
					get_chunk_pos_from_world_pos(chunk_pos, block_world, &chunk_info);
					t_chunk *next_chunk = get_chunk(&chunk_info, chunk_pos);
					if (next_chunk)
					{
						get_block_local_pos_from_world_pos(local_pos, block_world);
						index = get_block_index(&chunk_info, local_pos[0], local_pos[1], local_pos[2]);
						// Check if block or item equipped;
						if (player_info.equipped_block < BLOCK_TYPE_AMOUNT)
						{
							next_chunk->blocks[index].type = player_info.equipped_block;
							next_chunk->has_blocks = 1;
						}
						else if (player_info.equipped_block > ITEM_FIRST &&
							player_info.equipped_block < ITEM_LAST)
						{
							if (player_info.equipped_block == ITEM_TREE_PLACER)
								tree_placer(chunks, block_world);
						}
						LG_INFO("Block placed in chunk <%d %d %d> at (world : %f %f %f) (local : %d %d %d) (index : %d)",
							next_chunk->coordinate[0], next_chunk->coordinate[1], next_chunk->coordinate[2],
							block_world[0], block_world[1], block_world[2],
							local_pos[0], local_pos[1], local_pos[2],
							index);
						next_chunk->needs_to_update = 1;
					}
				}
				chunks[closest_index].needs_to_update = 1;
			}
		}
		/* END OF COLLISION */

		glEnable(GL_BLEND);
		nth_chunk = 0;
		for (; nth_chunk < chunk_info.chunks_loaded; ++nth_chunk)
			if (chunks[nth_chunk].render && chunks[nth_chunk].blocks_liquid_amount > 0)
				render_chunk_mesh(&chunks[nth_chunk].liquid_mesh, chunks[nth_chunk].world_coordinate, &player.camera, &cube_shader_v2);
		glDisable(GL_BLEND);

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
