#ifndef FLUID_H
# define FLUID_H

# include "shaderpixel.h"

typedef struct s_fluid_data
{
	char	type;
	char	*name;
	char	flow_dir; // corresponds to one of 'e_flow_dir', which corresponds to a 'float []' in g_flow_dir;
	char	dist_from_source;
	int		texture; // number from the texture pack;
}			t_fluid_data;

enum e_fluid_type
{
	FLUID_FIRST = FLORA_LAST,
	FLUID_WATER,
	FLUID_LAST
};

enum e_flow_dir
{
	FLOW_SOURCE, // not flowing, just static;
	FLOW_NORTH,
	FLOW_EAST,
	FLOW_SOUTH,
	FLOW_WEST
};

static const t_fluid_data	g_fluid_data[] = {
	{
		FLUID_WATER, "FLUID_WATER", FLOW_SOURCE, 0, 362
	}
};

static const float g_flow_dir[5][3] = {
	{0, 0, 0},
	{0, 0, 1},
	{1, 0, 0},
	{0, 0, -1},
	{-1, 0, 0}
};

#endif