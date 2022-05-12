#include "shaderpixel.h"

// Is BLOCK

int	is_gas(t_block *block)
{
	return (block && is_type_gas(block->type));
}

int	is_solid(t_block *block)
{
	return (block && is_type_solid(block->type));
}

int	is_flora(t_block *block)
{
	return (block && is_type_flora(block->type));
}

int	is_fluid(t_block *block)
{
	return (block && is_type_fluid(block->type));
}

int	is_solid_alpha(t_block *block)
{
	return (block && is_type_solid_alpha(block->type));
}

// Is Type

int	is_type_gas(int type)
{
	return (type > GAS_FIRST && type < GAS_LAST);
}

int	is_type_solid(int type)
{
	return (type > BLOCK_FIRST && type < BLOCK_LAST);
}

int	is_type_flora(int type)
{
	return (type > FLORA_FIRST && type < FLORA_LAST);
}

int	is_type_fluid(int type)
{
	return (type > FLUID_FIRST && type < FLUID_LAST);
}

int	is_type_solid_alpha(int type)
{
	return (type > BLOCK_ALPHA_FIRST && type < BLOCK_ALPHA_LAST);
}

// This type of 'whatever' doesnt have a block, its only an item;
int	is_type_item(int type)
{
	return (type > ITEM_FIRST && type < ITEM_LAST);
}

t_block_data	get_block_data(t_block *block)
{
	if (is_gas(block))
		return (g_gas_data[block->type - GAS_FIRST - 1]);
	if (is_solid(block))
		return (g_block_data[block->type - BLOCK_FIRST - 1]);
	if (is_solid_alpha(block))
		return (g_block_alpha_data[block->type - BLOCK_ALPHA_FIRST - 1]);
	if (is_flora(block))
		return (g_flora_data[block->type - FLORA_FIRST - 1]);
	if (is_fluid(block))
		return (g_fluid_data[block->type - FLUID_FIRST - 1]);
	return (g_gas_data[0]);
}

/*
 * Returns 1 if the water block could be placed;
*/
int	water_placer(t_chunk_info *info, float *world_pos, int nth_from_source)
{
	int	type;
	
	// Water can only spread 7 blocks from source;
	if (nth_from_source > 7)
		return (0);

	// Check that position is not occupied;
	type = get_block_type_at_world_pos(info, world_pos);
	// If block is not solid, we can proceed;
	// If block is smaller type of water block we can place this block there;
	if (is_type_solid(type) || is_type_solid_alpha(type))
		return (0);
	if  (is_type_fluid(type) && type < FLUID_WATER + nth_from_source)
		return (0);

	// Set water block
	set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);

	// If we dont have block under this, we place source block;
	float	under[3];

	vec3_new(under, world_pos[0], world_pos[1] - 1.0f, world_pos[2]);
	type = get_block_type_at_world_pos(info, under);
	if (!is_type_solid(type) && !is_type_solid_alpha(type))
	{
		water_placer(info, under, 0);
		return (0);
	}

	// Go through all directions and place water on the adjacent blocks;
	float	tmp[3];

	for (int j = DIR_NORTH; j <= DIR_WEST; j++)
	{
		vec3_add(tmp, world_pos, (float *)g_card_dir[j]);
		water_placer(info, tmp, 1 + nth_from_source);
	}
	return (1);
}

/*
 * Return direction if found (from e_card_dir);
 *
 * 'pos' : block world position;
*/
int	find_shortest_down(t_chunk_info *info, float *pos, int curr_len, int max_len)
{
	if (curr_len > max_len)
		return (-1);
	
	// Go through all directions and search if it goes down in that dir;
	float	tmp[3];
	// If we have gas block under this, we have found down;
	int		type;

	// Check first instantly down;
	vec3_add(tmp, pos, (float *)g_card_dir[DIR_DOWN]);
	type = get_block_type_at_world_pos(info, tmp);
	if (is_type_gas(type) || is_type_flora(type) || is_type_fluid(type))
		return (DIR_DOWN);

	for (int j = DIR_NORTH; j <= DIR_WEST; j++)
	{
		vec3_add(tmp, pos, (float *)g_card_dir[j]);
		vec3_add(tmp, tmp, (float *)g_card_dir[DIR_DOWN]);
		type = get_block_type_at_world_pos(info, tmp);
		if (is_type_gas(type) || is_type_flora(type) || is_type_fluid(type))
			return (j);

		vec3_add(tmp, pos, (float *)g_card_dir[j]);
		int res = find_shortest_down(info, tmp, curr_len + 1, max_len);
		if (res != -1)
			return (res);
	}

	return (-1);
}

int	water_placer_v2(t_chunk_info *info, float *world_pos, int nth_from_source)
{
	int		shortest_dir;
	int		type;
	float	tmp[3];

	if (nth_from_source > 7)
		return (0);

	// Check that position is not occupied;
	type = get_block_type_at_world_pos(info, world_pos);
	// If block is not solid, we can proceed;
	// If block is smaller type of water block we can place this block there;
	if (is_type_solid(type) || is_type_solid_alpha(type))
		return (0);
	if (is_type_fluid(type) && type < FLUID_WATER + nth_from_source)
		return (0);

	t_chunk	*chunk;

	// Find shortest dir; up to 3 blocks;
	shortest_dir = -1;
	int	i = 0;
	while (shortest_dir == -1 && i <= 3)
	{
		shortest_dir = find_shortest_down(info, world_pos, 0, i);
		i++;
	}

	if (shortest_dir == -1) // if we havent found down block, we spread water around;
	{
		for (int j = DIR_NORTH; j <= DIR_WEST; j++)
		{
			vec3_add(tmp, world_pos, (float *)g_card_dir[j]);
			set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);
			water_placer_v2(info, tmp, nth_from_source + 1);
		}
	}
	else // go in the direction of the shortest down block;
	{
		vec3_add(tmp, world_pos, (float *)g_card_dir[shortest_dir]);
		set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);
		if (shortest_dir == DIR_DOWN) // if dir is down we place source block there and it can spread up to 8 blocks again;
			water_placer_v2(info, tmp, 0);
		else
			water_placer_v2(info, tmp, nth_from_source + 1);
	}

	return (1);
}

/*
 * 'verts' == float[12];
 * 'face' == e_card_dir;
 * 'block_world' : the world position of the block we want the vertices of;
*/
void	flowing_water_verts(float *verts, int face, t_block *block, float *block_world, t_chunk_info *info)
{
	float	tmp[3];
	int		type;

	vec3_new(verts + 0, g_faces[face][0], g_faces[face][1], g_faces[face][2]);
	vec3_new(verts + 3, g_faces[face][3], g_faces[face][4], g_faces[face][5]);
	vec3_new(verts + 6, g_faces[face][6], g_faces[face][7], g_faces[face][8]);
	vec3_new(verts + 9, g_faces[face][9], g_faces[face][10], g_faces[face][11]);

	// A source block will always be a full block;
	if (block->type != FLUID_WATER)
	{
		int	most;
		float final;

		if (face == DIR_UP || face == DIR_NORTH || face == DIR_WEST)
		{
			// North west

			// Default the height of the block type;
			most = block->type;

			// Check the highest vertex in the direction of whichever card dir the vertex is in;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTHWEST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTH]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_WEST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			
			// If theres a gas block, we want the lowest position for that vertex;
			if (is_type_gas(most))
				most = FLUID_WATER_7;

			final = 1.0f - ((most - FLUID_WATER) * 0.25f);
			if (face == DIR_UP)
				verts[1] *= final;
			if (face == DIR_WEST)
				verts[1] *= final;
			if (face == DIR_NORTH)
				verts[10] *= final;
		}

		if (face == DIR_UP || face == DIR_SOUTH || face == DIR_WEST)
		{
			// South west
			most = block->type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTHWEST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTH]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_WEST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;

			if (is_type_gas(most))
				most = FLUID_WATER_7;

			final = 1.0f - ((most - FLUID_WATER) * 0.25f);
			if (face == DIR_UP)
				verts[4] *= final;
			if (face == DIR_SOUTH)
				verts[1] *= final;
			if (face == DIR_WEST)
				verts[10] *= final;
		}

		if (face == DIR_UP || face == DIR_SOUTH || face == DIR_EAST)
		{
			// South East
			most = block->type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTHEAST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTH]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_EAST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;

			if (is_type_gas(most))
				most = FLUID_WATER_7;
			
			final = 1.0f - ((most - FLUID_WATER) * 0.25f);
			if (face == DIR_UP)
				verts[7] *= final;
			if (face == DIR_EAST)
				verts[1] *= final;
			if (face == DIR_SOUTH)
				verts[10] *= final;
		}

		if (face == DIR_UP || face == DIR_NORTH || face == DIR_EAST)
		{
			// North East
			most = block->type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTHEAST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTH]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_EAST]));
			if (is_type_fluid(type) && type < most)
				if (type < most)
					most = type;

			if (is_type_gas(most))
				most = FLUID_WATER_7;
			
			final = 1.0f - ((most - FLUID_WATER) * 0.25f);
			if (face == DIR_UP)
				verts[10] *= final;
			if (face == DIR_NORTH)
				verts[1] *= final;
			if (face == DIR_EAST)
				verts[10] *= final;
		}
	}
}