#ifndef BLOCK_H
# define BLOCK_H

enum e_block_type
{
	BLOCK_AIR = -1,
	BLOCK_DIRT = 0,
	BLOCK_STONE = 1,
	BLOCK_BEDROCK = 2,
	BLOCK_WATER = 3,
	BLOCK_SAND = 4,
	BLOCK_TYPE_AMOUNT
};

// Every block has a block_data corresponding with its type;
typedef struct s_block_data
{
	char	type; // 'e_block_type', at some point maybe same array index in 'g_block_data';
	char	solid; // if solid or not;
	unsigned short	front_texture; // texture id from the texture atlas;
	unsigned short	left_texture;
	unsigned short	back_texture;
	unsigned short	right_texture;
	unsigned short	top_texture;
	unsigned short	bot_texture;
}	t_block_data;

static const t_block_data g_block_data[] = {
	{
		BLOCK_AIR, 0,
		-1, -1, -1, -1, -1, -1
	},
	{
		BLOCK_DIRT, 1,
		132, 132, 132, 132, 135, 224 
	},
	{
		BLOCK_STONE, 1,
		164, 164, 164, 164, 164, 164 
	},
	{
		BLOCK_BEDROCK, 1,
		6, 6, 6, 6, 6, 6 
	},
	{
		BLOCK_WATER, 0,
		362, 362, 362, 362, 362, 362 
	},
	{
		BLOCK_SAND, 1,
		211, 211, 211, 211, 211, 211 
	}
};

enum e_face
{
	FACE_FRONT,
	FACE_BACK,
	FACE_LEFT,
	FACE_RIGHT,
	FACE_TOP,
	FACE_BOT,
	FACE_AMOUNT
};

static const float g_faces[FACE_AMOUNT][12] = {
/* FRONT */	{-1, 1, 1,  -1, -1, 1,  1, -1, 1,  1, 1, 1},
/* BACK  */	{1, 1, -1,  1, -1, -1,  -1, -1, -1,  -1, 1, -1},
/* LEFT  */	{-1, 1, -1,  -1, -1, -1,  -1, -1, 1,  -1, 1, 1},
/* RIGHT */	{1, 1, 1,  1, -1, 1,  1, -1, -1,  1, 1, -1},
/* TOP   */	{-1, 1, -1,  -1, 1, 1,  1, 1, 1,  1, 1, -1},
/* BOT   */	{-1, -1, -1,  1, -1, -1,  1, -1, 1,  -1, -1, 1}
};

#endif