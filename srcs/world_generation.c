#include "world.h"
#include "chunk.h"

void chunk_generation(t_vox *vox, t_world *world)
{
	bool _useMultiThread = true;
	thread_manager_check_threadiness(&vox->tm_gen);

	if (vox->settings.regen_chunks)
	{
		int	max_regen_amount = RENDER_DISTANCE;
		int	col_indices[max_regen_amount];
		int	col_coords[max_regen_amount][2];
		int	start_coord[2];
		int tobegen = get_chunk_column_to_regen(world, world->player_chunk, col_indices, col_coords, max_regen_amount);
		t_chunk_column *column;
		for (int i = 0; i < tobegen && i < max_regen_amount; i++)
		{
			world->chunk_columns[col_indices[i]].wanted_coord[0] = col_coords[i][0];
			world->chunk_columns[col_indices[i]].wanted_coord[1] = col_coords[i][1];
			column = &world->chunk_columns[col_indices[i]];
			if (column->regeneration_threaded || column->update_threaded)
				continue;
			if (_useMultiThread)
				thread_manager_new_thread(&vox->tm_gen, regen_column_thread, column, regen_column_thread_start, regen_column_thread_end);
			else
				regenerate_chunk_column(column, col_coords[i], world->seed);
		}

		assert(tobegen != -1);
	}
}

void update_chunk_columns(t_vox *vox, t_world *world)
{
//	time_t _startTime = clock();
	t_chunk_column	*column;
	bool _useMultiThread = true;
	thread_manager_check_threadiness(&vox->tm_update);

//	int			_chunksUpdated = 0;

	for (int col = 0; col < CHUNK_COLUMN_AMOUNT; col++)
	{
		column = &world->chunk_columns[col];

		if (column->regeneration_threaded || column->update_threaded)
			continue ;

		t_chunk **chunksInColumn = column->chunks;
		// Check if a chunk in the column needs an update;
		for (int chunkIndex = 0; chunkIndex < CHUNKS_PER_COLUMN; chunkIndex++)
		{
			if (chunksInColumn[chunkIndex]->needs_to_update)
			{
				update_chunk_block_palette(chunksInColumn[chunkIndex]);
				column->chunk_needs_update = 1;
			}
		}

		// If the chunk doesnt need update, we can gtfo;
		if (!column->chunk_needs_update)
			continue;
		
		if (_useMultiThread)
			thread_manager_new_thread(&vox->tm_update, update_column_thread, column, update_column_thread_start, update_column_thread_end);
		else
			update_chunk_column(column);
	}
	/*
	if (_chunksUpdated > 0)
	{
		time_t endTime = clock(); // We want to take the time before we print, for the actual code time;
		LG_INFO("%d chunks updated", _chunksUpdated);
		LG_INFO("Time taken : %f", (float)(endTime - _startTime) / CLOCKS_PER_SEC);
	}
	*/
}

void update_chunk_column(t_chunk_column *column)
{
	t_world *world = column->world;
	t_chunk **chunksInColumn = column->chunks;

	column->chunk_needs_update = 0;

	// NOTE : this might break when threaded, dont know what happens if threaded
	//	columns try to add blocks to other columns. Maybe the tree gen should be
	//	outside threading;
	/*
	if (world->generate_structures && column->update_structures)
	{
		tree_gen(world, column);
		column->update_structures = 0;
	}
	*/

	// Light calculation;
	/* NOTE : Here we might have the same problems as with structure generation;
	if (world->light_calculation &&
		(column->chunk_needs_update ||
		world->sky_light_lvl != world->sky_light_lvl_prev))
		update_chunk_column_light(column);
	*/

	// Other Chunk updates;
	/*
	*/
	t_chunk	*neighbors[DIR_AMOUNT];
	for (int chunkIndex = 0; chunkIndex < CHUNKS_PER_COLUMN; chunkIndex++)
	{
		/*
		*/
		int neighbors_found = 0;

		// We only need to update the chunks if a chunk has been regenerated
		if ((chunksInColumn[chunkIndex]->needs_to_update ||
			(world->generate_structures && column->update_structures)))
		{
			// Get all neighbors for this chunk;
			for (int dir = DIR_NORTH, i = 0; dir < DIR_AMOUNT; dir++, i++)
			{
				neighbors[i] = get_adjacent_chunk(world, chunksInColumn[chunkIndex], (float *)g_card_dir[dir]);
				neighbors_found++;
			}
		}

		if (chunksInColumn[chunkIndex]->needs_to_update)
		{
			update_chunk(chunksInColumn[chunkIndex]);
			if (world->toggle_event)
				update_chunk_event_blocks(chunksInColumn[chunkIndex]);
			// Set needs to update to all 6 neighbors of the chunk;
			for (int dir = DIR_NORTH, i = 0; dir <= DIR_DOWN; dir++, i++)
				if (neighbors[i])
					neighbors[i]->secondary_update = 1;
			/*
					*/
		}

		chunksInColumn[chunkIndex]->needs_to_update = 0;

		/*
		if (world->toggle_event && world->game_tick)
			event_chunk(chunksInColumn[ent]);
		*/
	}
}

void *update_column_thread_start(void *args)
{
	t_chunk_column *column;
	
	column = args;
	column->update_threaded = 1;
	return (NULL);
}
void *update_column_thread(void *args)
{
	t_chunk_column *column;
	
	column = args;
//	column->update_threaded = 1;
	update_chunk_column(column);
//	column->update_threaded = 0;
	return (NULL);
}
void *update_column_thread_end(void *args)
{
	t_chunk_column *column;
	
	column = args;
	column->update_threaded = 0;
	return (NULL);
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