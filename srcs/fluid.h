#ifndef FLUID_H
# define FLUID_H

# include "shaderpixel.h"

enum e_fluid_type
{
	FLUID_FIRST = FLORA_LAST,
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

static const t_block_data	g_fluid_data[] = {
	{
		FLUID_WATER, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_1, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_2, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_3, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_4, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_5, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_6, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	},
	{
		FLUID_WATER_7, "FLUID_WATER",
		{362, 0, 0, 0, 0, 0},
		-3
	}
};

#endif