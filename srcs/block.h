#ifndef BLOCK_H
# define BLOCK_H

enum e_gas_type
{
	GAS_FIRST = 0,
	GAS_AIR,
	GAS_LAST,
};

enum e_solid_type
{
	BLOCK_FIRST = GAS_LAST,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_BEDROCK,
	BLOCK_SAND,
	BLOCK_OAK_LOG,
	BLOCK_OAK_PLANK,
	BLOCK_LAST
};

enum e_solid_alpha_type
{
	BLOCK_ALPHA_FIRST = BLOCK_LAST,
	BLOCK_ALPHA_OAK_LEAF,
	BLOCK_ALPHA_CACTUS,
	BLOCK_ALPHA_LAST
};

enum e_flora_type
{
	FLORA_FIRST = BLOCK_ALPHA_LAST,
	FLORA_GRASS,
	FLORA_FLOWER_RED,
	FLORA_FLOWER_YELLOW,
	FLORA_LAST
};

enum e_card_dir // cardinal direction aka väderstreck?
{
	DIR_NORTH = 0,
	DIR_EAST,
	DIR_SOUTH,
	DIR_WEST,
	DIR_UP,
	DIR_DOWN,
	DIR_NORTHEAST,
	DIR_NORTHWEST,
	DIR_SOUTHEAST,
	DIR_SOUTHWEST,
	DIR_SOURCE, // not flowing, just static;
	DIR_AMOUNT = DIR_SOURCE
};

static const float g_card_dir[DIR_AMOUNT][3] = {
	{0, 0, -1},
	{1, 0, 0},
	{0, 0, 1},
	{-1, 0, 0},
	{0, 1, 0},
	{0, -1, 0},
	{1, 0, -1},
	{-1, 0, -1},
	{1, 0, 1},
	{-1, 0, 1}
};

/*
 * Vegetation faces;
*/
enum e_flora_face
{
	FACE_FLORA_A,
	FACE_FLORA_B,
	FACE_FLORA_AMOUNT
};

// Every block has a block_data corresponding with its type;
typedef struct s_block_data
{
	char			type; // 'e_block_type', at some point maybe same array index in 'g_block_data';
	char			*name;
	unsigned short	face_texture[6]; // same order as 'e_face'
	char			light_emit; // negative is remove, positive is add;
}	t_block_data;

static const t_block_data	g_gas_data[] = {
	{
		GAS_AIR,
		"GAS_AIR",
		{0, 0, 0, 0, 0, 0},
		0
	}
};

static const t_block_data g_block_data[] = {
	{
		BLOCK_DIRT, 
		"BLOCK_DIRT",
		{132, 132, 132, 132, 135, 224},
		-15
	},
	{
		BLOCK_STONE, 
		"BLOCK_STONE",
		{164, 164, 164, 164, 164, 164},
		-15
	},
	{
		BLOCK_BEDROCK, 
		"BLOCK_BEDROCK",
		{292, 292, 292, 292, 292, 292},
		-15
	},
	{
		BLOCK_SAND, 
		"BLOCK_SAND",
		{211, 211, 211, 211, 211, 211},
		-15
	},
	{
		BLOCK_OAK_LOG, 
		"BLOCK_OAK_LOG",
		{27, 27, 27, 27, 28, 28},
		-15
	},
	{
		BLOCK_OAK_PLANK, 
		"BLOCK_OAK_PLANK",
		{280, 280, 280, 280, 280, 280},
		-15
	}
};

static const t_block_data	g_block_alpha_data[] = {
	{
		BLOCK_ALPHA_OAK_LEAF, 
		"BLOCK_ALPHA_OAK_LEAF",
		{52, 52, 52, 52, 52, 52},
		-1
	},
	{
		BLOCK_ALPHA_CACTUS, 
		"BLOCK_ALPHA_CACTUS",
		{342, 342, 342, 342, 318, 366},
		-1
	}
};

static const t_block_data	g_flora_data[] = {
	{
		FLORA_GRASS, 
		"FLORA_GRASS",
		{275, 275, 0, 0, 0, 0},
		0
	},
	{
		FLORA_FLOWER_RED, 
		"FLORA_FLOWER_RED",
		{231, 231, 0, 0, 0, 0},
		0
	},
	{
		FLORA_FLOWER_YELLOW, 
		"FLORA_FLOWER_YELLOW",
		{327, 327, 0, 0, 0, 0},
		0
	}
};

static const float g_faces[6][12] = {
/* NORTH */ {1, 1, -1,  1, -1, -1,  -1, -1, -1,  -1, 1, -1},
/* RIGHT */	{1, 1, 1,  1, -1, 1,  1, -1, -1,  1, 1, -1},
/* SOUTH */	{-1, 1, 1,  -1, -1, 1,  1, -1, 1,  1, 1, 1},
/* LEFT  */	{-1, 1, -1,  -1, -1, -1,  -1, -1, 1,  -1, 1, 1},
/* TOP   */	{-1, 1, -1,  -1, 1, 1,  1, 1, 1,  1, 1, -1},
/* BOT   */	{-1, -1, -1,  1, -1, -1,  1, -1, 1,  -1, -1, 1}
};

static const float g_flora_faces[2][12] = {
	/* A */ {-1, 1, 1,  -1, -1, 1,  1, -1, -1,  1, 1, -1},
	/* B */ {-1, 1, -1,  -1, -1, -1,  1, -1, 1,  1, 1, 1}
};

static const float g_faces_cactus[6][12] = {
/* NORTH */ {1, 1, -0.85,	1, -1, -0.85,	-1, -1, -0.85,	-1, 1, -0.85},
/* RIGHT */	{0.85, 1, 1,  	0.85, -1, 1,	0.85, -1, -1,	0.85, 1, -1},
/* SOUTH */	{-1, 1, 0.85,	-1, -1, 0.85,	1, -1, 0.85,	1, 1, 0.85},
/* LEFT  */	{-0.85, 1, -1,	-0.85, -1, -1,	-0.85, -1, 1,	-0.85, 1, 1},
/* TOP   */	{-1, 1, -1,		-1, 1, 1,		1, 1, 1,		1, 1, -1},
/* BOT   */	{-1, -1, -1,	1, -1, -1,		1, -1, 1,		-1, -1, 1}
};

static const int g_face_light[6] = {
	65, 90, 85, 65, 85, 65
};

#endif