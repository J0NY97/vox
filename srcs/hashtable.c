#include "hashtable.h"

void	hash_table_clear(t_hash_item *table, int table_size)
{
	int	index;

	index = 0;
	while (index < table_size)
	{
		table[index].data = -1;
		table[index].key = 0;
		++index;
	}
}

void	hash_table_print(t_hash_item *table, int table_size)
{
	int	index;

	index = 0;
	printf("Hash Table Print :\n");
	while (index < table_size)
	{
		printf("%u : %d\n", table[index].key, table[index].data);
		++index;
	}
}

/*
 * We want "key modulo table_size", but since C doesnt have modulo we have to
 * do it ourselves...
*/
int	hash_index(int key, int table_size)
{
	int	res;

	res = key % table_size;
    res = res < 0 ? res + table_size : res;
	if (res < 0)
	{
		printf("Hash Code : %d\n", res);
		exit(0);
	}
	return (res);
}

/*
 * Function that deteremines if the the item is empty or not, if we dont want
 *	to have pointers in the hash table;
*/
int	hash_is_empty(t_hash_item *item)
{
	if (item->data == -1 && item->key == 0)
		return (1);
	return (0);
}

t_hash_item	*hash_item_search(t_hash_item *table, int table_size, int key)
{
	int	index;
	int	amount = 0;

	index = hash_index(key, table_size);
	if (index < 0 || index >= table_size)
	{
		printf("search :Too mcuh\n");
		exit(0);
	}
	while (!hash_is_empty(&table[index]))
	{
		if (amount >= table_size)
		{
	//		printf("Search : We have gone through whole table and cant find it.\n");
			return (NULL);
		}
		if (table[index].key == key)
			return (&table[index]);
		++index;
		++amount;
		index %= table_size;
	}
	return (NULL);
}

/*
 * Returns 1 if success, otherwise 0;
*/
int	hash_item_insert(t_hash_item *table, int table_size, int key, int data)
{
	int	index;
	int	amount = 0;

	index = hash_index(key, table_size);
	if (index < 0 || index >= table_size)
	{
		printf("INsert :Too mcuh\n");
		exit(0);
	}
	while (!hash_is_empty(&table[index]))
	{
		if (amount >= table_size)
		{
			printf("Insert : We have gone through whole table and cant insert it. (key : %d, data  : %d)\n", key, data);
			return (0);
		}
		++index;
		++amount;
		index %= table_size;
	}
	table[index].data = data;
	table[index].key = key;
	return (1);
}

/*
 * Returns 1 if success, otherwise 0;
*/
int	hash_item_delete(t_hash_item *table, int table_size, int key)
{
	int	index;
	int amount = 0;

	index = hash_index(key, table_size);
	if (index < 0 || index >= table_size)
	{
		printf("delete :Too mcuh\n");
		exit(0);
	}
	while (!hash_is_empty(&table[index]))
	{
		if (amount >= table_size)
		{
			printf("Delete : We have gone through whole table and cant find it.\n");
			return (0);
		}
		if (table[index].key == key)
		{
			table[index].data = -1;
			table[index].key = 0;
			return (1);
		}
		++index;
		++amount;
		index %= table_size;
	}
	return (0);
}