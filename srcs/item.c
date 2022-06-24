/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   item.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 14:13:30 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/24 12:20:32 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"
#include "chunk.h"

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

int	is_water(t_block *block)
{
	return (block && block->type >= FLUID_WATER && block->type <= FLUID_WATER_7);
}

t_block_data	*get_block_data_from_type(int type)
{
	return (&g_block_data[type]);
}

t_block_data	*get_block_data(t_block *block)
{
	return (get_block_data_from_type(block->type));
}

/*
 *  Returns 1 if chunk has non solid blocks;
*/
int	chunk_has_non_solid(t_chunk *chunk)
{
	if (!chunk)
		return (0);
	if (chunk->block_palette[GAS_AIR] > 0)
		return (1);
	if (chunk->blocks_flora_amount > 0)
		return (1);
	if (chunk->blocks_fluid_amount > 0)
		return (1);
	return (0);
}

/*
 * Returns 1 if the water block could be placed;
*/
int	water_placer(t_world *info, float *world_pos, int nth_from_source)
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

	v3_new(under, world_pos[0], world_pos[1] - 1.0f, world_pos[2]);
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
		v3_add(tmp, world_pos, (float *)g_card_dir[j]);
		water_placer(info, tmp, 1 + nth_from_source);
	}
	return (1);
}

/*
 * Return direction if found (from e_card_dir); -1 if not found;
 *
 * 'pos' : block world position;
*/
int	find_shortest_down(t_world *info, float *pos, int curr_len, int max_len)
{
	if (curr_len > max_len)
		return (-1);
	
	// Go through all directions and search if it goes down in that dir;
	float	tmp[3];
	// If we have gas block under this, we have found down;
	int		type;

	// Check first instantly down;
	v3_add(tmp, pos, (float *)g_card_dir[DIR_DOWN]);
	type = get_block_type_at_world_pos(info, tmp);
	if (!is_type_solid(type) && !is_type_solid_alpha(type))
		return (DIR_DOWN);

	for (int j = DIR_NORTH; j <= DIR_WEST; j++)
	{
		// IF there is solid block in that dir, we wont go further;
		v3_add(tmp, pos, (float *)g_card_dir[j]);
		type = get_block_type_at_world_pos(info, tmp);
		if (is_type_solid(type) || is_type_solid_alpha(type))
			continue ;

		v3_add(tmp, tmp, (float *)g_card_dir[DIR_DOWN]);
		type = get_block_type_at_world_pos(info, tmp);
		if (!is_type_solid(type) && !is_type_solid_alpha(type))
			return (j);

		v3_add(tmp, pos, (float *)g_card_dir[j]);
		int res = find_shortest_down(info, tmp, curr_len + 1, max_len);
		if (res != -1)
			return (res);
	}

	return (-1);
}

/*
 * @brief Get water block from world pos, if exists; 
 * 
 * @param info 
 * @param pos 
 * @return t_block_water* 
*/
t_block_event	*get_water_block(t_world *info, float *pos)
{
	int		chunk_pos[3];
	t_chunk	*chunk;

	get_chunk_pos_from_world_pos(chunk_pos, pos);
	chunk = get_chunk(info, chunk_pos);
	if (chunk)
	{
		for (int i = 0; i < chunk->event_block_amount; i++)
			if (is_water(chunk->event_blocks[i].block) &&
				chunk->event_blocks[i].pos[0] == pos[0] &&
				chunk->event_blocks[i].pos[1] == pos[1] &&
				chunk->event_blocks[i].pos[2] == pos[2])
				return (&chunk->event_blocks[i]);
	}
	return (NULL);
}

void	water_flow(t_world *info, t_block_event *water)
{
	t_block			*neighbor[6]; // neswud
	t_block_event	*neighbor_water[6];
	float			tmp[3];
	int				type;

	// If flow dist is more than 7, the water has traveled the max distance;
	if (water->statique)
		return ;
	if (water->block->type >= FLUID_WATER_7)
	{
		v3_add(tmp, water->pos, (float *)g_card_dir[DIR_DOWN]);
		neighbor[0] = get_block(info, tmp);
		if ((!is_solid(neighbor[0]) && !is_solid_alpha(neighbor[0])) &&
			(!is_fluid(neighbor[0]) || (is_fluid(neighbor[0]) && neighbor[0]->type > FLUID_WATER)))
			set_block_at_world_pos(info, tmp, FLUID_WATER);
		return ;
	}
	
	// For all directions, get neighboring blocks;
	for (int d = DIR_NORTH, i = 0; d <= DIR_DOWN; d++, i++)
	{
		v3_add(tmp, water->pos, (float *)g_card_dir[d]);
		neighbor[i] = get_block(info, tmp);
		neighbor_water[i] = get_water_block(info, tmp);// TODO: maybe do this right before we actually use it, so we dont do this without point (onödan);
	}

	// Try to find flow dir, replace with old one, if shorter found;
	int	dir = -1;
	water->flow_dir = -1;
	water->dist_to_down = INT_MAX;
	for (int i = 0; i <= 3; i++)
	{
		dir = find_shortest_down(info, water->pos, 0, i);
		if (dir != -1 && i < water->dist_to_down)
		{
			water->flow_dir = dir;
			water->dist_to_down = i;
			break ;
		}
	}

	// Check neighbor 'dist_to_down', if its shorter than our own we take it;
	// Dont look up;
	// TODO: dont look in direction of where we came from;
	for (int i = 0; i < 4; i++)
	{
		if (is_fluid(neighbor[i]) && neighbor_water[i] && neighbor_water[i]->dist_to_down < water->dist_to_down)
		{
			water->dist_to_down = neighbor_water[i]->dist_to_down;
			water->flow_dir = neighbor_water[i]->flow_dir;
		}
	}

	// Flow
	// If doesnt have flow_dir, we spread water to all directions;
	if (water->flow_dir == -1)
	{
		v3_add(tmp, water->pos, (float *)g_card_dir[DIR_NORTH]);
		if ((!is_solid(neighbor[0]) && !is_solid_alpha(neighbor[0])) &&
			(!is_fluid(neighbor[0]) || (is_fluid(neighbor[0]) && neighbor[0]->type > water->block->type + 1)))
			set_block_at_world_pos(info, tmp, water->block->type + 1);

		v3_add(tmp, water->pos, (float *)g_card_dir[DIR_EAST]);
		if ((!is_solid(neighbor[1]) && !is_solid_alpha(neighbor[1])) &&
			(!is_fluid(neighbor[1]) || (is_fluid(neighbor[1]) && neighbor[1]->type > water->block->type + 1)))
			set_block_at_world_pos(info, tmp, water->block->type + 1);

		v3_add(tmp, water->pos, (float *)g_card_dir[DIR_SOUTH]);
		if ((!is_solid(neighbor[2]) && !is_solid_alpha(neighbor[2])) &&
			(!is_fluid(neighbor[2]) || (is_fluid(neighbor[2]) && neighbor[2]->type > water->block->type + 1)))
			set_block_at_world_pos(info, tmp, water->block->type + 1);

		v3_add(tmp, water->pos, (float *)g_card_dir[DIR_WEST]);
		if ((!is_solid(neighbor[3]) && !is_solid_alpha(neighbor[3])) &&
			(!is_fluid(neighbor[3]) || (is_fluid(neighbor[3]) && neighbor[3]->type > water->block->type + 1)))
			set_block_at_world_pos(info, tmp, water->block->type + 1);

		// DOWN
		v3_add(tmp, water->pos, (float *)g_card_dir[DIR_DOWN]);
		if ((!is_solid(neighbor[5]) && !is_solid_alpha(neighbor[5])) &&
			(is_fluid(neighbor[5]) && neighbor[5]->type > water->block->type + 1))
			set_block_at_world_pos(info, tmp, FLUID_WATER);
	}
	else // Flow in our own direction;
	{
		t_block	*block;

		v3_add(tmp, water->pos, (float *)g_card_dir[water->flow_dir]);
		block = get_block(info, tmp);
		if ((!is_solid(block) && !is_solid_alpha(block)) &&
			(!is_fluid(block) || (is_fluid(block) && block->type > water->block->type + 1)))
		{
			if (water->flow_dir == DIR_DOWN)
				set_block_at_world_pos(info, tmp, FLUID_WATER);
			else
				set_block_at_world_pos(info, tmp, water->block->type + 1);
		}
	}
}

/*
 * Returns whether water was removed;
*/
int	water_remove(t_world *info, t_block_event *water)
{
	t_block			*neighbor[6]; // neswud
	t_chunk			*neighbor_chunk[6];
	float			tmp[3];
	int				type;

	// For all directions, get neighboring blocks;
	for (int d = DIR_NORTH, i = 0; d <= DIR_DOWN; d++, i++)
	{
		v3_add(tmp, water->pos, (float *)g_card_dir[d]);
		neighbor[i] = get_block(info, tmp);
		neighbor_chunk[i] = get_chunk_from_world_pos(info, tmp);
	}

	// Only if not source block
	if (water->block->type != FLUID_WATER)
	{
		// If none of the neighboring blocks are larger water block than this, we remove it;
		int	bigger_found = 0;
		for (int i = 0; i < 4; i++)
		{
			if (is_fluid(neighbor[i]) &&
				neighbor[i]->type <= water->block->type)
			{
				bigger_found = 1;
				break ;
			}
		}
		if (!bigger_found)
		{
			set_block_at_world_pos(info, water->pos, GAS_AIR);
			for (int i = 0; i < 6; i++)
				if (neighbor_chunk[i])
					neighbor_chunk[i]->update_water = 1;
			get_chunk_from_world_pos(info, water->pos)->update_water = 1;
			water->dist_to_down = INT_MAX;
			water->flow_dir = -1;
			water->block = NULL;
			ft_printf("water block should be removed\n");
			return (1); // gtfo from function since this is not water block anymore;
		}
	}
	return (0);
}

// TODO: Remove this funct
void	chunk_water_remover(t_world *info, t_chunk *chunk)
{
	// For all water blocks in chunk;
	for (int j = 0; j < chunk->event_block_amount; j++)
		if (is_water(chunk->event_blocks[j].block))
			water_remove(info, &chunk->event_blocks[j]);
}

void	water_vertex_height_decider(t_world *info, t_block *block, float *block_world, float *verts, int face, int faces[3], int dirs[3], int v[3])
{
	int		fluid_found = 0;
	int		most = block->type;
	int		type = 0;
	float	tmp[3];
	float	final;

	for (int d = 0; d < 3; d++)
	{
		type = get_block_type_at_world_pos(info, v3_add(tmp, block_world, (float *)g_card_dir[dirs[d]]));
		if (is_type_fluid(type))
		{
			fluid_found = 1;
			if (type < most)
				most = type;
		}
	}

	if (!fluid_found)
		most = FLUID_WATER_7;

	final = 1.0f - ((most - FLUID_WATER) * 0.25f);
	if (face == faces[0])
		verts[v[0]] *= final;
	if (face == faces[1])
		verts[v[1]] *= final;
	if (face == faces[2])
		verts[v[2]] *= final;
}

/*
 * 'verts' == float[12];
 * 'face' == e_card_dir;
 * 'block_world' : the world position of the block we want the vertices of;
*/
void	flowing_water_verts(float *verts, int face, t_block *block, float *block_world, t_world *info)
{
	int	faces[3];
	int	dirs[3];
	int	v[3];

	v3_new(verts + 0, g_all_faces[BLOCK_FACES][face][0], g_all_faces[BLOCK_FACES][face][1], g_all_faces[BLOCK_FACES][face][2]);
	v3_new(verts + 3, g_all_faces[BLOCK_FACES][face][3], g_all_faces[BLOCK_FACES][face][4], g_all_faces[BLOCK_FACES][face][5]);
	v3_new(verts + 6, g_all_faces[BLOCK_FACES][face][6], g_all_faces[BLOCK_FACES][face][7], g_all_faces[BLOCK_FACES][face][8]);
	v3_new(verts + 9, g_all_faces[BLOCK_FACES][face][9], g_all_faces[BLOCK_FACES][face][10], g_all_faces[BLOCK_FACES][face][11]);

	// A source block will always be a full block;
	if (block->type == FLUID_WATER)
		return ;
	
	if (face == DIR_UP || face == DIR_NORTH || face == DIR_WEST)
	{
		faces[0] = DIR_UP;
		faces[1] = DIR_WEST;
		faces[2] = DIR_NORTH;
		dirs[0] = DIR_NORTHWEST;
		dirs[1] = DIR_NORTH;
		dirs[2] = DIR_WEST;
		v[0] = 1;
		v[1] = 1;
		v[2] = 10;
		water_vertex_height_decider(info, block, block_world, verts, face, faces, dirs, v);
	}
	if (face == DIR_UP || face == DIR_SOUTH || face == DIR_WEST)
	{
		faces[0] = DIR_UP;
		faces[1] = DIR_SOUTH;
		faces[2] = DIR_WEST;
		dirs[0] = DIR_SOUTHWEST;
		dirs[1] = DIR_SOUTH;
		dirs[2] = DIR_WEST;
		v[0] = 4;
		v[1] = 1;
		v[2] = 10;
		water_vertex_height_decider(info, block, block_world, verts, face, faces, dirs, v);
	}
	if (face == DIR_UP || face == DIR_SOUTH || face == DIR_EAST)
	{
		faces[0] = DIR_UP;
		faces[1] = DIR_EAST;
		faces[2] = DIR_SOUTH;
		dirs[0] = DIR_SOUTHEAST;
		dirs[1] = DIR_SOUTH;
		dirs[2] = DIR_EAST;
		v[0] = 7;
		v[1] = 1;
		v[2] = 10;
		water_vertex_height_decider(info, block, block_world, verts, face, faces, dirs, v);
	}
	if (face == DIR_UP || face == DIR_NORTH || face == DIR_EAST)
	{
		faces[0] = DIR_UP;
		faces[1] = DIR_NORTH;
		faces[2] = DIR_EAST;
		dirs[0] = DIR_NORTHEAST;
		dirs[1] = DIR_NORTH;
		dirs[2] = DIR_EAST;
		v[0] = 10;
		v[1] = 1;
		v[2] = 10;
		water_vertex_height_decider(info, block, block_world, verts, face, faces, dirs, v);
	}
}