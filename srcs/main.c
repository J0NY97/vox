#include "vox.h"
#include "entity.h"
#include "world.h"
#include "chunk.h"
#include "shader.h"
#include "bobj.h"
#include "enum.h"
#include "ui_manager.h"
#include "ui.h"

void	errorCallback(int err_nbr, const char *err_str)
{
	LG_ERROR("(%d) [%s]", err_nbr, err_str);
}

void settings_init(t_settings *settings)
{
	settings->chunk_generation = 1;
	settings->cave_generation = 0;
	settings->toggle_ui = 0;
	settings->entity_hitbox_enabled = 0;
	settings->attach_entity = 0;
	settings->attach_to_entity = 0;
}

void	init(t_vox *vox)
{
	lg_setFdLevel(LEVEL_INFO);

	if (!glfwInit())
		LG_ERROR("Couldn\'t init glfw.");
	glfwSetErrorCallback(errorCallback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	vox->win_w = 1280;
	vox->win_h = 720;
	vox->win = glfwCreateWindow(vox->win_w, vox->win_h, "vox", NULL, NULL);

	glfwMakeContextCurrent(vox->win);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		LG_ERROR("Couldn\'t load glad.");
	glViewport(0, 0, vox->win_w, vox->win_h);
	vox->polygon_mode = GL_FILL;
	vox->renderMode = 0;

	settings_init(&vox->settings);

	// TODO : Only have 1 thread manager, add all the threading to the one,
	//		in the main loop have 'check_threadiness()';
	thread_manager_new(&vox->tm_gen, 64);
	thread_manager_new(&vox->tm_update, 64);

	LG_INFO("Init Done");
}

void	uninit(t_vox *vox)
{
	(void)vox; // TODO : Free everything;
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

void debug_create_entities(t_entity_manager *manager, float *pos_v3, int amount_to_create)
{
	/// Create max_entities amount of entitites infront of the player in a cube;
	for (int i = 0; i < amount_to_create && i < manager->max_entities; i++)
	{
		t_entity *entity = entity_manager_new_entity(manager);
		// set all entities to draw_aabb;
//		entity->draw_aabb = 1;
		/*
		if (i % 2 == 0)
			entity->type = ENTITY_MELON_GOLEM;
		else
			entity->type = ENTITY_CHICKEN;
			*/
		int		w = cbrt(manager->max_entities);
		int		x = i % w;
		int		y = (i / w) % w;
		int		z = i / (w * w);
		v3_new(entity->pos, pos_v3[0] + (x * 4), pos_v3[1] + (y * 4), pos_v3[2] + (z * 4) );
	}
	LG_INFO("Created %d entities for debugging purposes", manager->max_entities);
}

void gui_update(t_vox *vox, t_ui *ui, t_ui_manager *ui_manager)
{
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	render_crosshair();

	///////////// UI ////////////
	////// EVENTS ////////
	if (vox->keys[GLFW_KEY_H].state == BUTTON_PRESS)
		ui->inventory_open = !ui->inventory_open;
	////// DRAW ////////
	if (vox->settings.toggle_ui)
	{
		ui_update(ui, vox->win);
		ui_manager_start(ui_manager);
		char		buffer[256];
		t_bitmap	bmp;

		/*
		// Player Position
		strcpy(buffer, "Position : ");
		ft_b_ftoa(player.camera->pos[0], 2, buffer + strlen(buffer));
		strcpy(buffer + strlen(buffer), " / ");
		ft_b_ftoa(player.camera->pos[1], 2, buffer + strlen(buffer));
		strcpy(buffer + strlen(buffer), " / ");
		ft_b_ftoa(player.camera->pos[2], 2, buffer + strlen(buffer));
		fm_render_text(&bmp, &ui.font_manager, 0, buffer, 0xff0000ff, 0xffffffff);
		ui_draw_bitmap(&ui, (float []){120, 10, bmp.width, bmp.height}, &bmp);
		bitmap_free(&bmp);

		// Player Rotation
		strcpy(buffer, "Rotation : ");
		ft_b_ftoa(player.camera->front[0], 2, buffer + strlen(buffer));
		strcpy(buffer + strlen(buffer), " / ");
		ft_b_ftoa(player.camera->front[1], 2, buffer + strlen(buffer));
		strcpy(buffer + strlen(buffer), " / ");
		ft_b_ftoa(player.camera->front[2], 2, buffer + strlen(buffer));
		fm_render_text(&bmp, &ui.font_manager, 0, buffer, 0xff0000ff, 0xffffffff);
		ui_draw_bitmap(&ui, (float []){120, 40, bmp.width, bmp.height}, &bmp);
		bitmap_free(&bmp);
		*/

		bitmap_new(&bmp, 100, 100);
		bitmap_fill(&bmp, 0xff00ffff);
		ui_draw_bitmap(ui_manager, (float []){10, 340, bmp.width, bmp.height}, &bmp);
		bitmap_free(&bmp);

		ui_draw_bitmap(ui_manager, (float []){10, 450, ui->block_texture_bmp.width, ui->block_texture_bmp.height}, &ui->block_texture_bmp);

		ui_draw_rect(ui_manager, (float []){10, 10, 100, 100}, (Uint8 []){255, 0, 0, 255});
		ui_draw_filled_rect(ui_manager, (float []){10, 120, 100, 100}, (Uint8 []){0, 255, 0, 255});
		ui_draw_filled_rect(ui_manager, (float []){10, 230, 100, 100}, (Uint8 []){255, 255, 255, 125});
		ui_draw_filled_rect_multi_color(ui_manager, (float []){120, 120, 100, 100}, (Uint32 []){0xffffff00, 0xff0000ff, 0xff00ff00, 0xffff0000});


		ui_draw(ui);
		ui_manager_render(ui_manager, vox->win_w, vox->win_h);
		ui_manager_end(ui_manager);
	}

	
}

void input_handle(t_vox *vox, t_world *world, t_player *player, t_ui *gui, t_fps *fps)
{
	update_all_keys(vox->keys, vox->mouse, vox->win);
	glfwPollEvents();
	if (vox->keys[GLFW_KEY_ESCAPE].state == BUTTON_PRESS)
		glfwSetWindowShouldClose(vox->win, GLFW_TRUE);
	if (vox->keys[GLFW_KEY_X].state == BUTTON_PRESS)
	{
		vox->renderMode = (vox->renderMode + 1) % 3;
		if (vox->renderMode == 0)
			vox->polygon_mode = GL_FILL;
		else if (vox->renderMode == 1)
			vox->polygon_mode = GL_LINE;
		else if (vox->renderMode == 2)
			vox->polygon_mode = GL_POINT;
		glPolygonMode(GL_FRONT_AND_BACK, vox->polygon_mode);
	}

	if (vox->keys[GLFW_KEY_P].state == BUTTON_PRESS)
		player_print(player);

	if (vox->keys[GLFW_KEY_R].state == BUTTON_PRESS)
		v3_assign(player->camera->pos, world->spawn_point);
	if (vox->keys[GLFW_KEY_LEFT].state == BUTTON_PRESS)
		v3_add(player->camera->pos, player->camera->pos, (float []){-10, 0, 0});
	if (vox->keys[GLFW_KEY_RIGHT].state == BUTTON_PRESS)
		v3_add(player->camera->pos, player->camera->pos, (float []){10, 0, 0});
	if (vox->keys[GLFW_KEY_UP].state == BUTTON_PRESS)
		v3_add(player->camera->pos, player->camera->pos, (float []){0, -10, 0});
	if (vox->keys[GLFW_KEY_DOWN].state == BUTTON_PRESS)
		v3_add(player->camera->pos, player->camera->pos, (float []){0, 10, 0});

	if (vox->keys[GLFW_KEY_C].state == BUTTON_PRESS)
	{
		world->block_collision_enabled = world->block_collision_enabled != 1;
		if (world->block_collision_enabled)
			LG_INFO("Block collision => ON");
		else
			LG_INFO("Block collision => OFF");
	}

	if (vox->keys[GLFW_KEY_V].state == BUTTON_PRESS)
	{
		world->player_collision_enabled = world->player_collision_enabled != 1;
		if (world->player_collision_enabled)
			LG_INFO("Player collision detection => ON");
		else
			LG_INFO("Player collision detection => OFF");
	}

	if (vox->keys[GLFW_KEY_B].state == BUTTON_PRESS)
	{
		if (player->gravity == 0)
		{
			player->gravity = 0.98;
			LG_INFO("Gravity => ON");
		}
		else
		{
			player->gravity = 0;
			LG_INFO("Gravity => OFF");
		}
	}

	// Generate Chunks Toggle
	if (vox->keys[GLFW_KEY_G].state == BUTTON_PRESS)
	{
		vox->settings.chunk_generation = vox->settings.chunk_generation != 1;
		if (vox->settings.chunk_generation)
			LG_INFO("Regeneration of chunks turned ON.");
		else
			LG_INFO("Regeneration of chunks turned OFF.");
	}

	// Generate Caves Toggle
	if (vox->keys[GLFW_KEY_H].state == BUTTON_PRESS)
	{
		vox->settings.cave_generation = vox->settings.cave_generation != 1;
		if (vox->settings.cave_generation)
			LG_INFO("Regeneration of caves turned ON.");
		else
			LG_INFO("Regeneration of caves turned OFF.");
	}

	// Enable hitbox for entities;
	if (vox->keys[GLFW_KEY_O].state == BUTTON_PRESS)
	{
		vox->settings.entity_hitbox_enabled = !vox->settings.entity_hitbox_enabled;
		if (vox->settings.entity_hitbox_enabled)
			LG_INFO("Entity Hitbox Enabled");
		else
			LG_INFO("Entity Hitbox Disabled");
	}

	// Force update all chunks
	if (vox->keys[GLFW_KEY_T].state == BUTTON_PRESS)
	{
		LG_INFO("Force updating all chunks. (Chunks Loaded : %d)", CHUNKS_LOADED);
		for (int i = 0; i < CHUNKS_LOADED; i++)
			world->chunks[i].needs_to_update = 1;
	}

	// Toggle fancy graphics
	if (vox->keys[GLFW_KEY_F].state == BUTTON_PRESS)
	{
		world->fancy_graphics = world->fancy_graphics != 1;
		if (world->fancy_graphics)
			LG_INFO("Fancy graphics => ON.");
		else
			LG_INFO("Fancy graphics => OFF.");
	}

	// Toggle generation of structures
	if (vox->keys[GLFW_KEY_Y].state == BUTTON_PRESS)
	{
		world->generate_structures = world->generate_structures != 1;
		if (world->generate_structures)
			LG_INFO("Generate Structures => ON.");
		else
			LG_INFO("Generate Structures => OFF.");
	}

	// Toggle light calculation;
	if (vox->keys[GLFW_KEY_L].state == BUTTON_PRESS)
	{
		world->light_calculation = world->light_calculation != 1;
		if (world->light_calculation)
			LG_INFO("Light Calculation => ON.");
		else
			LG_INFO("Light Calculation => OFF.");
	}

	// Toggle UI;
	if (vox->keys[GLFW_KEY_U].state == BUTTON_PRESS)
	{
		vox->settings.toggle_ui = vox->settings.toggle_ui != 1;
		if (vox->settings.toggle_ui)
			LG_INFO("UI => ON.");
		else
			LG_INFO("UI => OFF.");
	}

	// Toggle event;
	if (vox->keys[GLFW_KEY_I].state == BUTTON_PRESS)
	{
		world->toggle_event = world->toggle_event != 1;
		// TODO : should probably go through all the chunks and force update
		//	the event blocks; (aka chunk->needs_to_update);
		// There is a key that sets all chunk->needs_to_update to 1, one
		//	could convert that to a function and call it in here;
		if (world->toggle_event)
			LG_INFO("Event => ON.");
		else
			LG_INFO("Event => OFF.");
	}

	// Toggle attach entity;
	if (vox->keys[GLFW_KEY_Q].state == BUTTON_PRESS)
	{
		vox->settings.attach_entity = vox->settings.attach_entity != 1;
		if (vox->settings.attach_entity)
			LG_INFO("attach_entity => ON.");
		else
			LG_INFO("attach_entity => OFF.");
	}

	// Melon controls
	if (world->entity_manager.melon_entity)
	{
		if (vox->keys[GLFW_KEY_KP_4].state == BUTTON_HOLD)
		{
			world->entity_manager.melon_entity->yaw -= 0.1f;
		}
		if (vox->keys[GLFW_KEY_KP_6].state == BUTTON_HOLD)
		{
			world->entity_manager.melon_entity->yaw += 0.1f;
		}
		if (vox->keys[GLFW_KEY_KP_0].state == BUTTON_HOLD)
		{
			float speed = world->entity_manager.melon_entity->speed;
			if (vox->keys[GLFW_KEY_LEFT_SHIFT].state == BUTTON_HOLD)
				speed *= 10;
			v3_multiply_f(world->entity_manager.melon_entity->velocity, world->entity_manager.melon_entity->front, speed * fps->delta_time);
		}
		if (vox->keys[GLFW_KEY_P].state == BUTTON_PRESS)
		{
			ft_printf("Euler : %d %d\n", world->entity_manager.melon_entity->yaw, world->entity_manager.melon_entity->pitch);
			ft_printf("Front : %.2f %.2f %.2f\n", world->entity_manager.melon_entity->front[0], world->entity_manager.melon_entity->front[1], world->entity_manager.melon_entity->front[2]);
		}
		if (vox->keys[GLFW_KEY_KP_5].state == BUTTON_PRESS)
		{
			world->entity_manager.melon_entity->ai = world->entity_manager.melon_entity->ai != 1;
			world->entity_manager.chicken_entity->ai = world->entity_manager.chicken_entity->ai != 1;
			if (world->entity_manager.melon_entity->ai)
				LG_INFO("entity->ai => ON.");
			else
				LG_INFO("entity->ai => OFF.");
		}
		if (vox->keys[GLFW_KEY_KP_7].state == BUTTON_PRESS)
		{
			world->entity_manager.melon_entity->draw_aabb = world->entity_manager.melon_entity->draw_aabb != 1;
			world->entity_manager.melon_entity->draw_dir = world->entity_manager.melon_entity->draw_aabb;
			world->entity_manager.chicken_entity->draw_aabb = world->entity_manager.chicken_entity->draw_aabb != 1;
			world->entity_manager.chicken_entity->draw_dir = world->entity_manager.chicken_entity->draw_aabb;
			if (world->entity_manager.melon_entity->draw_aabb)
				LG_INFO("entity->draw_aabb => ON.");
			else
				LG_INFO("entity->draw_aabb => OFF.");
		}

		if (vox->settings.attach_entity)
			v3_new(world->entity_manager.melon_entity->pos, player->camera->pos[0], player->camera->pos[1] - 2, player->camera->pos[2] - 2);
		else if (vox->settings.attach_to_entity)
		{
			float p_pos[3];
			v3_multiply_f(p_pos, world->entity_manager.melon_entity->front, -2.0f);
			v3_add(p_pos, p_pos, world->entity_manager.melon_entity->pos);
			v3_assign(player->camera->pos, p_pos);
			player->camera->yaw = world->entity_manager.melon_entity->yaw;
			player->camera->pitch = -world->entity_manager.melon_entity->pitch;
		}
	}

	// Select equipped block;
	for (int i = GLFW_KEY_1; i <= GLFW_KEY_9; i++)
	{
		if (vox->keys[i].state == BUTTON_PRESS)
		{
			player->info.equipped_hotbar = i - GLFW_KEY_1;
			player->info.equipped_block = player->info.hotbar_item_ids[player->info.equipped_hotbar];
			gui->selected_hotbar = player->info.equipped_hotbar;
			if (is_type_solid(player->info.equipped_block) ||
				is_type_fluid(player->info.equipped_block) ||
				is_type_solid_alpha(player->info.equipped_block))
				ft_printf("Block[%d] : '%s' equipped.\n", player->info.equipped_block, g_block_data[player->info.equipped_block].name);
			else if (is_type_item(player->info.equipped_block))
				ft_printf("Item : '%s' equipped.\n", g_item_data[player->info.equipped_block - ITEM_FIRST - 1].name);
		}
	}

	// Change sky light level, Debug only;
	if (vox->keys[GLFW_KEY_KP_SUBTRACT].state == BUTTON_PRESS)
	{
		world->sky_light_lvl = ft_clamp(world->sky_light_lvl - 1, 0, 15);
		LG_INFO("Sky Light Level : %d", world->sky_light_lvl);
	}
	if (vox->keys[GLFW_KEY_KP_ADD].state == BUTTON_PRESS)
	{
		world->sky_light_lvl = ft_clamp(world->sky_light_lvl + 1, 0, 15);
		LG_INFO("Sky Light Level : %d", world->sky_light_lvl);
	}

}

void game_update(t_vox *vox, t_fps *fps, t_player *player, t_world *world, t_ui *gui, t_skybox *skybox)
{
	int error = glGetError();
	if (error)
		LG_ERROR("errors in start of game() : %d", error);

	glCullFace(GL_BACK);

	if (fps->count % 60 == 0)
	{
		char fps_str[16];
		ft_b_itoa(fps->fps, fps_str);
		glfwSetWindowTitle(vox->win, fps_str);
	}

	// CHUNK STUFF
	world->sky_light_lvl_prev = world->sky_light_lvl;

	// Input
	input_handle(vox, world, player, gui, fps);

	// Player Input?
	player_event(player, vox->keys, vox->win);

	if (player->gravity == 0)
		player_movement_creative(player, vox->win, fps);
	if (player->gravity != 0)
		player_movement_survival(player, vox->win, fps);
	if (player->enabled_mouse)
		player_looking(player, vox->win);

	// World game tick
	world->game_tick = 0;
	if ((int)(fps->curr_time * 100) % 33 == 0)
		world->game_tick = 1;

	//
	get_chunk_pos_from_world_pos(world->player_chunk, player->camera->pos);

	//// CHUNK REGENERATION ////
	chunk_generation(vox, world);

	//// CHUNK UPDATES ////
	update_chunk_columns(vox, world);

	// Secondary updater;
	// We dont want to immediately update the chunks that other chunks want
	//	updated, because they might update themselves;
	// So if theres still chunks that needs updating after we've gone through
	// 	the chunks once, we update them here;
	for (int col = 0; col < CHUNK_COLUMN_AMOUNT; col++)
	{
		t_chunk_column *column = &world->chunk_columns[col];

		if (column->regeneration_threaded || column->update_threaded)
			continue ;

		t_chunk **col_chunks = column->chunks;

		// Update the chunk border block face visibility;
		for (int ent = 0; ent < CHUNKS_PER_COLUMN; ++ent)
		{
			if (col_chunks[ent]->secondary_update)
			{
				col_chunks[ent]->secondary_update = 0;
				col_chunks[ent]->was_updated = 1;
				update_chunk_border_visible_blocks(col_chunks[ent]);
			}
		}
		/*
		*/

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

	// head
	//		player_terrain_collision(player.velocity, (float []){player.camera->pos[0], player.camera->pos[1] + 0.25f, player.camera->pos[2]}, player.velocity, &world);
	// feet
	//		player_terrain_collision(player.velocity, (float []){player.camera->pos[0], player.camera->pos[1] - 1.0f, player.camera->pos[2]}, player.velocity, &world);

	player_apply_velocity(player);
	update_camera(&world->camera);

	// Used for block collision
	float	intersect_point[16][3]; // Make sure the amount of collisions that happen are less than the amount of stack memory in these 2 arrays;
	int		intersect_chunk_index[16]; // correspond with the index in 'intersect_point';
	int		collision_result = 0; // will be the amount of collisions that has happened;

	// Chunk Collision Detection (MINING)
	// And hovered block highlight, and chunk aabb highlight;
	glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST); // note to self, if we disable depth_test. we dont write to it either, which means the next thing checking int the depth tester wont see what you have not written there.... aka the skybox in this case

	for (int nth_chunk = 0; nth_chunk < CHUNKS_LOADED; ++nth_chunk)
	{
		if (!world->chunks[nth_chunk].has_visible_blocks)
			continue ;
		if (world->block_collision_enabled)
		{
			if (point_aabb_center_distance(player->camera->pos, &world->chunks[nth_chunk].aabb) <= (CHUNK_WIDTH * CHUNK_WIDTH))
			{
				show_chunk_borders(&world->chunks[nth_chunk], player->camera, (float []){1, 0, 0});
				// Place Blocking and Removing;
				// Go through all chunks and check for collision on blocks,
				// store intersections and indices of the chunk the intersection
				// is in;

				// Collision on solid mesh;
				if (world->chunks[nth_chunk].blocks_solid_amount > 0)
				{
					int collisions_on_chunk = chunk_mesh_collision_v2(player->camera->pos, player->camera->front, &world->chunks[nth_chunk].meshes, BLOCK_MESH, world->chunks[nth_chunk].world_coordinate, player->info.reach, intersect_point + collision_result);
					for (int i = 0; i < collisions_on_chunk; i++)
						intersect_chunk_index[collision_result + i] = nth_chunk;
					collision_result += collisions_on_chunk;
				}
			}
		}
	}

	/* * * * * START OF COLLISION * * * * */
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
			distancione = v3_dist_sqrd(player->camera->pos, intersect_point[colli]);
			if (distancione < closest_dist)
			{
				closest_dist = distancione;
				v3_assign(closest_point, intersect_point[colli]);
				closest_index = intersect_chunk_index[colli];
			}
		}

		t_block *hovered_block = NULL;
		hovered_block = get_block_from_chunk(&world->chunks[closest_index], closest_point, block_pos, &face);
		if (hovered_block)
		{
			render_block_outline(block_pos, (float []){0, 0, 0}, player->camera->view, player->camera->projection);
			block_world_to_local_pos(block_local, block_pos);
		}
		// Lets summon mob on the hovered_block
		if (hovered_block && vox->keys[GLFW_KEY_M].state == BUTTON_PRESS)
			create_entity_at_world_pos(&world->entity_manager, v3_add(block_pos, block_pos, (float *)g_card_dir[DIR_UP]), ENTITY_MELON_GOLEM);
		else if (hovered_block &&
			(vox->mouse[GLFW_MOUSE_BUTTON_LEFT].state == BUTTON_PRESS ||
			vox->mouse[GLFW_MOUSE_BUTTON_RIGHT].state == BUTTON_PRESS ||
			vox->mouse[GLFW_MOUSE_BUTTON_MIDDLE].state == BUTTON_PRESS))
		{
			if (vox->mouse[GLFW_MOUSE_BUTTON_LEFT].state == BUTTON_PRESS)
				set_block_at_world_pos(world, block_pos, GAS_AIR);
			else if (vox->mouse[GLFW_MOUSE_BUTTON_RIGHT].state == BUTTON_PRESS)
			{
				float	block_world[3];
				v3_add(block_world, block_pos, (float *)g_card_dir[face]);
				// Check if block or item equipped;
				LG_INFO("Place Item at %f %f %f", block_world[0], block_world[1], block_world[2]);
				if (!is_type_item(player->info.equipped_block))
					set_block_at_world_pos(world, block_world, player->info.equipped_block);
				else if (is_type_item(player->info.equipped_block))
				{
					ft_printf("Using : %s\n", g_item_data[player->info.equipped_block - ITEM_FIRST - 1].name);
					if (player->info.equipped_block == ITEM_TREE_PLACER)
						tree_placer(world, block_world);
				}
				else
					LG_WARN("We dont allow the placing of that type of block.");
			}
			else if (vox->mouse[GLFW_MOUSE_BUTTON_MIDDLE].state == BUTTON_PRESS)
			{
				block_print(hovered_block);
				v3i_string("Local Coordinates : ", block_local);
				v3_string("World Coordinates : ", block_pos);
				t_chunk *temp_chunk = get_chunk_from_world_pos(world, block_pos);
				v3i_string("Chunk Coordinates : ", temp_chunk->coordinate);
				v3_string("Chunk World Coordinates : ", temp_chunk->world_coordinate);

			}
		}
	}
	/* END OF COLLISION */

	/* * * * * START OF PLAYER ENTITY HITBOX COLLISION * * * * */
	if (vox->settings.entity_hitbox_enabled)
	{
		t_entity	*rent;
		t_aabb		rent_aabb;
		for (int i = 0; i < world->entity_manager.entity_amount; i++)
		{
			if (!world->entity_manager.slot_occupied[i])
				continue;

			rent = &world->entity_manager.entities[i];
			// Continue if we are not within reach;
			if (v3_dist_sqrd(player->camera->pos, rent->pos) > player->info.reach * player->info.reach)
				continue ;
			v3_add(rent_aabb.min, world->entity_manager.entity_models[(int)rent->type].bound.min, rent->pos);
			v3_add(rent_aabb.max, world->entity_manager.entity_models[(int)rent->type].bound.max, rent->pos);
			if (aabb_ray_intersection(&rent_aabb, player->camera->pos, player->camera->front)) //v3_multiply_f(tmp2, player.camera->front, player_info.reach)))
			{
				render_3d_rectangle(rent_aabb.min, rent_aabb.max,
					(float []){255, 255, 0}, player->camera->view, player->camera->projection);
			}
		}
	}
	/* END OF PLAYER ENTITY HITBOX COLLISION */

	/// SKY BOX RENDER
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	render_skybox(skybox, player->camera);

	//// CHUNK RENDERING ////
	//decide_which_chunks_to_render(world);

	// Solid mesh;
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	/* Render every chunk 
	for (int r = 0; r < world->meshes_render_amount; r++)
	{
		int render_index = world->meshes_render_indices[r];
		if (world->chunks[render_index].blocks_solid_amount > 0)
			render_chunk_mesh(&world->chunks[render_index].meshes, BLOCK_MESH,
				world->chunks[render_index].world_coordinate, player->camera);
	}
	*/
	// Render every chunk from column;	
	for (int i = 0; i < CHUNK_COLUMN_AMOUNT; i++)
	{
		t_chunk_column *column = &world->chunk_columns[i];

		if (column->regeneration_threaded || column->update_threaded)
			continue;

		for (int j = 0; j < CHUNKS_PER_COLUMN; j++)
		{
			t_chunk *chunk = column->chunks[j];

			// If the chunk has no visible block,
			//	OR chunk has no solid blocks,
			//	OR we haven uploaded the mesh to the gpu yet;
			if (!chunk->has_visible_blocks ||
				chunk->blocks_solid_amount <= 0 ||
				chunk->was_updated)
				continue;

			render_chunk_mesh(&chunk->meshes, BLOCK_MESH, chunk->world_coordinate, player->camera);
		}
	}


	// Fluid meshes;
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	/*
	for (int r = 0; r < world->meshes_render_amount; r++)
	{
		int render_index = world->meshes_render_indices[r];
		if (world->chunks[render_index].blocks_fluid_amount > 0)
			render_chunk_mesh(&world->chunks[render_index].meshes, FLUID_MESH,
				world->chunks[render_index].world_coordinate, player->camera);
	}
	*/
	for (int i = 0; i < CHUNK_COLUMN_AMOUNT; i++)
	{
		t_chunk_column *column = &world->chunk_columns[i];

		if (column->regeneration_threaded || column->update_threaded)
			continue;

		for (int j = 0; j < CHUNKS_PER_COLUMN; j++)
		{
			t_chunk *chunk = column->chunks[j];
			if (!chunk->has_visible_blocks ||
				chunk->blocks_fluid_amount <= 0 ||
				chunk->was_updated)
				continue;

			render_chunk_mesh(&chunk->meshes, FLUID_MESH, chunk->world_coordinate, player->camera);
		}
	}

	// Entity Rendering
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	entity_manager_draw(&world->entity_manager, &world->camera);
}

void camera_settings_init(t_camera *camera, float win_w, float win_h)
{
	camera->pitch = 0;
	camera->yaw = -90;
	camera->viewport_w = win_w;
	camera->viewport_h = win_h;
	camera->far_plane = CAMERA_FAR_PLANE;
}

void	world_init(t_world *world)
{
	world->seed = 896868766;
	world->block_collision_enabled = 0;
	world->player_collision_enabled = 0;
	world->fancy_graphics = 0;
	world->generate_structures = 0;
	world->light_calculation = 0;
	world->toggle_event = 0;
	world->generate_caves = 0;
	world->sky_light_lvl = 15;
	
	// Set spawn point;
	v3_new(world->spawn_point, 5000, 90, 5000);

	// Create camera;
	new_camera(&world->camera);

	// Create entity manager;
	entity_manager_init(&world->entity_manager, 2);
	entity_manager_load_entity_objects(&world->entity_manager);

	// Creation of rendering lists;
	world->meshes_render_indices = malloc(sizeof(int) * CHUNKS_LOADED);
	world->meshes_render_amount = 0;

	glGenTextures(1, &world->texture);
	new_texture(&world->texture, TEXTURE_PATH"version_3_texture_alpha.bmp");

	world->chunks = malloc(sizeof(t_chunk) * CHUNKS_LOADED);
	world->chunk_columns = malloc(sizeof(t_chunk_column) * CHUNK_COLUMN_AMOUNT);

	// Get player chunk;
	get_chunk_pos_from_world_pos(world->player_chunk, world->camera.pos);

	LG_INFO("Inits done, lets create some chunks (%d wanted)\n", CHUNKS_LOADED);

	GLuint	cube_shader_v2;
	new_shader(&cube_shader_v2, SHADER_PATH"block_mesh.vs", SHADER_PATH"block_mesh.fs");

	int	nth_col = -1;
	int	nth_col_chunk = 0;
	int	nth_chunk = 0;
	for (; nth_chunk < CHUNKS_LOADED; nth_chunk++)
	{
		new_chunk(&world->chunks[nth_chunk], world, nth_chunk);
		init_chunk_mesh(&world->chunks[nth_chunk].meshes, cube_shader_v2, MESH_TYPE_AMOUNT);
		world->chunks[nth_chunk].meshes.texture = world->texture;
		if (nth_chunk % CHUNKS_PER_COLUMN == 0)
		{
			nth_col_chunk = 0;
			nth_col++;
			world->chunk_columns[nth_col].chunks = malloc(sizeof(t_chunk *) * CHUNKS_PER_COLUMN);
			world->chunk_columns[nth_col].lights = malloc(sizeof(t_light) * CHUNK_COLUMN_LIGHT_AMOUNT);
			world->chunk_columns[nth_col].height_map.map = NULL;

			world->chunk_columns[nth_col].coordinate[0] = world->chunks[nth_chunk].coordinate[0];
			world->chunk_columns[nth_col].coordinate[1] = world->chunks[nth_chunk].coordinate[2];
			world->chunk_columns[nth_col].world_coordinate[0] = world->chunks[nth_chunk].world_coordinate[0];
			world->chunk_columns[nth_col].world_coordinate[1] = world->chunks[nth_chunk].world_coordinate[2];
			world->chunk_columns[nth_col].update_structures = 0;
			world->chunk_columns[nth_col].regeneration_threaded = 0;
			world->chunk_columns[nth_col].update_threaded = 0;
			world->chunk_columns[nth_col].world = world;
		}
		world->chunk_columns[nth_col].chunks[nth_col_chunk] = &world->chunks[nth_chunk];
		nth_col_chunk++;
	}
	LG_INFO("Chunks created : %d\n", nth_chunk);
}

void player_init(t_player *player)
{
	player->gravity = 0;

	player->info.reach = 5;
	player->info.equipped_hotbar = 1;
	player->info.hotbar_item_ids[0] = BLOCK_DIRT;
	player->info.hotbar_item_ids[1] = BLOCK_STONE;
	player->info.hotbar_item_ids[2] = BLOCK_ALPHA_TORCH;
	player->info.hotbar_item_ids[3] = BLOCK_SAND;
	player->info.hotbar_item_ids[4] = FLORA_GRASS;
	player->info.hotbar_item_ids[5] = BLOCK_ALPHA_OAK_LEAF;
	player->info.hotbar_item_ids[6] = BLOCK_ALPHA_CACTUS;
	player->info.hotbar_item_ids[7] = BLOCK_TNT;
	player->info.hotbar_item_ids[8] = 0;//FLUID_WATER;
	player->info.equipped_block = player->info.hotbar_item_ids[player->info.equipped_hotbar];
}

int	main(void)
{
	t_vox	vox;
	init(&vox);

	t_fps	fps;
	new_fps(&fps);

	t_world	world;
	world_init(&world);

	camera_settings_init(&world.camera, vox.win_w, vox.win_h);

	// Player
	t_player	player;
	new_player(&player);
	player_init(&player);
	player.camera = &world.camera;

	// Skybox
	t_skybox	skybox;
	new_skybox(&skybox, g_mc_skybox);

	// set player pos to spawn point;
	v3_assign(player.camera->pos, world.spawn_point);

	// Entity Debug;
	debug_create_entities(&world.entity_manager, player.camera->pos, world.entity_manager.max_entities);
	// Melon
	world.entity_manager.melon_entity = &world.entity_manager.entities[0];//entity_manager_get_entity(&world.entity_manager, 0);
	world.entity_manager.melon_entity->type = ENTITY_MELON_GOLEM;
	world.entity_manager.melon_entity->ai = 0;
	world.entity_manager.melon_entity->draw_aabb = 1;
	world.entity_manager.melon_entity->draw_dir = 1;
	// Chicken
	world.entity_manager.chicken_entity = &world.entity_manager.entities[1];//entity_manager_get_entity(&world.entity_manager, 1);
	world.entity_manager.chicken_entity->type = ENTITY_MELON_GOLEM;
	//world.entity_manager.chicken_entity->type = ENTITY_CHICKEN;
	world.entity_manager.chicken_entity->ai = 0;
	world.entity_manager.chicken_entity->draw_aabb = 1;
	world.entity_manager.chicken_entity->draw_dir = 1;
	// END entity debug;

////////////////////////////////////////
	// UI TESTING
////////////////////////////////////////
	t_ui	ui;
	ui_init(&ui);

	t_ui_manager	ui_manager;
	ui_manager_init(&ui_manager);

	ui.keys = vox.keys;
	ui.mouse = vox.mouse;
	ui.manager = &ui_manager;
	ui.hotbar_item_id = player.info.hotbar_item_ids;
	ui.selected_hotbar = player.info.equipped_hotbar;
	LG_INFO("UI init done.");
////////////////////////////////////////
	// END UI TESTING
///////////////////////////////////////

	glfwSwapInterval(0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LINE_SMOOTH);
	glClearColor(1, 0, 1, 1);

	while (!glfwWindowShouldClose(vox.win))
	{
		update_fps(&fps);
		game_update(&vox, &fps, &player, &world, &ui, &skybox);
		gui_update(&vox, &ui, &ui_manager);

		glfwSwapBuffers(vox.win);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	uninit(&vox);
	return (0);
}
