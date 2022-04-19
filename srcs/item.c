#include "shaderpixel.h"

int	is_gas(t_block *block)
{
	if (block->type > GAS_FIRST && block->type < GAS_LAST)
		return (1);
	return (0);
}

int	is_solid(t_block *block)
{
	if (block->type > BLOCK_FIRST && block->type < BLOCK_LAST)
		return (1);
	return (0);
}

int	is_flora(t_block *block)
{
	if (block->type > FLORA_FIRST && block->type < FLORA_LAST)
		return (1);
	return (0);
}

int	is_fluid(t_block *block)
{
	if (block->type > FLUID_FIRST && block->type < FLUID_LAST)
		return (1);
	return (0);
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
	if (!(type == GAS_AIR ||
		(type > FLUID_WATER + nth_from_source && type < FLUID_WATER_7 + 1) ||
		(type > FLORA_FIRST && type < FLORA_LAST)))
		return (0);
	
	// Check that we have a block under this block;
	type = get_block_type_at_world_pos(info, (float []) {world_pos[0], world_pos[1] - 1.0f, world_pos[2]});
	if (type <= BLOCK_FIRST || type >= BLOCK_LAST)
	{
		set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);
		water_placer(info, (float []){world_pos[0], world_pos[1] - 1.0f, world_pos[2]}, 0);
		return (0);
	}

	// Set water block
	set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);

	float	tmp[3];
	float	dir_mult[3];

	for (int j = 0; j < DIR_AMOUNT; j++)
	{
		if (j == DIR_UP || j == DIR_DOWN)
			continue ;
		for (int i = 1; i < 8 - nth_from_source; i++)
		{
			vec3_multiply_f(dir_mult, (float *)g_card_dir[j], i);
			vec3_add(tmp, world_pos, dir_mult);
			if (!water_placer(info, tmp, i + nth_from_source))
				break ;
		}
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
			most = block->type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTH]), (float *)g_card_dir[DIR_WEST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTH]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_WEST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;

			if (most > GAS_FIRST && most < GAS_LAST)
				most = FLUID_WATER_7;

			final = 1.0f - ((g_fluid_data[most - FLUID_FIRST - 1].dist_from_source) * 0.25f);
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
			type = get_block_type_at_world_pos(info, vec3_add(tmp, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTH]), (float *)g_card_dir[DIR_WEST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTH]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_WEST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;

			if (most > GAS_FIRST && most < GAS_LAST)
				most = FLUID_WATER_7;

			final = 1.0f - ((g_fluid_data[most - FLUID_FIRST - 1].dist_from_source) * 0.25f);
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
			type = get_block_type_at_world_pos(info, vec3_add(tmp, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTH]), (float *)g_card_dir[DIR_EAST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_SOUTH]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_EAST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;

			if (most > GAS_FIRST && most < GAS_LAST)
				most = FLUID_WATER_7;
			
			final = 1.0f - ((g_fluid_data[most - FLUID_FIRST - 1].dist_from_source) * 0.25f);
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
			type = get_block_type_at_world_pos(info, vec3_add(tmp, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTH]), (float *)g_card_dir[DIR_EAST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_NORTH]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;
			type = get_block_type_at_world_pos(info, vec3_add(tmp, block_world, (float *)g_card_dir[DIR_EAST]));
			if (type > FLUID_FIRST && type < FLUID_LAST && type < most)
				if (type < most)
					most = type;

			if (most > GAS_FIRST && most < GAS_LAST)
				most = FLUID_WATER_7;
			
			final = 1.0f - ((g_fluid_data[most - FLUID_FIRST - 1].dist_from_source) * 0.25f);
			if (face == DIR_UP)
				verts[10] *= final;
			if (face == DIR_NORTH)
				verts[1] *= final;
			if (face == DIR_EAST)
				verts[10] *= final;
		}
	}
}