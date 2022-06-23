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

/* EXTRA stuff that shouldnt be added to the g_block_data */
enum e_item_type
{
	ITEM_FIRST = FLUID_LAST - 1,
	ITEM_TREE_PLACER, // aka sapling...
	ITEM_LAST
};

/*
 * Used in 't_block->visible_faces' to 'OR' all face into a char;
 *	Not used anywhere else, if you need faces somewhere, use the cardinal dirs;
*/
static const char	g_visible_faces[] = {
	0x80,
	0x40,
	0x20,
	0x10,
	0x08,
	0x04,
	0xff // ALL faces
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

enum e_face_verts
{
	BLOCK_FACES,
	FLORA_FACES,
	CACTUS_FACES,
	TORCH_FACES
};

static const float g_all_faces[][6][12] = {
	{ // g_faces
/* NORTH */ {1, 1, -1,  1, -1, -1,  -1, -1, -1,  -1, 1, -1},
/* RIGHT */	{1, 1, 1,  1, -1, 1,  1, -1, -1,  1, 1, -1},
/* SOUTH */	{-1, 1, 1,  -1, -1, 1,  1, -1, 1,  1, 1, 1},
/* LEFT  */	{-1, 1, -1,  -1, -1, -1,  -1, -1, 1,  -1, 1, 1},
/* TOP   */	{-1, 1, -1,  -1, 1, 1,  1, 1, 1,  1, 1, -1},
/* BOT   */	{-1, -1, -1,  1, -1, -1,  1, -1, 1,  -1, -1, 1}
	},
	{ // g_faces_flora
	/* A */ {-1, 1, 1,  -1, -1, 1,  1, -1, -1,  1, 1, -1},
	/* B */ {-1, 1, -1,  -1, -1, -1,  1, -1, 1,  1, 1, 1},
	/* C */ {1, 1, 1,  1, -1, 1,  -1, -1, -1,  -1, 1, -1},
	/* D */ {1, 1, -1,  1, -1, -1,  -1, -1, 1,  -1, 1, 1}
	},
	{ // g_face_cactus
/* NORTH */ {1, 1, -0.85,	1, -1, -0.85,	-1, -1, -0.85,	-1, 1, -0.85},
/* RIGHT */	{0.85, 1, 1,  	0.85, -1, 1,	0.85, -1, -1,	0.85, 1, -1},
/* SOUTH */	{-1, 1, 0.85,	-1, -1, 0.85,	1, -1, 0.85,	1, 1, 0.85},
/* LEFT  */	{-0.85, 1, -1,	-0.85, -1, -1,	-0.85, -1, 1,	-0.85, 1, 1},
/* TOP   */	{-1, 1, -1,		-1, 1, 1,		1, 1, 1,		1, 1, -1},
/* BOT   */	{-1, -1, -1,	1, -1, -1,		1, -1, 1,		-1, -1, 1}
	},
	{ // g_faces_torch
/* NORTH */ {1, 1, -0.125,	1, -1, -0.125,	-1, -1, -0.125,	-1, 1, -0.125},
/* RIGHT */	{0.125, 1, 1,  	0.125, -1, 1,		0.125, -1, -1,	0.125, 1, -1},
/* SOUTH */	{-1, 1, 0.125,	-1, -1, 0.125,	1, -1, 0.125,		1, 1, 0.125},
/* LEFT  */	{-0.125, 1, -1,	-0.125, -1, -1,	-0.125, -1, 1,	-0.125, 1, 1},
/* TOP   */	//{-1, 0.25, -1,	-1, 0.25, 1,		1, 0.25, 1,		1, 0.25, -1},
/* BOT   */	//{-1, -1, -1,	1, -1, -1,	1, -1, 1,		-1, -1, 1}
	}
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
	char			type; // 'e_block_type', same as array index in 'g_block_data';
	char			*name;
	char			*readable_name;
	unsigned short	texture[6]; // same order as 'e_face'
	char			light_emit; // negative is remove, positive is add;
	char			trigger_light_heightmap; // 0 / 1;
	float			blast_resistance;
	char			force_see_through;
	char			force_through_see;
	char			see_through; // (affects neighbors) if the block is see through and neighboring block face should be added to mesh;
	char			through_see; // (affects block)		if the block faces should be added to the mesh if the neighboring block is see through;
	char			entity_collision; // if player will collide with its body;
	char			hand_collision; // if you can break the block;
	char			max_stack_size; // the maximum value of items in one stack in the inventory;
	char			face_index; // 'e_face_verts', from g_all_faces, the index of the faces to be used;
}	t_block_data;

/*
 * ITEMS
 *
 * Keeping these in separate list for now, dont want to convolute the block data
*/
static const t_block_data	g_item_data[] = {
	{
		ITEM_TREE_PLACER,
		"ITEM_TREE_PLACER",
		"Tree Placer (probably sapling at some point)",
		// The values from here down, dont matter;
		{},
		0, 0, 0.0f,
		0, 0, 0, 0,
		0, 0,
		0,
		BLOCK_FACES
	}
};


static const t_block_data	g_block_data[] = {
// GAS
	{
		GAS_AIR,
		"GAS_AIR",
		"Air",
		{0, 0, 0, 0, 0, 0},
		0, 0, 0.0f,
		1, 0, 1, 0,
		0, 0,
		0,
		BLOCK_FACES
	},
// BLOCKS
	{
		BLOCK_DIRT_GRASS,
		"BLOCK_DIRT_GRASS",
		"Dirt",
		{132, 132, 132, 132, 135, 224},
		-15, 1, 0.5f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_DIRT,
		"BLOCK_DIRT",
		"Dirt",
		{224, 224, 224, 224, 224, 224},
		-15, 1, 0.5f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_STONE,
		"BLOCK_STONE",
		"Stone",
		{164, 164, 164, 164, 164, 164},
		-15, 1, 6.0f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_BEDROCK,
		"BLOCK_BEDROCK",
		"Bedrock",
		{292, 292, 292, 292, 292, 292},
		-15, 1, 3600000.0f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_SAND,
		"BLOCK_SAND",
		"Sand",
		{211, 211, 211, 211, 211, 211},
		-15, 1, 0.5f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_OAK_LOG,
		"BLOCK_OAK_LOG",
		"Oak Log",
		{27, 27, 27, 27, 28, 28},
		-15, 1, 2.0f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_OAK_PLANK,
		"BLOCK_OAK_PLANK",
		"Oak Plank",
		{280, 280, 280, 280, 280, 280},
		-15, 1, 3.0f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_TNT,
		"BLOCK_TNT",
		"TNT",
		{189, 189, 189, 189, 165, 213},
		-15, 1, 0.0f,
		0, 0, 0, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
// BLOCK ALPHA
	{
		BLOCK_ALPHA_OAK_LEAF,
		"BLOCK_ALPHA_OAK_LEAF",
		"Oak Leaf",
		{52, 52, 52, 52, 52, 52},
		-1, 1, 0.2f,
		1, 0, 1, 1,
		1, 1,
		64,
		BLOCK_FACES
	},
	{
		BLOCK_ALPHA_CACTUS,
		"BLOCK_ALPHA_CACTUS",
		"Cactus",
		{342, 342, 342, 342, 318, 366},
		-1, 1, 0.4f,
		1, 1, 1, 1,
		1, 1,
		64,
		CACTUS_FACES
	},
	{
		BLOCK_ALPHA_TORCH,
		"BLOCK_ALPHA_TORCH",
		"Torch",
		{141, 141, 141, 141, 42, 141},
		14, 0, 0.4f,
		1, 1, 1, 1,
		1, 1,
		64,
		TORCH_FACES
	},
// FLORA
	{
		FLORA_GRASS,
		"FLORA_GRASS",
		"Grass",
		{275, 275, 275, 275, 0, 0},
		0, 0, 0.0f,
		1, 1, 1, 1,
		0, 1,
		64,
		FLORA_FACES
	},
	{
		FLORA_FLOWER_RED,
		"FLORA_FLOWER_RED",
		"Red Flower",
		{231, 231, 231, 231, 0, 0},
		0, 0, 0.0f,
		1, 1, 1, 1,
		0, 1,
		64,
		FLORA_FACES
	},
	{
		FLORA_FLOWER_YELLOW,
		"FLORA_FLOWER_YELLOW",
		"Yellow Flower",
		{327, 327, 327, 327, 0, 0},
		0, 0, 0.0f,
		1, 1, 1, 1,
		0, 1,
		64,
		FLORA_FACES
	},
// FLUID
	{
		FLUID_WATER, "FLUID_WATER",
		"Water",
		{362, 362, 362, 362, 362, 362},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		64,
		BLOCK_FACES
	},
	{
		FLUID_WATER_1, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	},
	{
		FLUID_WATER_2, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	},
	{
		FLUID_WATER_3, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	},
	{
		FLUID_WATER_4, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	},
	{
		FLUID_WATER_5, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	},
	{
		FLUID_WATER_6, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	},
	{
		FLUID_WATER_7, "FLUID_WATER",
		"Water",
		{362, 0, 0, 0, 0, 0},
		-3, 1, 100.0f,
		0, 0, 1, 0,
		1, 0,
		0,
		BLOCK_FACES
	}
};

#endif
