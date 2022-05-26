#ifndef ITEM_H
# define ITEM_H

#include "shaderpixel.h"

typedef struct s_item_data
{
	char	*name;
}	t_item_data;

enum e_item_type
{
	ITEM_FIRST = FLUID_LAST,
	ITEM_TREE_PLACER,
	ITEM_LAST
};

static const t_item_data g_item_data[] = {
	{
		"ITEM_TREE_PLACER"
	}
};

#endif