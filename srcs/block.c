#include "vox.h"

/*
 * Get block info from world position;
 * 
 * Return : the same 't_block_info' you gave in as argument;
 * 
 * 'res' : the pointer to where we store the block info;
 * 'info' : the chunk info that we use to help finding correct info for block;
 * 'pos' : world position of the wanted block;
*/
t_block_info	*get_block_info_pos(t_block_info *res, t_world *info, float *pos)
{
	int		chunk_local_pos[3];

	// Block local pos;
	res->local[0] = mod(pos[0], CHUNK_WIDTH);
	res->local[1] = mod(pos[1], CHUNK_HEIGHT);
	res->local[2] = mod(pos[2], CHUNK_BREADTH);

	// Block world pos;
	res->pos[0] = pos[0];
	res->pos[1] = pos[1];
	res->pos[2] = pos[2];

	// Block index;
	res->index = (local_pos[0] * CHUNK_WIDTH * CHUNK_BREADTH) + (local_pos[2] * CHUNK_HEIGHT) + local_pos[1];

	// Chunk local pos;
	chunk_local_pos[0] = floor(pos[0] / CHUNK_SIZE_X);
	chunk_local_pos[1] = floor(pos[1] / CHUNK_SIZE_Y);
	chunk_local_pos[2] = floor(pos[2] / CHUNK_SIZE_Z);
	// Chunk from info, with local pos;
	for (int i = 0; i < CHUNKS_LOADED; i++)
	{
		if (info->chunks[i].coordinate[0] == chunk_local_pos[0] && 
			info->chunks[i].coordinate[1] == chunk_local_pos[1] && 
			info->chunks[i].coordinate[2] == chunk_local_pos[2])
			res->chunk = &info->chunks[i];
	}

	// Block from chunk, with index;
	res->block = &res->chunk->blocks[res->index];

	return (res);
}