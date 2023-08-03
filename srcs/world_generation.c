#include "world.h"
#include "chunk.h"

void chunk_generation(t_vox *vox, t_world *world)
{
	int use_multi_thread = true;
	if (use_multi_thread)
		thread_manager_check_threadiness(&vox->tm);

	if (vox->settings.regen_chunks)
	{
		int	max_regen_amount = RENDER_DISTANCE;
		int	col_indices[max_regen_amount];
		int	col_coords[max_regen_amount][2];
		int	start_coord[2];
		int tobegen = get_chunk_column_to_regen(world, world->player_chunk, col_indices, col_coords, max_regen_amount);
		for (int i = 0; i < tobegen && i < max_regen_amount; i++)
		{
			world->chunk_columns[col_indices[i]].wanted_coord[0] = col_coords[i][0];
			world->chunk_columns[col_indices[i]].wanted_coord[1] = col_coords[i][1];
			if (use_multi_thread)
				thread_manager_new_thread(&vox->tm, &regen_column_thread, &world->chunk_columns[col_indices[i]]);
			else
				regenerate_chunk_column(&world->chunk_columns[col_indices[i]], col_coords[i], world->seed);
		}

		assert(tobegen != -1);
	}
}

void chunk_update(t_world *world)
{
	t_chunk		*neighbors[DIR_AMOUNT];
	t_chunk		**col_chunks;
	t_chunk_col	*column;
	int			neighbors_found;

	for (int col = 0; col < CHUNK_COLUMNS; col++)
	{
		column = &world->chunk_columns[col];

		if (column->being_threaded)
			continue ;

		col_chunks = column->chunks;
		column->chunk_needs_update = 0;

		if (world->generate_structures && column->update_structures)
		{
			tree_gen(world, column);
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
		if (world->light_calculation &&
			(column->chunk_needs_update ||
			world->sky_light_lvl != world->sky_light_lvl_prev))
			update_chunk_column_light(column);

		// Other Chunk updates;
		for (int ent = 0; ent < CHUNKS_PER_COLUMN; ++ent)
		{
			neighbors_found = 0;

			// We only need to update the chunks if a chunk has been regenerated
			if ((col_chunks[ent]->needs_to_update ||
				(world->generate_structures && column->update_structures)))
			{
				// Get all neighbors for this chunk;
				for (int dir = DIR_NORTH, i = 0; dir < DIR_AMOUNT; ++dir, ++i)
				{
					neighbors[i] = get_adjacent_chunk(world, col_chunks[ent], (float *)g_card_dir[dir]);
					++neighbors_found;
				}
			}

			if (col_chunks[ent]->needs_to_update)
			{
				update_chunk(col_chunks[ent]);
				if (world->toggle_event)
					update_chunk_event_blocks(col_chunks[ent]);
				// Set needs to update to all 6 neighbors of the chunk;
				for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; ++dir, ++i)
					if (neighbors[i])
						neighbors[i]->secondary_update = 1;
			}

			col_chunks[ent]->needs_to_update = 0;

			if (world->toggle_event && world->game_tick)
				event_chunk(col_chunks[ent]);
		}
	}
}

void decide_which_chunks_to_render(t_world *world)
{
	// Reset the rendering amount to 0;
	world->meshes_render_amount = 0;

	for (int nth_chunk = 0; nth_chunk < CHUNKS_LOADED; ++nth_chunk)
	{
		if (!world->chunks[nth_chunk].has_visible_blocks)
			continue ;

		// Decide if we want to render the chunk or not;
		// Dont render chunk if the chunk is further away than the farplane of the camear;
		// Dont render if the chunk is outside the view fustrum;
		// Dont render if hasnt been sent to gpu yet;
		if (world->chunks[nth_chunk].was_updated == 0 &&
			v3_dist_sqrd(world->camera.pos, world->chunks[nth_chunk].world_coordinate) <
			(world->camera.far_plane + CHUNK_SIZE_X) * (world->camera.far_plane + CHUNK_SIZE_X) &&
			aabb_in_frustum(&world->chunks[nth_chunk].aabb, &world->camera.frustum))
		{
			world->meshes_render_indices[world->meshes_render_amount] = nth_chunk;
			world->meshes_render_amount++;
		}
	}
}