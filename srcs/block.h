#ifndef BLOCK_H
# define BLOCK_H

enum e_block_type
{
	BLOCK_AIR = -1,
	BLOCK_DIRT = 0,
	BLOCK_STONE = 1,
	BLOCK_BEDROCK = 2,
	BLOCK_WATER = 3,
	BLOCK_TYPE_AMOUNT
};

// Every block has a block_data corresponding with its type;
typedef struct s_block_data
{
	char	type; // 'e_block_type', at some point maybe same array index in 'g_block_data';
	char	solid; // if solid or not;
	char	front_texture; // texture id from the texture atlas;
	char	left_texture;
	char	back_texture;
	char	right_texture;
	char	top_texture;
	char	bot_texture;
}	t_block_data;

static const t_block_data g_block_data[] = {
	{
		BLOCK_AIR, 0,
		-1, -1, -1, -1, -1, -1
	},
	{
		BLOCK_DIRT, 1,
		1, 1, 1, 1, 0, 2 
	},
	{
		BLOCK_STONE, 1,
		3, 3, 3, 3, 3, 3 
	},
	{
		BLOCK_BEDROCK, 1,
		6, 6, 6, 6, 6, 6 
	},
	{
		BLOCK_WATER, 0,
		9, 9, 9, 9, 9, 9 
	}
};

static const float g_front_face[] = {-1, 1, 1,  -1, -1, 1,  1, -1, 1,  1, 1, 1};
static const float g_back_face[] = {-1, 1, -1,  -1, -1, -1,  1, -1, -1,  1, 1, -1};
static const float g_left_face[] = {-1, 1, -1,  -1, -1, -1,  -1, -1, 1,  -1, 1, 1};
static const float g_right_face[] = {1, 1, -1,  1, -1, -1,  1, -1, 1,  1, 1, 1};
static const float g_top_face[] = {-1, 1, -1,  -1, 1, 1,  1, 1, 1,  1, 1, -1};
static const float g_bot_face[] = {-1, -1, -1,  -1, -1, 1,  -1, 1, 1,  -1, 1, -1};

#endif