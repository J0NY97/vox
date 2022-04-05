#ifndef BLOCK_H
# define BLOCK_H

enum e_block_type
{
	BLOCK_AIR = -1,
	BLOCK_DIRT = 0,
	BLOCK_STONE,
	BLOCK_BEDROCK,
	BLOCK_WATER,
	BLOCK_SAND,
	BLOCK_OAK_LOG,
	BLOCK_OAK_PLANK,
	BLOCK_TYPE_AMOUNT
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

// Every block has a block_data corresponding with its type;
typedef struct s_block_data
{
	char			type; // 'e_block_type', at some point maybe same array index in 'g_block_data';
	char			solid; // if solid or not;
	char			liquid; // if liquid or not; to know if we want to add to liquid mesh;
	unsigned short	face_texture[FACE_AMOUNT]; // same order as 'e_face'
}	t_block_data;

static const t_block_data g_block_data[] = {
	{
		BLOCK_AIR, 0, 0,
		{-1, -1, -1, -1, -1, -1}
	},
	{
		BLOCK_DIRT, 1, 0,
		{132, 132, 132, 132, 135, 224}
	},
	{
		BLOCK_STONE, 1, 0,
		{164, 164, 164, 164, 164, 164}
	},
	{
		BLOCK_BEDROCK, 1, 0,
		{6, 6, 6, 6, 6, 6}
	},
	{
		BLOCK_WATER, 0, 1,
		{362, 362, 362, 362, 362, 362}
	},
	{
		BLOCK_SAND, 1, 0,
		{211, 211, 211, 211, 211, 211}
	},
	{
		BLOCK_OAK_LOG, 1, 0,
		{27, 27, 27, 27, 28, 28}
	},
	{
		BLOCK_OAK_PLANK, 1, 0,
		{280, 280, 280, 280, 280, 280}
	}
};

static const float g_faces[FACE_AMOUNT][12] = {
/* FRONT */	{-1, 1, 1,  -1, -1, 1,  1, -1, 1,  1, 1, 1},
/* BACK  */	{1, 1, -1,  1, -1, -1,  -1, -1, -1,  -1, 1, -1},
/* LEFT  */	{-1, 1, -1,  -1, -1, -1,  -1, -1, 1,  -1, 1, 1},
/* RIGHT */	{1, 1, 1,  1, -1, 1,  1, -1, -1,  1, 1, -1},
/* TOP   */	{-1, 1, -1,  -1, 1, 1,  1, 1, 1,  1, 1, -1},
/* BOT   */	{-1, -1, -1,  1, -1, -1,  1, -1, 1,  -1, -1, 1}
};

static const int g_neighbors[6][3] = {
	{-1, 0, 0},
	{1, 0, 0},
	{0, 1, 0},
	{0, -1, 0},
	{0, 0, 1},
	{0, 0, -1}
};

#endif