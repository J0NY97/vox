#ifndef FLUID_H
# define FLUID_H

# include "shaderpixel.h"

typedef struct s_fluid_data
{
	char	type;
	char	*name;
	char	flow_dir; // corresponds to one of 'e_card_dir', which corresponds to a 'float *' in 'g_card_dir';
	char	dist_from_source;
	int		texture; // number from the texture pack;
}			t_fluid_data;

enum e_fluid_type
{
	FLUID_FIRST = FLORA_LAST,
	FLUID_WATER,
	FLUID_LAST
};

static const t_fluid_data	g_fluid_data[] = {
	{
		FLUID_WATER, "FLUID_WATER", DIR_SOURCE, 0, 362
	}
};

#endif