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
	if (!type || type != GAS_AIR)
		return ;
	set_block_at_world_pos(info, world_pos, FLUID_WATER + nth_from_source);

	// All directions to the sides
	water_placer(info, (float []){world_pos[0] + 1.0f, world_pos[1], world_pos[2]}, nth_from_source + 1);
	water_placer(info, (float []){world_pos[0] - 1.0f, world_pos[1], world_pos[2]}, nth_from_source + 1);
	water_placer(info, (float []){world_pos[0], world_pos[1], world_pos[2] + 1.0f}, nth_from_source + 1);
	water_placer(info, (float []){world_pos[0], world_pos[1], world_pos[2] - 1.0f}, nth_from_source + 1);

	// Down
	water_placer(info, (float []){world_pos[0], world_pos[1] - 1, world_pos[2]}, 0);
}