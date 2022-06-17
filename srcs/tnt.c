#include "shaderpixel.h"
#include "chunk.h"

void	explode(t_world *info, float pos[3], float intensity)
{
	float			tmp[3];
	t_block			*block;
	t_block_data	data;

	block = get_block(info, pos);
	if (!block) // CRASH : if the tnt is trying to explode something that doesnt exist;
		return ;
	data = get_block_data(block);
	intensity -= (0.225f * 3);
	if (!is_gas(block))
		intensity -= (data.blast_resistance + 0.3f) * 0.3f;
	if (intensity <= 0.0f)
		return ;

	set_block_at_world_pos(info, pos, GAS_AIR);

	v3_new(tmp, pos[0] - 1, pos[1], pos[2]);
	explode(info, tmp, intensity);
		
	v3_new(tmp, pos[0] + 1, pos[1], pos[2]);
	explode(info, tmp, intensity);

	v3_new(tmp, pos[0], pos[1] - 1, pos[2]);
	explode(info, tmp, intensity);

	v3_new(tmp, pos[0], pos[1] + 1, pos[2]);
	explode(info, tmp, intensity);

	v3_new(tmp, pos[0], pos[1], pos[2] - 1);
	explode(info, tmp, intensity);

	v3_new(tmp, pos[0], pos[1], pos[2] + 1);
	explode(info, tmp, intensity);
}

void	tnt_explosion(t_world *info, t_chunk *chunk, t_block_event *event_block)
{
	float	intensity = 0;
	float	explosion_str = 4.0f;
	float	variance = 0.3f; // should be random (0.0f - 0.6f);

    variance = ((float)rand()/(float)(RAND_MAX)) * 0.6f;
	intensity = (0.7f + variance) * explosion_str;
	ft_printf("TNT starting intensity : %f (%f)\n", intensity, variance);
	explode(info, event_block->pos, intensity);
}