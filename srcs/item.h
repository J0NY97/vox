#ifndef ITEM_H
# define ITEM_H

enum e_item_type
{
	ITEM_FIRST = BLOCK_TYPE_AMOUNT,
	ITEM_TREE_PLACER,
	ITEM_LAST // the true amount of items is == (ITEM_LAST - ITEM_FIRST);
};

#endif