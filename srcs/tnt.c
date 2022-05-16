#include "shaderpixel.h"

void	explode(t_chunk_info *info, float pos[3], int curr_len, int max_len)
{
	float	tmp[3];

	if (curr_len >= max_len)
		return ;
	
	vec3_new(tmp, pos[0] - 1, pos[1], pos[2]);
	set_block_at_world_pos(info, tmp, GAS_AIR);
	explode(info, tmp, curr_len + 1, max_len);

	vec3_new(tmp, pos[0] + 1, pos[1], pos[2]);
	set_block_at_world_pos(info, tmp, GAS_AIR);
	explode(info, tmp, curr_len + 1, max_len);

	vec3_new(tmp, pos[0], pos[1] - 1, pos[2]);
	set_block_at_world_pos(info, tmp, GAS_AIR);
	explode(info, tmp, curr_len + 1, max_len);

	vec3_new(tmp, pos[0], pos[1] + 1, pos[2]);
	set_block_at_world_pos(info, tmp, GAS_AIR);
	explode(info, tmp, curr_len + 1, max_len);

	vec3_new(tmp, pos[0], pos[1], pos[2] - 1);
	set_block_at_world_pos(info, tmp, GAS_AIR);
	explode(info, tmp, curr_len + 1, max_len);

	vec3_new(tmp, pos[0], pos[1], pos[2] + 1);
	set_block_at_world_pos(info, tmp, GAS_AIR);
	explode(info, tmp, curr_len + 1, max_len);
}

void	tnt_explosion(t_chunk_info *info, t_chunk *chunk, t_block_event *event_block)
{
	explode(info, event_block->pos, 0, 4);
}