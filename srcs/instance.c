#include "shaderpixel.h"

// Basically everything needed for minecraft : https://www.redblobgames.com/maps/terrain-from-noise/
void	new_chunk(t_chunk *chunk, t_chunk_info *info, int nth)
{
	int error = glGetError();
	if (error)
		LG_ERROR("Before (%d)", error);

	chunk->info = info;

	chunk->needs_to_update = 0;

	int	max_blocks = chunk->info->width * chunk->info->breadth * chunk->info->height;

	chunk->blocks = malloc(sizeof(t_block) * (max_blocks));

	// Set INT_MAX to coordinates, so that the chunk regenerator knows to regenerate these chunks;
	for (int i = 0; i < 3; i++)
	{
		chunk->coordinate[i] = 999 - (i * nth);
		chunk->world_coordinate[i] = 999 - (i * nth);
	}
	/*
	int key = get_chunk_hash_key(chunk->coordinate);
	hash_item_insert(info->hash_table, info->hash_table_size, key, nth);
	*/

	init_chunk_mesh(&chunk->mesh);
	init_chunk_mesh(&chunk->liquid_mesh);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

int	chunk_gen(t_chunk *chunk)
{
	int		start_y = 64;
	float	freq = 0.005f;
	float	pers = 0.5;
	int		i = 0;

	chunk->has_blocks = 0;

	for (int x = 0; x < chunk->info->width; x++)
	{
		float	block_world_x = fabs(chunk->world_coordinate[0] + x);
		float	to_use_x = block_world_x * freq;
		for (int z = 0; z < chunk->info->breadth; z++)
		{
			float	block_world_z = fabs(chunk->world_coordinate[2] + z);
			float	to_use_z = block_world_z * freq;
			float	perper =
				octave_perlin(to_use_x, start_y * freq, to_use_z, 1, pers) +
				octave_perlin(to_use_x, start_y * freq, to_use_z, 2, pers) +
				octave_perlin(to_use_x, start_y * freq, to_use_z, 4, pers) +
				octave_perlin(to_use_x, start_y * freq, to_use_z, 8, pers);
			float	e = pers * 3;
			int		wanted_y = (start_y * (perper / e));
			int		whatchumacallit = wanted_y - (chunk->world_coordinate[1]);
			int		amount = ft_clamp(whatchumacallit, 0, chunk->info->height - 1);

			for (int y = 0; y < chunk->info->height; y++)
			{
				float	block_world_y = (chunk->world_coordinate[1] + y);
				/* ////// CAVE GEN ///////// */
				/*
				float	cave_freq = 0.005f;
				float	cave_height = cave_freq * 200;
				float	cave_x = block_world_x * cave_freq;
				float	cave_z = block_world_z * cave_freq;
				float	cave_y = (chunk->world_coordinate[1] + y) * cave_freq;
				float	rep = 1.0f;

				rep =
					octave_perlin(cave_x, cave_y, cave_z, 2, 0.5) +
					octave_perlin(cave_x, cave_y, cave_z, 4, 0.5) +
					octave_perlin(cave_x, cave_y, cave_z, 8, 0.5) +
					octave_perlin(cave_x, cave_y, cave_z, 16, 0.5);
					*/

//				vec3_new(chunk->blocks[i].pos, x, y, z);
				if (/*rep > 1.0f &&*/ y <= whatchumacallit)
				{
					if (y <= whatchumacallit - 1) // if we have 3 dirt block on top we make the rest stone blocks;
					{
						chunk->blocks[i].type = BLOCK_STONE;
					}
					else
					{
						if (block_world_y <= 67 && block_world_y >= 65)
							chunk->blocks[i].type = BLOCK_SAND;
						else if (block_world_y < 4)
							chunk->blocks[i].type = BLOCK_BEDROCK;
						else
							chunk->blocks[i].type = BLOCK_DIRT;
					}
					chunk->has_blocks = 1;
				}
				else
				{
					if (block_world_y == 65)
					{
						chunk->blocks[i].type = BLOCK_WATER;
						chunk->has_blocks = 1;
					}
					else
						chunk->blocks[i].type = BLOCK_AIR;
				}
				i++;
			}
		}
	}
	return (i);
}

/*
 * Give any world position to 'world_coords' and this calculates the 
 *	chunk coordinate that you're in;
*/
int	*get_chunk_pos_from_world_pos(int *res, float *world_coords, t_chunk_info *info)
{
	res[0] = floor(world_coords[0] / info->chunk_size[0]);
	res[1] = floor(world_coords[1] / info->chunk_size[1]);
	res[2] = floor(world_coords[2] / info->chunk_size[2]);
	return (res);
}

void	chunk_aabb_update(t_chunk *chunk)
{
	t_aabb	*a;

	a = &chunk->aabb;
	a->min[0] = chunk->world_coordinate[0] - (chunk->info->block_size / 2);
	a->min[1] = chunk->world_coordinate[1] - (chunk->info->block_size / 2);
	a->min[2] = chunk->world_coordinate[2] - (chunk->info->block_size / 2);
	a->max[0] = a->min[0] + chunk->info->chunk_size[0];
	a->max[1] = a->min[1] + chunk->info->chunk_size[1];
	a->max[2] = a->min[2] + chunk->info->chunk_size[2];
}

/*
 * Returns pointer to chunk if we can find the correct one;
 * 	else NULL;
 * 
 * 'from' is the chunk we want to look in a direction from,
 * 'chunks' is all the loaded chunks,
 * 'dir' is the direction you want to look for the chunk in; (v3)
*/
t_chunk	*get_adjacent_chunk(t_chunk *from, t_chunk *chunks, int *dir)
{
	int	from_coord[3];

	from_coord[0] = from->coordinate[0] + dir[0];
	from_coord[1] = from->coordinate[1] + dir[1];
	from_coord[2] = from->coordinate[2] + dir[2];
/*
	int key = get_chunk_hash_key(from_coord);
	t_hash_item *item = hash_item_search(from->info->hash_table, from->info->hash_table_size, key);
	if (!item)
		return (NULL);
	return (&from->info->chunks[item->data]);
	*/

	for (int i = 0; i < from->info->chunks_loaded; i++)
	{
		if (chunks[i].coordinate[0] == from_coord[0] &&
			chunks[i].coordinate[1] == from_coord[1] &&
			chunks[i].coordinate[2] == from_coord[2])
			return (&chunks[i]);
	}
	return (NULL);
	/*
	*/
}

t_chunk	*get_chunk(t_chunk_info	*info, int *pos)
{
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		if (info->chunks[i].coordinate[0] == pos[0] && 
			info->chunks[i].coordinate[1] == pos[1] && 
			info->chunks[i].coordinate[2] == pos[2])
			return (&info->chunks[i]);
	}
	return (NULL);
}

/*
 * From local chunk block pos to the index in the 'chunk->blocks' array;
*/
int	get_block_index(t_chunk_info *info, int x, int y, int z)
{
	return ((x * info->width * info->breadth) + (z * info->height) + y);
}

int	*block_world_to_local_pos(int *res, float *world)
{
	res[0] = mod(world[0], 16);
	res[1] = mod(world[1], 16);
	res[2] = mod(world[2], 16);
	return (res);
}

/*
 * Saves answer in 'res';
 * And returns it;
*/
float *block_world_pos(float *res, float *chunk_world_pos, int *block_local_pos)
{
	float	scale = 0.5; // t_chunk_info->block_scale;

	res[0] = chunk_world_pos[0] + (block_local_pos[0] * scale);
	res[1] = chunk_world_pos[0] + (block_local_pos[1] * scale);
	res[2] = chunk_world_pos[0] + (block_local_pos[2] * scale);
	return (res);
}

/*
 * From 'index' in 'chunk->blocks' get the x,y,z pos in the chunk coordinates;
 * 	'max' is the max width, breadth and height of the chunk;
*/
int	*get_block_local_pos_from_index(int *res, int *max, int index)
{
	// TODO use chunk->w, h, b;
	res[0] = index / (16 * 16);
	res[2] = (index / 16) % 16;
	res[1] = index % 16;
	return (res);
}

/*
 * Returns pointer to block anywhere in the world with the 'coord's given;
 * World coordinates btw;
*/
t_block	*get_block(t_chunk_info *info, float *coords)
{
	int		chunk_pos[3];
	t_chunk	*chunk;

	get_chunk_pos_from_world_pos(chunk_pos, coords, info);
	chunk = get_chunk(info, chunk_pos);
	if (chunk)
	{
		int	local_pos[3];
		block_world_to_local_pos(local_pos, coords);
		int index = get_block_index(info, local_pos[0], local_pos[1], local_pos[2]);
		return (&chunk->blocks[index]);
	}
	return (NULL);
}

void	adjacent_block_checker(t_chunk *chunk, int i, int *local_pos, int face, t_chunk *neighbor)
{
	t_block	*tmp_block;
	t_block	*blocks = chunk->blocks;
	int		x, y, z;

	x = local_pos[0];
	y = local_pos[1];
	z = local_pos[2];

	if (face == FACE_LEFT)
		x -= 1;
	else if (face == FACE_RIGHT)
		x += 1;
	else if (face == FACE_TOP)
		y += 1;
	else if (face == FACE_BOT)
		y -= 1;
	else if (face == FACE_FRONT)
		z += 1;
	else if (face == FACE_BACK)
		z -= 1;

	tmp_block = NULL;
	if (x >= 0 && x <= 15 &&
		y >= 0 && y <= 15 &&
		z >= 0 && z <= 15)
		tmp_block = &blocks[get_block_index(chunk->info, x, y, z)];
	else if (neighbor)
		tmp_block = &neighbor->blocks[get_block_index(chunk->info, mod(x, 16), mod(y, 16), mod(z, 16))];
	else
		return ;
	if (tmp_block && (!g_block_data[tmp_block->type + 1].solid))
	{
		// If both blocks are liquid, we dont add face to mesh;
		if (!(g_block_data[blocks[i].type + 1].liquid && g_block_data[tmp_block->type + 1].liquid))
		{
			if (g_block_data[blocks[i].type + 1].liquid)
			{
				add_to_chunk_mesh(&chunk->liquid_mesh, local_pos, (float *)g_faces[face], g_block_data[blocks[i].type + 1].face_texture[face], (int)g_face_light[face]);
				++chunk->blocks_liquid_amount;
			}
			else
			{
				add_to_chunk_mesh(&chunk->mesh, local_pos, (float *)g_faces[face], g_block_data[blocks[i].type + 1].face_texture[face], (int)g_face_light[face]);
				++chunk->blocks_solid_amount;
			}
		}
	}
}

/*
 * For each block in chunk;
 *	Go through each block in chunk;
 *		check if not touching solid block;
 *	if touching;
 *		add to render array;
 *
 * THIS MIGHT ONLY WORK NOW THAT THE CHUNKS ARE SYMMETRIC, IF SOMETHING BREAKS
 * CHECK THAT THE XYZ CORRESPOND CORRECTLY!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/
void	get_blocks_visible(t_chunk *chunk)
{
	t_block	*blocks;

	chunk->blocks_solid_amount = 0;
	chunk->blocks_liquid_amount = 0;

	if (!chunk->has_blocks)
		return ;
	blocks = chunk->blocks;

	// Get all neighbors of current chunk;
	t_chunk *neighbors[6];
	for (int i = 0; i < 6; i++)
		neighbors[i] = get_adjacent_chunk(chunk, chunk->info->chunks, (int *)g_neighbors[i]);

	int		pos[3];
	for (int i = 0; i < chunk->block_amount; i++)
	{
		if (blocks[i].type == BLOCK_AIR) // <-- very important, im not sure what happens if we are trying to render an air block;
			continue ;

		get_block_local_pos_from_index(pos, (int []){16, 16, 16}, i);

		adjacent_block_checker(chunk, i, pos, FACE_LEFT, neighbors[0]);
		adjacent_block_checker(chunk, i, pos, FACE_RIGHT, neighbors[1]);
		adjacent_block_checker(chunk, i, pos, FACE_TOP, neighbors[2]);
		adjacent_block_checker(chunk, i, pos, FACE_BOT, neighbors[3]);
		adjacent_block_checker(chunk, i, pos, FACE_FRONT, neighbors[4]);
		adjacent_block_checker(chunk, i, pos, FACE_BACK, neighbors[5]);
	}
}

int	get_chunk_hash_key(int *coords)
{
	int	res;

	res = coords[0] +
		(8 * coords[1]) +
		(8 * 8 * coords[2]);
	return (res);
}

void	update_chunk(t_chunk *chunk, int *coord)
{
	/*
	int	old_key = get_chunk_hash_key(chunk->coordinate);
	t_hash_item	*old_item = hash_item_search(chunk->info->hash_table, chunk->info->hash_table_size, old_key);
	int				old_data = old_item->data;
	hash_item_delete(chunk->info->hash_table, chunk->info->hash_table_size, old_key);
	*/

	for (int i = 0; i < 3; i++)
		chunk->coordinate[i] = coord[i];
	vec3_new(chunk->world_coordinate,
		chunk->coordinate[0] * chunk->info->chunk_size[0],
		chunk->coordinate[1] * chunk->info->chunk_size[1],
		chunk->coordinate[2] * chunk->info->chunk_size[2]);

/*
	int	new_key = get_chunk_hash_key(chunk->coordinate);	
	hash_item_insert(chunk->info->hash_table, chunk->info->hash_table_size, new_key, old_data);
	*/

	// Generate Chunks	
	chunk->block_amount = chunk_gen(chunk); // should always return max amount of blocks in a chunk;

	chunk->needs_to_update = 1;
}

void	*update_chunk_threaded(void *arg)
{
	t_chunk_args	*info = arg;

	update_chunk(info->chunk, info->coords);
	info->chunk->args.being_threaded = 0;
	return (NULL);
}

/*
 * Check which chunks are not going to be in the next iteration of
 *	loaded chunks, save those to 'reload_these_chunks' and when starting
 *	to update the new chunks that are going to be loaded, and put the
 *	new chunk info into those 'chunks' indices;
 * Takes 0.000000 seconds;
*/
int	get_chunks_to_reload(int *chunks, int *start_coord, t_chunk_info *info, int *player_chunk_v3)
{
	int	reload_amount = 0;
	int	found = 0;

	start_coord[0] = player_chunk_v3[0] - (info->render_distance / 2);
	start_coord[1] = 0;
	start_coord[2] = player_chunk_v3[2] - (info->render_distance / 2);
	for (int i = 0; i < info->chunks_loaded; i++)
	{
		found = 0;
		for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
		{
			for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
			{
				if (info->chunks[i].coordinate[0] == x && info->chunks[i].coordinate[2] == z)
				{
					found = 1;
					break ;
				}
			}
			if (found)
				break ;
		}
		if (!found)
		{
			chunks[reload_amount] = i;
			reload_amount++;
		}
	}
	return (reload_amount);
}

void	regenerate_chunks(t_chunk *chunks, t_chunk_info *info, int *player_chunk_v3)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	start_coord[3];
	int found = 0;
	int reload_amount;
	
	reload_amount = get_chunks_to_reload(reload_these_chunks, start_coord, info, player_chunk_v3);
	if (reload_amount <= 0)
		return ;
	
	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	int	nth_chunk = 0;

	for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
	{
		for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
		{
			found = 0;
			for (int i = 0; i < info->chunks_loaded; i++)
			{
				if (chunks[i].coordinate[0] == x && chunks[i].coordinate[2] == z)
				{
					found = 1;
					break ;
				}
			}
			if (!found)
			{
				for (int y = start_coord[1], y_amount = 0; y_amount < info->y_chunk_amount; y++, y_amount++)
				{
					if (nth_chunk >= 16)
						break ;
					int index = reload_these_chunks[nth_chunk];
					chunks[index].args.chunk = &chunks[index];
					chunks[index].args.coords[0] = x;
					chunks[index].args.coords[1] = y;
					chunks[index].args.coords[2] = z;
					update_chunk_threaded(&chunks[index].args);
					nth_chunk++;
					if (nth_chunk >= info->chunks_loaded) 
						break ;
				}
			}
			if (nth_chunk >= info->chunks_loaded) 
				break ;
		}
		if (nth_chunk >= info->chunks_loaded) 
			break ;
	}
}

void	regenerate_chunks_v3(t_chunk *chunks, t_chunk_info *info, int *player_chunk_v3, t_thread_manager *tm)
{
	int	reload_these_chunks[info->chunks_loaded];
	int	start_coord[3];
	int found = 0;
	int reload_amount;
	
	reload_amount = get_chunks_to_reload(reload_these_chunks, start_coord, info, player_chunk_v3);
	if (reload_amount <= 0)
		return ;
	if (tm->thread_amount + 63 >= tm->max_thread_amount)
	{
	//	LG_WARN("Max threads reached");
		return ;
	}
	
	// Go through all the coordinates that will be loaded next time, and
	//  check if any of the loaded chunks have those coordinates, if not
	//	we take one of the chunks that are not going to be loaded next time
	// 	and update the new chunk into that memory;
	int	nth_chunk = 0;

	for (int x = start_coord[0], x_amount = 0; x_amount < info->render_distance; x++, x_amount++)
	{
		for (int z = start_coord[2], z_amount = 0; z_amount < info->render_distance; z++, z_amount++)
		{
			found = 0;
			for (int i = 0; i < info->chunks_loaded; i++)
			{
				if (chunks[i].coordinate[0] == x && chunks[i].coordinate[2] == z)
				{
					found = 1;
					break ;
				}
			}
			if (!found)
			{
				for (int y = start_coord[1], y_amount = 0; y_amount < info->y_chunk_amount; y++, y_amount++)
				{
					if (nth_chunk >= 64)
						break ;
					int index = reload_these_chunks[nth_chunk];
					chunks[index].args.chunk = &chunks[index];
					chunks[index].args.coords[0] = x;
					chunks[index].args.coords[1] = y;
					chunks[index].args.coords[2] = z;
					if (!thread_manager_new_thread(tm, update_chunk_threaded, &chunks[index].args))
						LG_WARN("Couldnt create new thread (nth : %d)", nth_chunk);
					nth_chunk++;
					if (nth_chunk >= info->chunks_loaded) 
						break ;
				}
			}
			if (nth_chunk >= info->chunks_loaded) 
				break ;
		}
		if (nth_chunk >= info->chunks_loaded) 
			break ;
	}
}

void	show_chunk_borders(t_chunk *chunk, t_camera *camera, float *col)
{
	render_aabb(&chunk->aabb, camera, col);
}

void	render_aabb(t_aabb *a, t_camera *camera, float *col)
{
	// VERTICAL LINES
	render_3d_line(
		(float []){a->min[0], a->min[1], a->min[2]},
		(float []){a->min[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);

	// HORIZONTAL LINES Bottom
	render_3d_line(
		(float []){a->min[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->min[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->min[2]},
		(float []){a->max[0], a->min[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->min[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->min[1], a->max[2]},
		(float []){a->min[0], a->min[1], a->min[2]},
		col,
		camera->view, camera->projection);
	// TOP
	render_3d_line(
		(float []){a->min[0], a->max[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->max[1], a->min[2]},
		(float []){a->max[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->max[0], a->max[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->max[2]},
		col,
		camera->view, camera->projection);
	render_3d_line(
		(float []){a->min[0], a->max[1], a->max[2]},
		(float []){a->min[0], a->max[1], a->min[2]},
		col,
		camera->view, camera->projection);
}

/*
 * I think this has to be called after all the chunk creation since we need all the adjacent
 *	chunks to already exist, and if the update_chunk is multithreaded we might not have 
 *	created the needed chunks yet;
*/
void	update_chunk_visible_blocks(t_chunk *chunk)
{
	int	blocks_visible[2]; // 0 : solid, 1 : liquid;

	reset_chunk_mesh(&chunk->mesh);
	reset_chunk_mesh(&chunk->liquid_mesh);

	get_blocks_visible(chunk);
}

/*
 * Makes all the values 0, doesnt free anything;
*/
void	reset_chunk_mesh(t_chunk_mesh *mesh)
{
	mesh->vertices_amount = 0;
	mesh->texture_id_amount = 0;
	mesh->indices_amount = 0;
	mesh->index_amount = 0;
}

/*
 * Creates all the opengl stuff that needs to be created only once;
*/
void	init_chunk_mesh(t_chunk_mesh *mesh)
{
	GLuint	vbo[2];
	int		error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	mesh->vertices_allocated = 6972;
	mesh->vertices = malloc(sizeof(float) * mesh->vertices_allocated);
	mesh->vertices_amount = 0;

	mesh->texture_ids_allocated = 2324; 
	mesh->texture_ids = malloc(sizeof(int) * mesh->texture_ids_allocated);
	mesh->texture_id_amount = 0;

	mesh->indices_allocated = 3486;
	mesh->indices = malloc(sizeof(unsigned int) * mesh->indices_allocated);
	mesh->indices_amount = 0;

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // tex

	glGenBuffers(2, vbo);
	mesh->vbo_pos = vbo[0];
	mesh->vbo_texture_ids = vbo[1];

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texture_ids);
	glVertexAttribIPointer(1, 1, GL_INT, sizeof(int), NULL);

	glGenBuffers(1, &mesh->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * 'coordinate' world coordinate of the chunk;
*/
void	render_chunk_mesh(t_chunk_mesh *mesh, float *coordinate, t_camera *camera, t_shader *shader)
{
	int	error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);
	
	glUseProgram(shader->program);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "view"), 1, GL_FALSE, &camera->view[0]);
	glUniformMatrix4fv(glGetUniformLocation(shader->program, "projection"), 1, GL_FALSE, &camera->projection[0]);

	glUniform3fv(glGetUniformLocation(shader->program, "chunkPos"), 1, &coordinate[0]);

	glBindVertexArray(mesh->vao);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glDrawElements(GL_TRIANGLES, mesh->indices_amount, GL_UNSIGNED_INT, NULL);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

void	update_chunk_mesh(t_chunk_mesh *mesh)
{
	int error;
	error = glGetError();
	if (error)
		LG_ERROR("BEFORE (%d)", error);

	glBindVertexArray(mesh->vao);

error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
	// Pos
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, mesh->vertices_amount * sizeof(float),
		&mesh->vertices[0], GL_STATIC_DRAW);

	error = glGetError();
	if (error)
	{
		LG_INFO("Vertices Amount : %d", mesh->vertices_amount);
		LG_ERROR("(%d)", error);
	}

	// Texture ID
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_texture_ids);
	glBufferData(GL_ARRAY_BUFFER, mesh->texture_id_amount * sizeof(int),
		&mesh->texture_ids[0], GL_STATIC_DRAW);

error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	// EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices_amount * sizeof(unsigned int),
		&mesh->indices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

/*
 * 'coord' position of block in the chunk->blocks array;
 *
 * You give in the mesh you want to add the face to;
 * Otherwise same as the old version;
 * 
 * You give either solid mesh or liquid mesh to this;
*/
void	add_to_chunk_mesh(t_chunk_mesh *mesh, int *coord, float *face_vertices, int texture_id, int light)
{
	// @Modulate (aka make modular)
	float	block_scale = 0.5f;

// Vertices and Texture
	if (mesh->vertices_allocated < mesh->vertices_amount + 12)
	{
		mesh->vertices_allocated += 256;
		mesh->vertices = realloc(mesh->vertices, sizeof(float) * mesh->vertices_allocated);
		LG_WARN("Reallocating Vertices from %d to %d", mesh->indices_allocated - 256, mesh->indices_allocated);
	}
	if (mesh->texture_ids_allocated < mesh->texture_id_amount + 4)
	{
		mesh->texture_ids_allocated += 256;
		mesh->texture_ids = realloc(mesh->texture_ids, sizeof(int) * mesh->texture_ids_allocated);
		LG_WARN("Reallocating Texture Ids from %d to %d", mesh->texture_ids_allocated - 256, mesh->texture_ids_allocated);
	}

	int ind = 0;
	int	tex = 0;
	for (int i = 0; i < 4; i++)
	{
		ind = 3 * i;
		mesh->vertices[mesh->vertices_amount + ind + 0] = (face_vertices[ind + 0] * block_scale) + coord[0];
		mesh->vertices[mesh->vertices_amount + ind + 1] = (face_vertices[ind + 1] * block_scale) + coord[1];
		mesh->vertices[mesh->vertices_amount + ind + 2] = (face_vertices[ind + 2] * block_scale) + coord[2];

		tex = texture_id | (i << 16) | (light << 20);
		mesh->texture_ids[mesh->texture_id_amount + i] = tex;
	}

/*
	int	uv_index = (tex >> 16) & 0x0000000f;
	int	texture_index = tex & 0x0000FFFF;
	int	light_index = (tex >> 20) & 0x000000FF;
	ft_printf("uv_index %d == 3, texture_index %d == %d, ligth_index %d == 100\n", uv_index, texture_index, texture_id, light_index);
	exit(0);
	*/

	mesh->vertices_amount += 4 * 3;
	mesh->texture_id_amount += 4;

// Indices
	if (mesh->indices_allocated < mesh->indices_amount + 6)
	{
		mesh->indices_allocated += 256;
		mesh->indices = realloc(mesh->indices, sizeof(float) * mesh->indices_allocated);
		LG_WARN("Reallocating Indices from %d to %d", mesh->indices_allocated - 256, mesh->indices_allocated);
	}

	mesh->indices[mesh->indices_amount + 0] = mesh->index_amount;
	mesh->indices[mesh->indices_amount + 1] = mesh->index_amount + 1;
	mesh->indices[mesh->indices_amount + 2] = mesh->index_amount + 2;

	mesh->indices[mesh->indices_amount + 3] = mesh->index_amount;
	mesh->indices[mesh->indices_amount + 4] = mesh->index_amount + 2;
	mesh->indices[mesh->indices_amount + 5] = mesh->index_amount + 3;

	mesh->indices_amount += 6;
	mesh->index_amount += 4;
}

/*
 * If block found : we save the position of the block in 'block_pos'
 *	(which is the world position of the block) and
 *	the face, of the block the point belongs to, in 'face' and
 *	return the pointer of the block;
*/
t_block	*get_block_from_chunk_mesh(t_chunk *chunk, float *point, float *block_pos, int *face)
{
	float	p1[VEC3_SIZE];
	float	p2[VEC3_SIZE];
	float	p3[VEC3_SIZE];
	int		i = 0;

	for (; i < chunk->block_amount; i++)
	{
		if (g_block_data[chunk->blocks[i].type + 1].solid == 0)
			continue ;

		int		l_pos[3];
		float	pos[3];
		get_block_local_pos_from_index(l_pos, (int []){16, 16, 16}, i);
		pos[0] = l_pos[0] + chunk->world_coordinate[0];
		pos[1] = l_pos[1] + chunk->world_coordinate[1];
		pos[2] = l_pos[2] + chunk->world_coordinate[2];

		for (int f = 0; f < FACE_AMOUNT; f++)
		{
			p1[0] = (g_faces[f][0] * chunk->info->block_scale) + pos[0];
			p1[1] = (g_faces[f][1] * chunk->info->block_scale) + pos[1];
			p1[2] = (g_faces[f][2] * chunk->info->block_scale) + pos[2];

			p2[0] = (g_faces[f][3] * chunk->info->block_scale) + pos[0];
			p2[1] = (g_faces[f][4] * chunk->info->block_scale) + pos[1];
			p2[2] = (g_faces[f][5] * chunk->info->block_scale) + pos[2];

			p3[0] = (g_faces[f][6] * chunk->info->block_scale) + pos[0];
			p3[1] = (g_faces[f][7] * chunk->info->block_scale) + pos[1];
			p3[2] = (g_faces[f][8] * chunk->info->block_scale) + pos[2];

			if (point_in_triangle(point, p1, p2, p3))
			{
				for (int c = 0; c < 3; c++)
					block_pos[c] = pos[c];
				*face = f;
				return (&chunk->blocks[i]);
			}

			p1[0] = (g_faces[f][0] * chunk->info->block_scale) + pos[0];
			p1[1] = (g_faces[f][1] * chunk->info->block_scale) + pos[1];
			p1[2] = (g_faces[f][2] * chunk->info->block_scale) + pos[2];

			p2[0] = (g_faces[f][6] * chunk->info->block_scale) + pos[0];
			p2[1] = (g_faces[f][7] * chunk->info->block_scale) + pos[1];
			p2[2] = (g_faces[f][8] * chunk->info->block_scale) + pos[2];

			p3[0] = (g_faces[f][9] * chunk->info->block_scale) + pos[0];
			p3[1] = (g_faces[f][10] * chunk->info->block_scale) + pos[1];
			p3[2] = (g_faces[f][11] * chunk->info->block_scale) + pos[2];
			if (point_in_triangle(point, p1, p2, p3))
			{
				for (int c = 0; c < 3; c++)
					block_pos[c] = pos[c];
				*face = f;
				return (&chunk->blocks[i]);
			}
		}
	}
	return (NULL);
}

/*
 * Returns amount of collisions;
 * Result of intersection points are stored in 'intersect_point';
 * 
 * TODO: This should probably returns either an array of points being hit,
 * 	or just the closest one....?
*/
int	chunk_mesh_collision(float *orig, float *dir, t_chunk *chunk, float reach, float intersect_point[16][3])
{
	float			*vertices;
	unsigned int	*indices;
	float			p1[3];
	float			p2[3];
	float			p3[3];
	float			norm_dir[3];
	int				collisions = 0;

	vec3_normalize(norm_dir, dir);
	vertices = chunk->mesh.vertices;
	indices = chunk->mesh.indices;
	for (int i = 0; i < chunk->mesh.indices_amount / 3; i++)
	{
		int k = i * 3;
		vec3_new(p1,
			vertices[indices[k + 0] * 3 + 0],
			vertices[indices[k + 0] * 3 + 1],
			vertices[indices[k + 0] * 3 + 2]);
		vec3_new(p2,
			vertices[indices[k + 1] * 3 + 0],
			vertices[indices[k + 1] * 3 + 1],
			vertices[indices[k + 1] * 3 + 2]);
		vec3_new(p3,
			vertices[indices[k + 2] * 3 + 0],
			vertices[indices[k + 2] * 3 + 1],
			vertices[indices[k + 2] * 3 + 2]);
		
		// We have to add the chunk position to the chunk mesh, since that is 
		//		what we are doing in the shader;
		vec3_add(p1, p1, chunk->world_coordinate);
		vec3_add(p2, p2, chunk->world_coordinate);
		vec3_add(p3, p3, chunk->world_coordinate);
		if (ray_triangle_intersect(orig, dir, p1, p2, p3, intersect_point[collisions]))/* ||
			ray_line_intersect(orig, dir, p1, p2, intersect_point[collisions]) ||
			ray_line_intersect(orig, dir, p1, p3, intersect_point[collisions]) ||
			ray_line_intersect(orig, dir, p2, p3, intersect_point[collisions]))*/
			if (vec3_dist(orig, intersect_point[collisions]) <= reach + 0.001f)
				collisions += 1;
	}
	return (collisions);
}

/*
 * Same as the chunk_mesh_collision, but instead of saving the intersect_points,
 *	it saves the normals of the faces it collides with;
*/
int	chunk_mesh_collision_v2(float *orig, float *dir, t_chunk *chunk, float reach, float intersect_points[16][3], float intersect_normals[16][3])
{
	float			*vertices;
	unsigned int	*indices;
	float			p1[3];
	float			p2[3];
	float			p3[3];
	float			norm_dir[3];
	int				collisions = 0;

	vec3_normalize(norm_dir, dir);
	vertices = chunk->mesh.vertices;
	indices = chunk->mesh.indices;
	for (int i = 0; i < chunk->mesh.indices_amount / 3; i++)
	{
		int k = i * 3;
		vec3_new(p1,
			vertices[indices[k + 0] * 3 + 0],
			vertices[indices[k + 0] * 3 + 1],
			vertices[indices[k + 0] * 3 + 2]);
		vec3_new(p2,
			vertices[indices[k + 1] * 3 + 0],
			vertices[indices[k + 1] * 3 + 1],
			vertices[indices[k + 1] * 3 + 2]);
		vec3_new(p3,
			vertices[indices[k + 2] * 3 + 0],
			vertices[indices[k + 2] * 3 + 1],
			vertices[indices[k + 2] * 3 + 2]);
		
		// We have to add the chunk position to the chunk mesh, since that is 
		//		what we are doing in the shader;
		vec3_add(p1, p1, chunk->world_coordinate);
		vec3_add(p2, p2, chunk->world_coordinate);
		vec3_add(p3, p3, chunk->world_coordinate);
		if (ray_triangle_intersect(orig, dir, p1, p2, p3, intersect_points[collisions]))
		{
			if (vec3_dist(orig, intersect_points[collisions]) <= reach + EPSILON)
			{
				triangle_face_normal(intersect_normals[collisions], p1, p2, p3);
				collisions += 1;
			}
		}
	}
	return (collisions);
}

void	render_block_outline(float *pos, float *color, float *view, float *projection)
{
	// Front
	float	p1[3]; // top left
	float	p2[3]; // bot left
	float	p3[3]; // bot right
	float	p4[3]; // top right

	p1[0] = (g_faces[0][0] * 0.5) + pos[0];
	p1[1] = (g_faces[0][1] * 0.5) + pos[1];
	p1[2] = (g_faces[0][2] * 0.5) + pos[2];

	p2[0] = (g_faces[0][3] * 0.5) + pos[0];
	p2[1] = (g_faces[0][4] * 0.5) + pos[1];
	p2[2] = (g_faces[0][5] * 0.5) + pos[2];

	p3[0] = (g_faces[0][6] * 0.5) + pos[0];
	p3[1] = (g_faces[0][7] * 0.5) + pos[1];
	p3[2] = (g_faces[0][8] * 0.5) + pos[2];

	p4[0] = (g_faces[0][9] * 0.5) + pos[0];
	p4[1] = (g_faces[0][10] * 0.5) + pos[1];
	p4[2] = (g_faces[0][11] * 0.5) + pos[2];

	// Back
	float	b1[3]; // top left
	float	b2[3]; // bot left
	float	b3[3]; // bot right
	float	b4[3]; // top right

	b1[0] = (g_faces[1][0] * 0.5) + pos[0];
	b1[1] = (g_faces[1][1] * 0.5) + pos[1];
	b1[2] = (g_faces[1][2] * 0.5) + pos[2];

	b2[0] = (g_faces[1][3] * 0.5) + pos[0];
	b2[1] = (g_faces[1][4] * 0.5) + pos[1];
	b2[2] = (g_faces[1][5] * 0.5) + pos[2];

	b3[0] = (g_faces[1][6] * 0.5) + pos[0];
	b3[1] = (g_faces[1][7] * 0.5) + pos[1];
	b3[2] = (g_faces[1][8] * 0.5) + pos[2];

	b4[0] = (g_faces[1][9] * 0.5) + pos[0];
	b4[1] = (g_faces[1][10] * 0.5) + pos[1];
	b4[2] = (g_faces[1][11] * 0.5) + pos[2];

	// Front
	render_3d_line(p1, p2, color, view, projection);
	render_3d_line(p2, p3, color, view, projection);
	render_3d_line(p3, p4, color, view, projection);
	render_3d_line(p4, p1, color, view, projection);

	// Back
	render_3d_line(b1, b2, color, view, projection);
	render_3d_line(b2, b3, color, view, projection);
	render_3d_line(b3, b4, color, view, projection);
	render_3d_line(b4, b1, color, view, projection);

	// Top / Bot
	render_3d_line(p1, b4, color, view, projection);
	render_3d_line(p4, b1, color, view, projection);

	render_3d_line(p2, b3, color, view, projection);
	render_3d_line(p3, b2, color, view, projection);
}

void	something(float *res, float *pos, float *velocity, t_chunk *chunks)
{
	float	normed_velocity[3];
	float	player_intersect_normal[10][3];
	float	player_intersect_point[10][3];
	int		player_collision_amount = 0;
	float	velocity_dist = vec3_dist((float []){0, 0, 0}, velocity);
	int		player_chunk[3];

	while (velocity_dist > EPSILON)
	{
		get_chunk_pos_from_world_pos(player_chunk, pos, chunks[0].info);
		player_collision_amount = 0;
		vec3_normalize(normed_velocity, velocity);
		for (int i = 0; i < chunks[0].info->chunks_loaded; i++)
		{
			if (!(chunks[i].blocks_solid_amount > 0 &&
				vec3i_dist(player_chunk, chunks[i].coordinate) < 2))
				continue ;	
			int colls = chunk_mesh_collision_v2(pos, normed_velocity, &chunks[i], velocity_dist, player_intersect_point + player_collision_amount, player_intersect_normal + player_collision_amount);
			player_collision_amount += colls;
		}
		if (player_collision_amount <= 0)
			break ;
		for (int i = 0; i < player_collision_amount; i++)
		{
			float	destination[3];
			vec3_add(destination, pos, velocity);

			float	distance;
			distance = vec3_dist(player_intersect_point[i], destination);	
			if (distance < EPSILON)
			{
				vec3_new(res, 0, 0, 0);
				break ;
			}

			float	new_destination[3];
			vec3_multiply_f(new_destination, player_intersect_normal[i], distance);
			vec3_sub(new_destination, destination, new_destination);

			float	tmp[3];
			vec3_sub(tmp, destination, new_destination);
			vec3_add(res, res, tmp);

			vec3_string("destination :", destination);
			ft_printf("distance : %f\n", distance);
			vec3_string("new_destination : ", new_destination);
		}
		velocity_dist = vec3_dist((float []){0, 0, 0}, res);
	}
}