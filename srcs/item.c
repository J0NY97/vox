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