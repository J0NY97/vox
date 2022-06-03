#ifndef BLOCK_H
# define BLOCK_H

enum e_gas_type
{
	GAS_FIRST = -1,
	GAS_AIR = 0,
	GAS_LAST = 1,
};

enum e_solid_type
{
	BLOCK_FIRST = GAS_LAST - 1,
	BLOCK_DIRT_GRASS,
	BLOCK_DIRT,
	BLOCK_STONE,
	BLOCK_BEDROCK,
	BLOCK_SAND,
	BLOCK_OAK_LOG,
	BLOCK_OAK_PLANK,
	BLOCK_TNT,
	BLOCK_LAST
};

enum e_solid_alpha_type
{
	BLOCK_ALPHA_FIRST = BLOCK_LAST - 1,
	BLOCK_ALPHA_OAK_LEAF,
	BLOCK_ALPHA_CACTUS,
	BLOCK_ALPHA_TORCH,
	BLOCK_ALPHA_LAST
};

enum e_flora_type
{
	FLORA_FIRST = BLOCK_ALPHA_LAST - 1,
	FLORA_GRASS,
	FLORA_FLOWER_RED,
	FLORA_FLOWER_YELLOW,
	FLORA_LAST
};

enum e_fluid_type
{
	FLUID_FIRST = FLORA_LAST - 1,
	FLUID_WATER,
	FLUID_WATER_1,
	FLUID_WATER_2,
	FLUID_WATER_3,
	FLUID_WATER_4,
	FLUID_WATER_5,
	FLUID_WATER_6,
	FLUID_WATER_7,
	FLUID_LAST
};

/* 'FLUID_LAST' should be the last block type */
#define BLOCK_TYPE_AMOUNT FLUID_LAST

/*
 * Used in 't_block->visible_faces' to 'OR' all face into a char;
 *	Not used anywhere else, if you need faces somewhere, use the cardinal dirs;
*/
static const int g_visible_faces[] = {
	0x00000080,
	0x00000040,
	0x00000020,
	0x00000010,
	0x00000008,
	0x00000004,
	0x000000ff // ALL faces
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

static const float g_faces_torch[6][12] = {
/* NORTH */ {1, 1, -0.125,	1, -1, -0.125,	-1, -1, -0.125,	-1, 1, -0.125},
/* RIGHT */	{0.125, 1, 1,  	0.125, -1, 1,		0.125, -1, -1,	0.125, 1, -1},
/* SOUTH */	{-1, 1, 0.125,	-1, -1, 0.125,	1, -1, 0.125,		1, 1, 0.125},
/* LEFT  */	{-0.125, 1, -1,	-0.125, -1, -1,	-0.125, -1, 1,	-0.125, 1, 1},
/* TOP   */	{-1, 0.25, -1,	-1, 0.25, 1,		1, 0.25, 1,		1, 0.25, -1},
/* BOT   */	{-1, -1, -1,	1, -1, -1,	1, -1, 1,		-1, -1, 1}
};

static const int g_face_light[6] = {
	80, 80, 60, 60, 100, 50
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

// Same as g_card_dir, but type casted to int already;
static const int g_card_dir_int[DIR_AMOUNT][3] = {
	{(int)g_card_dir[0][0], (int)g_card_dir[0][1], (int)g_card_dir[0][2]},
	{(int)g_card_dir[1][0], (int)g_card_dir[1][1], (int)g_card_dir[1][2]},
	{(int)g_card_dir[2][0], (int)g_card_dir[2][1], (int)g_card_dir[2][2]},
	{(int)g_card_dir[3][0], (int)g_card_dir[3][1], (int)g_card_dir[3][2]},
	{(int)g_card_dir[4][0], (int)g_card_dir[4][1], (int)g_card_dir[4][2]},
	{(int)g_card_dir[5][0], (int)g_card_dir[5][1], (int)g_card_dir[5][2]},
	{(int)g_card_dir[6][0], (int)g_card_dir[6][1], (int)g_card_dir[6][2]},
	{(int)g_card_dir[7][0], (int)g_card_dir[7][1], (int)g_card_dir[7][2]},
	{(int)g_card_dir[8][0], (int)g_card_dir[8][1], (int)g_card_dir[8][2]},
	{(int)g_card_dir[9][0], (int)g_card_dir[9][1], (int)g_card_dir[9][2]}
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
	char			type; // 'e_block_type', same as array index in 'g_block_data';
	char			*name;
	char			*readable_name;
	unsigned short	texture[6]; // same order as 'e_face'
	char			light_emit; // negative is remove, positive is add;
	float			blast_resistance;
	float			**faces; // TODO
}	t_block_data;


static const t_block_data	g_block_data[] = {
// GAS
	{
		GAS_AIR,
		"GAS_AIR",
		"Air",
		{0, 0, 0, 0, 0, 0},
		0, 0.0f,
		(float **)g_faces
	},
// BLOCKS
	{
		BLOCK_DIRT_GRASS, 
		"BLOCK_DIRT_GRASS",
		"Dirt",
		{132, 132, 132, 132, 135, 224},
		-15, 0.5f,
		(float **)g_faces
	},
	{
		BLOCK_DIRT, 
		"BLOCK_DIRT",
		"Dirt",
		{224, 224, 224, 224, 224, 224},
		-15, 0.5f,
		(float **)g_faces
	},
	{
		BLOCK_STONE, 
		"BLOCK_STONE",
		"Stone",
		{164, 164, 164, 164, 164, 164},
		-15, 6.0f,
		(float **)g_faces
	},
	{
		BLOCK_BEDROCK, 
		"BLOCK_BEDROCK",
		"Bedrock",
		{292, 292, 292, 292, 292, 292},
		-15, 3600000.0f,
		(float **)g_faces
	},
	{
		BLOCK_SAND, 
		"BLOCK_SAND",
		"Sand",
		{211, 211, 211, 211, 211, 211},
		-15, 0.5f,
		(float **)g_faces
	},
	{
		BLOCK_OAK_LOG, 
		"BLOCK_OAK_LOG",
		"Oak Log",
		{27, 27, 27, 27, 28, 28},
		-15, 2.0f,
		(float **)g_faces
	},
	{
		BLOCK_OAK_PLANK, 
		"BLOCK_OAK_PLANK",
		"Oak Plank",
		{280, 280, 280, 280, 280, 280},
		-15, 3.0f,
		(float **)g_faces
	},
	{
		BLOCK_TNT, 
		"BLOCK_TNT",
		"TNT",
		{189, 189, 189, 189, 165, 213},
		-15, 0.0f,
		(float **)g_faces
	},
// BLOCK ALPHA
	{
		BLOCK_ALPHA_OAK_LEAF, 
		"BLOCK_ALPHA_OAK_LEAF",
		"Oak Leaf",
		{52, 52, 52, 52, 52, 52},
		-1, 0.2f,
		(float **)g_faces
	},
	{
		BLOCK_ALPHA_CACTUS, 
		"BLOCK_ALPHA_CACTUS",
		"Cactus",
		{342, 342, 342, 342, 318, 366},
		-1, 0.4f,
		(float **)g_faces_cactus
	},
	{
		BLOCK_ALPHA_TORCH, 
		"BLOCK_ALPHA_TORCH",
		"Torch",
		{141, 141, 141, 141, 42, 141},
		14, 0.4f,
		(float **)g_faces_torch
	},
// FLORA
	{
		FLORA_GRASS, 
		"FLORA_GRASS",
		"Grass",
		{275, 275, 0, 0, 0, 0},
		0, 0.0f,
		(float **)g_flora_faces
	},
	{
		FLORA_FLOWER_RED, 
		"FLORA_FLOWER_RED",
		"Red Flower",
		{231, 231, 0, 0, 0, 0},
		0, 0.0f,
		(float **)g_flora_faces
	},
	{
		FLORA_FLOWER_YELLOW, 
		"FLORA_FLOWER_YELLOW",
		"Yellow Flower",
		{327, 327, 0, 0, 0, 0},
		0, 0.0f,
		(float **)g_flora_faces
	},
// FLUID
	{
		FLUID_WATER, "FLUID_WATER",
		"Water",
		{362, 362, 362, 362, 362, 362},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_1, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_2, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_3, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_4, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_5, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_6, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	},
	{
		FLUID_WATER_7, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 100.0f,
		(float **)g_faces
	}
};

#endif