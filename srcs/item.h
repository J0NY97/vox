#ifndef ITEM_H
# define ITEM_H

typedef struct s_item_data
{
	char	*name;
}	t_item_data;

enum e_item_type
{
	ITEM_FIRST = BLOCK_TYPE_AMOUNT,
	ITEM_WATER_PLACER,
	ITEM_TREE_PLACER,
	ITEM_LAST // the true amount of items is == (ITEM_LAST - ITEM_FIRST);
};

static const t_item_data g_item_data[] = {
	{
		"ITEM_WATER_PLACER"
	},
	{
		"ITEM_TREE_PLACER"
	}
};

#endif