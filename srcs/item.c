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

void	water_placer(t_chunk_info *info, float *world_pos, int nth_from_source)
{
	int	type;
	
	// Water can only spread 7 blocks from source;
	if (nth_from_source > 7)
		return ;

	// Check that position is not occupied;
	type = get_block_type_at_world_pos(info, world_pos);
	if (!(type == GAS_AIR || (type > FLUID_WATER + nth_from_source && type < FLUID_WATER_7 + 1)))
		return ;
	
	// Check that we have a block under this block;
	type = get_block_type_at_world_pos(info, (float []) {world_pos[0], world_pos[1] - 1.0f, world_pos[2]});
	if (type < BLOCK_FIRST || type > BLOCK_LAST)
	{
		water_placer(info, (float []) {world_pos[0], world_pos[1] - 1.0f, world_pos[2]}, 0);
		return ;
	}

	// Set water block
	set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);

	float	tmp[3];
	float	dir_mult[3];

	for (int i = 1; i < 8 - nth_from_source; i++)
	{
		for (int j = 0; j < DIR_AMOUNT; j++)
		{
			if (j == DIR_UP)
				continue ;
			vec3_multiply_f(dir_mult, (float *)g_card_dir[j], i);
			vec3_add(tmp, world_pos, dir_mult);
			if (j == DIR_DOWN) // reset nth_from_source
				water_placer(info, tmp, 0);
			else
				water_placer(info, tmp, i + nth_from_source);
		}
	}
}