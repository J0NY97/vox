#include "shaderpixel.h"
#include "chunk.h"
#include "shader.h"
#include "bobj.h"
#include "enum.h"
//#include "ui_manager.h"
//#include "ui.h"

void	init(t_shaderpixel *sp)
{
	//lg_setFdLevel(LEVEL_DEBUG);
	lg_setFdLevel(LEVEL_INFO);

	if (!glfwInit())
		LG_ERROR("Couldn\'t init glfw.");

#ifdef __APPLE__
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

	sp->win_w = 1280;
	sp->win_h = 720;
	sp->win = glfwCreateWindow(sp->win_w, sp->win_h, "MyPixel", NULL, NULL);

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
 * res[V3_SIZE];
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

void	scop_stuff(t_shaderpixel *sp)
{
	t_player	player;
	new_player(&player);
	v3_new(player.camera.pos, 5, 70, 5);
//	v3_new(player.camera.pos, 16384, 80, 16384);
	player.camera.pitch = 0;
	player.camera.yaw = -90;
	player.camera.viewport_w = sp->win_w;
	player.camera.viewport_h = sp->win_h;

	player.gravity = 0;

	t_scene	scene;
	create_scene(&scene);
	GLuint	shader1;
	new_shader(&shader1, SHADER_PATH"simple.vs", SHADER_PATH"simple.fs");
	t_obj		retrotv_obj;
	obj_load(&retrotv_obj, MODEL_PATH"retrotv/retrotv.obj");
	t_entity	*retrotv = malloc(sizeof(t_entity));
	new_entity(retrotv);
	new_model(&retrotv->model, &retrotv_obj);
	//v3_new(retrotv->pos, 0, 0, -2.5);
	v3_new(retrotv->pos, 0, 90, 0);
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
	v3_new(display->pos, 1.2, 90, -2.0);
	display->collision_detection_enabled = 0;
	display->scale_value = 0.1;
	display->rot_x_angle = 0;
	display->rot_y_angle = 90;
	display->rot_z_angle = 0;

	float		rot_amount = 1;
	int			toggle_rot_x = 0;
	int			toggle_rot_y = 0;
	int			toggle_rot_z = 0;

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

		glEnable(GL_DEPTH_TEST);
		size_t	entities_rendered = 0;
		for (size_t i = 0; i < scene.entities_allocated && entities_rendered < scene.entity_amount; i++)
		{
			if (scene.entities[i])
			{
				render_entity(scene.entities[i], &player.camera, &scene.entities[i]->model, shader1);
				entities_rendered += 1;
			}
		}
	}
}

int	main(void)
{
	t_shaderpixel	sp;
	t_key			keys[GLFW_KEY_LAST];
	t_key			mouse[GLFW_MOUSE_BUTTON_LAST];
	t_fps			fps;

	init(&sp);
	new_fps(&fps);

	//////////////////
	// PLAYER
	//////////////////
	t_player	player;
	new_player(&player);
	v3_new(player.camera.pos, 5000, 90, 5000);
//	new_vec3(player.camera.pos, 16384, 80, 16384);
	player.camera.pitch = 0;
	player.camera.yaw = -90;
	player.camera.viewport_w = sp.win_w;
	player.camera.viewport_h = sp.win_h;
	player.gravity = 0;


	////////////////////////////////
	// MODEL / BOBJ / ENTITY
	////////////////////////////////

	GLuint		model_shader;
	t_bobj		bobj_melon_golem;
	t_model_v2	model_melon_golem;
	float		melon_golem_pos[3];
	int			attach_entity = 1;

	new_shader(&model_shader, SHADER_PATH"model.vs", SHADER_PATH"model.fs");
	bobj_load(&bobj_melon_golem, MODEL_PATH"melon_golem/melon_golem.obj");
	model_from_bobj(&model_melon_golem, &bobj_melon_golem, 0);

	// Load vox entity models
	t_bobj		vox_entity_bobj[ENTITY_AMOUNT]; // NOTE : remember to free;
	t_model_v2	vox_entity_models[ENTITY_AMOUNT];

	GLuint		model_instance_shader;
	new_shader(&model_instance_shader, SHADER_PATH"model_instance.vs", SHADER_PATH"model_instance.fs");
	t_model_v2	instance_model;
	for (int i = 0; i < ENTITY_AMOUNT; i++)
	{
		bobj_load(&vox_entity_bobj[i], g_entity_data[i].model_path);
	//	model_from_bobj(&vox_entity_models[i], &vox_entity_bobj[i], 0);
		model_instance_from_bobj(&instance_model, &vox_entity_bobj[i], 0);
		model_update(&instance_model);
	}
	LG_INFO("All entity models loaded (%d)", ENTITY_AMOUNT);

	// Create entities (DEBUG)
	int				entity_amount = 64660;
	t_vox_entity	*entities = malloc(sizeof(t_vox_entity) * entity_amount);
	float	*model_matrices;
	model_matrices = malloc(sizeof(float) * entity_amount * 16);
//	free(model_matrices);
	for (int i = 0; i < entity_amount ; i++)
	{
		vox_entity_new(&entities[i]);
		entities[i].type = ENTITY_MELON_GOLEM;
		int		w = cbrt(entity_amount);
		int		x = i % w;
		int		y = (i / w) % w;
		int		z = i / (w * w);
		v3_new(entities[i].pos,
			player.camera.pos[0] + (x * 4),
			player.camera.pos[1] + (y * 4), 
			player.camera.pos[2] + (z * 4)
			);
		scale_matrix(entities[i].scale_m4, entities[i].scale);
		rotation_matrix(entities[i].rot_m4, entities[i].rot);
		translation_matrix(entities[i].trans_m4, entities[i].pos);
	}


	GLuint	crosshair_shader;
	new_crosshair_shader(&crosshair_shader);

	GLuint	mandelbrot_shader;
	new_shader(&mandelbrot_shader, SHADER_PATH"mandelbrot.vs", SHADER_PATH"mandelbrot.fs");
	t_fractal2d	fractal;
	new_fractal2d(&fractal);

	t_skybox	skybox;
	new_skybox(&skybox, g_mc_skybox);

//////////////////////////////
	// Instance testing
//////////////////////////////
	GLuint	cube_shader_v2;
	new_shader(&cube_shader_v2, SHADER_PATH"block_mesh.vs", SHADER_PATH"block_mesh.fs");

	t_thread_manager	tm;
	thread_manager_new(&tm, 64);

	int	regen_chunks = 1;

	t_player_info	player_info;

	player_info.reach = 5;
	player_info.equipped_hotbar = 1;
	player_info.hotbar_item_ids[0] = BLOCK_DIRT;
	player_info.hotbar_item_ids[1] = BLOCK_STONE;
	player_info.hotbar_item_ids[2] = BLOCK_ALPHA_TORCH;
	player_info.hotbar_item_ids[3] = BLOCK_SAND;
	player_info.hotbar_item_ids[4] = FLORA_GRASS;
	player_info.hotbar_item_ids[5] = BLOCK_ALPHA_OAK_LEAF;
	player_info.hotbar_item_ids[6] = BLOCK_ALPHA_CACTUS;
	player_info.hotbar_item_ids[7] = BLOCK_TNT;
	player_info.hotbar_item_ids[8] = FLUID_WATER;
	player_info.equipped_block = player_info.hotbar_item_ids[player_info.equipped_hotbar];

	t_world	chunk_info;

	chunk_info.seed = 896868766;
//	chunk_info.seed = 596547633;

	chunk_info.block_collision_enabled = 0;
	chunk_info.player_collision_enabled = 0;
	chunk_info.fancy_graphics = 0;
	chunk_info.generate_structures = 1;
	chunk_info.light_calculation = 0;
	chunk_info.toggle_ui = 0;
	chunk_info.toggle_event = 0;

	chunk_info.sky_light_lvl = 15;

	// Creation of hashtable
	/*
	chunk_info.hash_table_size = (int)(chunk_info.chunks_loaded * 3);
	chunk_info.hash_table = malloc(sizeof(t_hash_item) * chunk_info.hash_table_size);
	hash_table_clear(chunk_info.hash_table, chunk_info.hash_table_size);
	*/

	// Creation of rendering lists;
	chunk_info.meshes_render_indices = malloc(sizeof(int) * CHUNKS_LOADED);
	chunk_info.meshes_render_amount = 0;

	glGenTextures(1, &chunk_info.texture);
	new_texture(&chunk_info.texture, TEXTURE_PATH"version_3_texture_alpha.bmp");

	chunk_info.chunks = malloc(sizeof(t_chunk) * CHUNKS_LOADED);
	chunk_info.chunk_columns = malloc(sizeof(t_chunk_col) * CHUNK_COLUMNS);

	int		nth_chunk = 0;
	int		nth_col = -1;
	int		nth_col_chunk = 0;
	int		player_chunk[V3_SIZE];

	get_chunk_pos_from_world_pos(player_chunk, player.camera.pos);

	LG_INFO("Inits done, lets create some chunks (%d wanted)\n", CHUNKS_LOADED);
	for (; nth_chunk < CHUNKS_LOADED; ++nth_chunk)
	{
		new_chunk(&chunk_info.chunks[nth_chunk], &chunk_info, nth_chunk);
		init_chunk_mesh(&chunk_info.chunks[nth_chunk].meshes, cube_shader_v2, MESH_TYPE_AMOUNT);
		chunk_info.chunks[nth_chunk].meshes.texture = chunk_info.texture;
		if (nth_chunk % CHUNKS_PER_COLUMN == 0)
		{
			nth_col_chunk = 0;
			++nth_col;
			chunk_info.chunk_columns[nth_col].chunks = malloc(sizeof(t_chunk *) * CHUNKS_PER_COLUMN);
			chunk_info.chunk_columns[nth_col].lights = malloc(sizeof(t_light) * CHUNK_COLUMN_LIGHT_AMOUNT);
			chunk_info.chunk_columns[nth_col].height_map.map = NULL;

			chunk_info.chunk_columns[nth_col].coordinate[0] = chunk_info.chunks[nth_chunk].coordinate[0];
			chunk_info.chunk_columns[nth_col].coordinate[1] = chunk_info.chunks[nth_chunk].coordinate[2];
			chunk_info.chunk_columns[nth_col].world_coordinate[0] = chunk_info.chunks[nth_chunk].world_coordinate[0];
			chunk_info.chunk_columns[nth_col].world_coordinate[1] = chunk_info.chunks[nth_chunk].world_coordinate[2];
			chunk_info.chunk_columns[nth_col].update_structures = 0;
		}
		chunk_info.chunk_columns[nth_col].chunks[nth_col_chunk] = &chunk_info.chunks[nth_chunk];
		++nth_col_chunk;
	}
	ft_printf("Total Chunks created : %d\n", nth_chunk);
//////////////////////////////
	// END Instance testing
//////////////////////////////

////////////////////////////////////////
	// UI TESTING
////////////////////////////////////////
/*
	t_ui			gui;
	t_ui_manager	ui;

	ui_manager_init(&ui);
	ui_init(&gui);
	gui.manager = &ui;
	gui.hotbar_item_id = player_info.hotbar_item_ids;
	gui.selected_hotbar = player_info.equipped_hotbar;
	LG_INFO("UI init done.");
	*/
////////////////////////////////////////
	// END UI TESTING
///////////////////////////////////////

	int error = glGetError();
	if (error)
		LG_ERROR("gl errors before while : %d", error);

	glfwSwapInterval(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(1, 0, 1, 1);

	char	fps_str[10];
	while (!glfwWindowShouldClose(sp.win))
	{
		error = glGetError();
		if (error)
			LG_ERROR("errors in start of while : %d", error);

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

		// CHUNK STUFF
		chunk_info.sky_light_lvl_prev = chunk_info.sky_light_lvl;

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

		if (keys[GLFW_KEY_P].state == BUTTON_PRESS)
			player_print(&player);

		if (keys[GLFW_KEY_R].state == BUTTON_PRESS)
			v3_new(player.camera.pos, 0, 100, 0);
		if (keys[GLFW_KEY_LEFT].state == BUTTON_PRESS)
			v3_add(player.camera.pos, player.camera.pos, (float []){-10, 0, 0});
		if (keys[GLFW_KEY_RIGHT].state == BUTTON_PRESS)
			v3_add(player.camera.pos, player.camera.pos, (float []){10, 0, 0});
		if (keys[GLFW_KEY_UP].state == BUTTON_PRESS)
			v3_add(player.camera.pos, player.camera.pos, (float []){0, -10, 0});
		if (keys[GLFW_KEY_DOWN].state == BUTTON_PRESS)
			v3_add(player.camera.pos, player.camera.pos, (float []){0, 10, 0});

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
			for (int j = 0; j < CHUNKS_LOADED; j++)
			{
				if (chunk_info.chunks[j].meshes.vertices_allocated > most_vertices)
					most_vertices = chunk_info.chunks[j].meshes.vertices_allocated;
				if (chunk_info.chunks[j].meshes.texture_ids_allocated > most_textures)
					most_textures = chunk_info.chunks[j].meshes.texture_ids_allocated;
				for (int m = 0; m < chunk_info.chunks[j].meshes.amount; m++)
					if (chunk_info.chunks[j].meshes.indices_allocated[m] > most_indices)
						most_indices = chunk_info.chunks[j].meshes.indices_allocated[m];
			}
			LG_INFO("Most vertices : %d", most_vertices);
			LG_INFO("Most texture ids : %d", most_textures);
			LG_INFO("Most indices : %d", most_indices);
			// TESTING ///
		}

		// Force update all chunks
		if (keys[GLFW_KEY_T].state == BUTTON_PRESS)
		{
			LG_INFO("Force updating all chunks. (Chunks Loaded : %d)", CHUNKS_LOADED);
			for (int i = 0; i < CHUNKS_LOADED; i++)
				chunk_info.chunks[i].needs_to_update = 1;
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

		// Toggle generation of structures
		if (keys[GLFW_KEY_Y].state == BUTTON_PRESS)
		{
			chunk_info.generate_structures = chunk_info.generate_structures != 1;
			if (chunk_info.generate_structures)
				LG_INFO("Generate Structures => ON.");
			else
				LG_INFO("Generate Structures => OFF.");
		}

		// Toggle light calculation;
		if (keys[GLFW_KEY_L].state == BUTTON_PRESS)
		{
			chunk_info.light_calculation = chunk_info.light_calculation != 1;
			if (chunk_info.light_calculation)
				LG_INFO("Light Calculation => ON.");
			else
				LG_INFO("Light Calculation => OFF.");
		}

		// Toggle UI;
		if (keys[GLFW_KEY_U].state == BUTTON_PRESS)
		{
			chunk_info.toggle_ui = chunk_info.toggle_ui != 1;
			if (chunk_info.toggle_ui)
				LG_INFO("UI => ON.");
			else
				LG_INFO("UI => OFF.");
		}

		// Toggle event;
		if (keys[GLFW_KEY_I].state == BUTTON_PRESS)
		{
			chunk_info.toggle_event = chunk_info.toggle_event != 1;
			if (chunk_info.toggle_event)
				LG_INFO("Event => ON.");
			else
				LG_INFO("Event => OFF.");
		}

		// Toggle event;
		if (keys[GLFW_KEY_Q].state == BUTTON_PRESS)
		{
			attach_entity = attach_entity != 1;
			if (attach_entity)
				LG_INFO("attach_entity => ON.");
			else
				LG_INFO("attach_entity => OFF.");
		}

// Select equipped block;
		for (int i = GLFW_KEY_1; i <= GLFW_KEY_9; i++)
		{
			if (keys[i].state == BUTTON_PRESS)
			{
				player_info.equipped_hotbar = i - GLFW_KEY_1;
				player_info.equipped_block = player_info.hotbar_item_ids[player_info.equipped_hotbar];
				//gui.selected_hotbar = player_info.equipped_hotbar;
				if (is_type_solid(player_info.equipped_block) ||
					is_type_fluid(player_info.equipped_block) ||
					is_type_solid_alpha(player_info.equipped_block))
					ft_printf("Block[%d] : '%s' equipped.\n", player_info.equipped_block, g_block_data[player_info.equipped_block].name);
				else if (is_type_item(player_info.equipped_block))
					ft_printf("Item : '%s' equipped.\n", g_item_data[player_info.equipped_block - ITEM_FIRST - 1].name);
			}
		}

		// Change sky light level, Debug only;
		if (keys[GLFW_KEY_KP_SUBTRACT].state == BUTTON_PRESS)
		{
			chunk_info.sky_light_lvl = ft_clamp(chunk_info.sky_light_lvl - 1, 0, 15);
			LG_INFO("Sky Light Level : %d", chunk_info.sky_light_lvl);
		}
		if (keys[GLFW_KEY_KP_ADD].state == BUTTON_PRESS)
		{
			chunk_info.sky_light_lvl = ft_clamp(chunk_info.sky_light_lvl + 1, 0, 15);
			LG_INFO("Sky Light Level : %d", chunk_info.sky_light_lvl);
		}

		update_fps(&fps);
		player_events(&player, keys, sp.win);

		if (player.gravity == 0)
			player_movement_creative(&player, sp.win, fps);
		if (player.gravity != 0)
			player_movement_survival(&player, sp.win, fps);
		if (player.enabled_mouse)
			player_looking(&player, sp.win, fps);

/////////////////
		// Chunk things
/////////////////
		chunk_info.game_tick = 0;
		if ((int)(fps.curr_time * 100) % 33 == 0)
			chunk_info.game_tick = 1;

		get_chunk_pos_from_world_pos(player_chunk, player.camera.pos);

		int	tobegen = 0;
		if (regen_chunks)
		{
			int	max_get = 1;
			int	col_indices[max_get];
			int	col_coords[max_get][2];
			int	start_coord[2];
			tobegen = get_chunk_column_to_regen(chunk_info.chunk_columns, player_chunk, col_indices, col_coords, max_get);
			for (int i = 0; i < tobegen; i++)
			{
				regenerate_chunk_column(&chunk_info.chunk_columns[col_indices[i]], col_coords[i], chunk_info.seed);
			}
		}

		thread_manager_check_threadiness(&tm);

		t_chunk		*neighbors[DIR_AMOUNT];
		t_chunk		**col_chunks;
		t_chunk_col	*column;
		int			neighbors_found;

		for (int col = 0; col < CHUNK_COLUMNS; col++)
		{
			column = &chunk_info.chunk_columns[col];
			col_chunks = column->chunks;

			column->chunk_needs_update = 0;
		
			if (chunk_info.generate_structures && column->update_structures)
			{
				tree_gen(&chunk_info, column);
				column->update_structures = 0;
			}

			// Check if a chunk in the column needs an update;
			for (int ent = 0; ent < CHUNKS_PER_COLUMN; ++ent)
			{
				if (col_chunks[ent]->needs_to_update)
				{
					update_chunk_block_palette(col_chunks[ent]);
					column->chunk_needs_update = 1;
				}
			}

			// Light calculation;
			if (chunk_info.light_calculation &&
				(column->chunk_needs_update ||
				chunk_info.sky_light_lvl != chunk_info.sky_light_lvl_prev))
				update_chunk_column_light(column);
			// Other Chunk updates;
			for (int ent = 0; ent < CHUNKS_PER_COLUMN; ++ent)
			{
				neighbors_found = 0;

				// We only need to update the chunks if a chunk has been regenerated
				if ((col_chunks[ent]->needs_to_update ||
					(chunk_info.generate_structures && column->update_structures)))
				{
					// Get all neighbors for this chunk;
					for (int dir = DIR_NORTH, i = 0; dir < DIR_AMOUNT; ++dir, ++i)
					{
						neighbors[i] = get_adjacent_chunk(&chunk_info, col_chunks[ent], (float *)g_card_dir[dir]);
						++neighbors_found;
					}
				}

				if (col_chunks[ent]->needs_to_update)
				{
					update_chunk(col_chunks[ent]);
					update_chunk_event_blocks(col_chunks[ent]);
					// Set needs to update to all 6 neighbors of the chunk;
					for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; ++dir, ++i)
						if (neighbors[i])
							neighbors[i]->secondary_update = 1;
				}

				col_chunks[ent]->needs_to_update = 0;

				if (chunk_info.toggle_event && chunk_info.game_tick)
					event_chunk(col_chunks[ent]);
			}
		}

		// Secondary updater;
		// We dont want to immediately update the chunks that other chunks want
		//	updated, because they might update themselves;
		// So if theres still chunks that needs updating after we've gone through
		// 	the chunks once, we update them here;
		if (tobegen == 0)
		{
			int	column_redo_light;
			for (int col = 0; col < CHUNK_COLUMNS; col++)
			{
				column = &chunk_info.chunk_columns[col];
				col_chunks = column->chunks;

				for (int ent = 0; ent < CHUNKS_PER_COLUMN; ++ent)
				{
					if (col_chunks[ent]->secondary_update)
					{
						col_chunks[ent]->secondary_update = 0;
						col_chunks[ent]->was_updated = 1;
						update_chunk_border_visible_blocks(col_chunks[ent]);
					}
				}

				// Send chunk mesh to GPU if it was updated;
				for (int ent = 0; ent < CHUNKS_PER_COLUMN; ++ent)
				{
					if (col_chunks[ent]->was_updated) // Only send mesh info to gpu, if the chunk actually was changed;
					{
						col_chunks[ent]->was_updated = 0;
						if (col_chunks[ent]->has_visible_blocks)
							update_chunk_mesh(&col_chunks[ent]->meshes);
					}
				}
			}
			// REMOVE DEBUG
			//exit(0);
		}

		// head
//		player_terrain_collision(player.velocity, (float []){player.camera.pos[0], player.camera.pos[1] + 0.25f, player.camera.pos[2]}, player.velocity, &chunk_info);
		// feet
//		player_terrain_collision(player.velocity, (float []){player.camera.pos[0], player.camera.pos[1] - 1.0f, player.camera.pos[2]}, player.velocity, &chunk_info);

		player_apply_velocity(&player);
		update_camera(&player.camera);

	// Used for block collision
		float	intersect_point[16][3]; // Make sure the amount of collisions that happen are less than the amount of stack memory in these 2 arrays;
		int		intersect_chunk_index[16]; // correspond with the index in 'intersect_point';
		int		collision_result = 0; // will be the amount of collisions that has happened;

		nth_chunk = 0;

		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST); // note to self, if we disable depth_test. we dont write to it either, which means the next thing checking int the depth tester wont see what you have not written there.... aka the skybox in this case

		// Reset the rendering amount to 0;
		chunk_info.meshes_render_amount = 0;

		for (; nth_chunk < CHUNKS_LOADED; ++nth_chunk)
		{
			if (!chunk_info.chunks[nth_chunk].has_visible_blocks)
				continue ;

			// Decide if we want to render the chunk or not;
			// Dont render chunk if the chunk is further away than the farplane of the camear;
			// Dont render if the chunk is outside the view fustrum;
			// Dont render if hasnt been sent to gpu yet;
			if (chunk_info.chunks[nth_chunk].was_updated == 0 &&
				v3_dist(player.camera.pos, chunk_info.chunks[nth_chunk].world_coordinate) <
				player.camera.far_plane + CHUNK_SIZE_X &&
				aabb_in_frustum(&chunk_info.chunks[nth_chunk].aabb, &player.camera.frustum))
			{
				chunk_info.meshes_render_indices[chunk_info.meshes_render_amount] = nth_chunk;
				++chunk_info.meshes_render_amount;
			}

			// Collision Detection
			if (chunk_info.block_collision_enabled)
			{
				if (point_aabb_center_distance(player.camera.pos, &chunk_info.chunks[nth_chunk].aabb) <= (CHUNK_WIDTH * CHUNK_WIDTH))
				{
					show_chunk_borders(&chunk_info.chunks[nth_chunk], &player.camera, (float []){1, 0, 0});
					// Place Blocking and Removing;
					// Go through all chunks and check for collision on blocks,
					// store intersections and indices of the chunk the intersection
					// is in;

					// Collision on solid mesh;
					if (chunk_info.chunks[nth_chunk].blocks_solid_amount > 0)
					{
						int collisions_on_chunk = chunk_mesh_collision_v2(player.camera.pos, player.camera.front, &chunk_info.chunks[nth_chunk].meshes, BLOCK_MESH, chunk_info.chunks[nth_chunk].world_coordinate, player_info.reach, intersect_point + collision_result);
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
		int		block_local[3];
		float	block_pos[3]; // get_block_from_chunk(); stores the pos of the block we are hovering over;
		float	closest_point[3];
		int		closest_index = -1; // the index of the chunk that has the closest collision;
		int		face = -1; // -1 is no face; which face has been hovered on the block;
		if (collision_result > 0)
		{
			float	distancione = -1;
			float	closest_dist = INFINITY;
			for (int colli = 0; colli < collision_result; ++colli)
			{
				distancione = v3_dist_sqrd(player.camera.pos, intersect_point[colli]);
				if (distancione < closest_dist)
				{
					closest_dist = distancione;
					v3_assign(closest_point, intersect_point[colli]);
					closest_index = intersect_chunk_index[colli];
				}
			}

			t_block *hovered_block = NULL;
			hovered_block = get_block_from_chunk(&chunk_info.chunks[closest_index], closest_point, block_pos, &face);
			if (hovered_block)
				render_block_outline(block_pos, (float []){0, 0, 0}, player.camera.view, player.camera.projection);
			if (hovered_block &&
				(mouse[GLFW_MOUSE_BUTTON_LEFT].state == BUTTON_PRESS ||
				mouse[GLFW_MOUSE_BUTTON_RIGHT].state == BUTTON_PRESS ||
				mouse[GLFW_MOUSE_BUTTON_MIDDLE].state == BUTTON_PRESS))
			{
				get_block_local_pos_from_world_pos(block_local, block_pos);
				if (mouse[GLFW_MOUSE_BUTTON_LEFT].state == BUTTON_PRESS)
					set_block_at_world_pos(&chunk_info, block_pos, GAS_AIR);
				else if (mouse[GLFW_MOUSE_BUTTON_RIGHT].state == BUTTON_PRESS)
				{
					float	block_world[3];
					v3_add(block_world, block_pos, (float *)g_card_dir[face]);
					// Check if block or item equipped;
					LG_INFO("Place Item at %f %f %f", block_world[0], block_world[1], block_world[2]);
					if (!is_type_item(player_info.equipped_block))
						set_block_at_world_pos(&chunk_info, block_world, player_info.equipped_block);
					else if (is_type_item(player_info.equipped_block))
					{
						ft_printf("Using : %s\n", g_item_data[player_info.equipped_block - ITEM_FIRST - 1].name);
						if (player_info.equipped_block == ITEM_TREE_PLACER)
							tree_placer(&chunk_info, block_world);
					}
					else
						LG_WARN("We dont allow the placing of that type of block.");
				}
				else if (mouse[GLFW_MOUSE_BUTTON_MIDDLE].state == BUTTON_PRESS)
				{
					block_print(hovered_block);
					v3i_string("Local Coordinates : ", block_local);
					v3_string("World Coordinates : ", block_pos);
					t_chunk *temp_chunk = get_chunk_from_world_pos(&chunk_info, block_pos);
					v3i_string("Chunk Coordinates : ", temp_chunk->coordinate);
					v3_string("Chunk World Coordinates : ", temp_chunk->world_coordinate);

				}
			}
		}
		/* END OF COLLISION */
error = glGetError();
if (error)
	LG_ERROR("before skybox things. (%d)", error);

		glEnable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		render_skybox(&skybox, &player.camera);

error = glGetError();
if (error)
	LG_ERROR("before solid things. (%d)", error);
		// Render solid meshes;
		glEnable(GL_DEPTH_TEST);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		for (int r = 0; r < chunk_info.meshes_render_amount; r++)
		{
			int render_index = chunk_info.meshes_render_indices[r];
			if (chunk_info.chunks[render_index].blocks_solid_amount > 0)
				render_chunk_mesh(&chunk_info.chunks[render_index].meshes, BLOCK_MESH, chunk_info.chunks[render_index].world_coordinate, &player.camera);
		}

error = glGetError();
if (error)
	LG_ERROR("before fluid things. (%d)", error);

		// Render fluid meshes;
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		for (int r = 0; r < chunk_info.meshes_render_amount; r++)
		{
			int render_index = chunk_info.meshes_render_indices[r];
			if (chunk_info.chunks[render_index].blocks_fluid_amount > 0)
				render_chunk_mesh(&chunk_info.chunks[render_index].meshes, FLUID_MESH, chunk_info.chunks[render_index].world_coordinate, &player.camera);
		}

/////////////////
		// END Chunk things
/////////////////

////////////////////////
// Model Rendering
////////////////////////
	float	model_mat[16];

	if (attach_entity)
	{
		v3_new(melon_golem_pos, player.camera.pos[0], player.camera.pos[1] - 2, player.camera.pos[2] - 2);
		model_update(&model_melon_golem);
	}
	new_model_matrix(model_mat, 1.0f, (float []){90, 0, 0}, melon_golem_pos);
	model_render(&model_melon_golem, model_shader, model_mat, player.camera.view, player.camera.projection);

////////////////////////
// Model Rendering Instance
////////////////////////
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	// NOTE : we need the amount of entities of a single type (entity_palette just like block_palette);
	// Create model matrix array from all the entities of the same type;

	float	v3_total = 0;
	float	rot_total = 0;
	float	mod_total = 0;
	for (int i = 0; i < entity_amount; i++)
	{
		// REMOVE : apply random rotation just to show that they have their own model mat;
		ft_timer_start();
		v3_new(entities[i].rot,
			entities[i].rot[0] + rand() / 100 * fps.delta_time,
			entities[i].rot[1] + rand() / 100 * fps.delta_time,
			entities[i].rot[2] + rand() / 100 * fps.delta_time
			);
		v3_total += ft_timer_end();

		ft_timer_start();
		rotation_matrix(entities[i].rot_m4, entities[i].rot);
		rot_total += ft_timer_end();

		ft_timer_start();
		model_matrix(model_matrices + i * 16, entities[i].scale_m4, entities[i].rot_m4, entities[i].trans_m4);
		mod_total += ft_timer_end();
	//	new_model_matrix(model_matrices + i * 16, entities[i].scale, entities[i].rot, entities[i].pos);
	}
	ft_printf("model mat creation : v3 : %f, rot : %f, mod : %f\n", v3_total, rot_total, mod_total);
	model_instance_render(&instance_model, model_instance_shader, model_matrices, entity_amount, player.camera.view, player.camera.projection);

	error = glGetError();
if (error)
	LG_ERROR("Afetr chunk things. (%d)", error);
		glDisable(GL_BLEND);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		render_crosshair();

		if (chunk_info.toggle_ui)
		{
			/*
			ui_manager_start(&ui);
			{
				char		buffer[256];
				t_bitmap	bmp;

				// Player Position
				strcpy(buffer, "Position : ");
				ft_b_ftoa(player.camera.pos[0], 2, buffer + strlen(buffer));
				strcpy(buffer + strlen(buffer), " / ");
				ft_b_ftoa(player.camera.pos[1], 2, buffer + strlen(buffer));
				strcpy(buffer + strlen(buffer), " / ");
				ft_b_ftoa(player.camera.pos[2], 2, buffer + strlen(buffer));
				fm_render_text(&bmp, &ui.font_manager, 0, buffer, 0xff0000ff, 0xffffffff);
				ui_draw_bitmap(&ui, (float []){120, 10, bmp.width, bmp.height}, &bmp);
				bitmap_free(&bmp);

				// Player Rotation
				strcpy(buffer, "Rotation : ");
				ft_b_ftoa(player.camera.front[0], 2, buffer + strlen(buffer));
				strcpy(buffer + strlen(buffer), " / ");
				ft_b_ftoa(player.camera.front[1], 2, buffer + strlen(buffer));
				strcpy(buffer + strlen(buffer), " / ");
				ft_b_ftoa(player.camera.front[2], 2, buffer + strlen(buffer));
				fm_render_text(&bmp, &ui.font_manager, 0, buffer, 0xff0000ff, 0xffffffff);
				ui_draw_bitmap(&ui, (float []){120, 40, bmp.width, bmp.height}, &bmp);
				bitmap_free(&bmp);

				bitmap_new(&bmp, 100, 100);
				bitmap_fill(&bmp, 0xff00ffff);
				ui_draw_bitmap(&ui, (float []){10, 340, bmp.width, bmp.height}, &bmp);
				bitmap_free(&bmp);

				ui_draw_bitmap(&ui, (float []){10, 450, gui.block_texture_bmp.width, gui.block_texture_bmp.height}, &gui.block_texture_bmp);

				ui_draw_rect(&ui, (float []){10, 10, 100, 100}, (Uint8 []){255, 0, 0, 255});
				ui_draw_filled_rect(&ui, (float []){10, 120, 100, 100}, (Uint8 []){0, 255, 0, 255});
				ui_draw_filled_rect(&ui, (float []){10, 230, 100, 100}, (Uint8 []){255, 255, 255, 125});
				ui_draw_filled_rect_multi_color(&ui, (float []){120, 120, 100, 100}, (Uint32 []){0xffffff00, 0xff0000ff, 0xff00ff00, 0xffff0000});


				ui_draw(&gui);
			}
			ui_manager_render(&ui, sp.win_w, sp.win_h);
			ui_manager_end(&ui);
			*/
		}

		glfwSwapBuffers(sp.win);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		error = glGetError();
		if (error)
			LG_ERROR("errors in while : %d", error);
	}

	uninit(&sp);
	return (0);
}
