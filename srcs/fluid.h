#ifndef FLUID_H
# define FLUID_H

# include "shaderpixel.h"

typedef struct s_fluid_data
{
	char	type;
	char	*name;
	char	dist_from_source;
	int		texture; // number from the texture pack;
}			t_fluid_data;

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

static const t_fluid_data	g_fluid_data[] = {
	{
		FLUID_WATER, "FLUID_WATER", 0, 362
	},
	{
		FLUID_WATER_1, "FLUID_WATER_1", 1, 362
	},
	{
		FLUID_WATER_2, "FLUID_WATER_2", 2, 362
	},
	{
		FLUID_WATER_3, "FLUID_WATER_3", 3, 362
	},
	{
		FLUID_WATER_4, "FLUID_WATER_4", 4, 362
	},
	{
		FLUID_WATER_5, "FLUID_WATER_5", 5, 362
	},
	{
		FLUID_WATER_6, "FLUID_WATER_6", 6, 362
	},
	{
		FLUID_WATER_7, "FLUID_WATER_7", 7, 362
	}
};

#endif