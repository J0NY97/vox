#include "hashtable.h"

int	hash_item_is_empty(t_hash_item *item)
{
	return (item->is_empty);
}

void	hash_item_clear(t_hash_item *item)
{
	item->data = 0;
	item->key = 0;
	item->is_empty = 1;
}

void	hash_item_populate(t_hash_item *item, int data, int key)
{
	item->data = data;
	item->key = key;
	item->is_empty = 0;
}

void	hash_table_clear(t_hash_item *table, int table_size)
{
	int	index;

	index = 0;
	while (index < table_size)
	{
		hash_item_clear(&table[index]);
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
		if (hash_item_is_empty(&table[index]))
			printf("#%d = EMPTY\n", index);
		else
			printf("#%d = %d : %d\n", index, table[index].key, table[index].data);
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
	while (1)
	{
		if (amount >= table_size)
		{
		//	printf("Search : We have gone through whole table and cant find it. (key : %d, index : %d, amount : %d)\n", key, index, amount);
		//	exit(0);
			return (NULL);
		}
		if (table[index].key == key)
		{
		//	printf("Found on %d:th try.\n", amount);
			return (&table[index]);
		}
		++index;
		++amount;
		if (index >= table_size)
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
	while (!hash_item_is_empty(&table[index]))
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
	if (amount < table_size)
	{
		hash_item_populate(&table[index], data, key);
		return (1);
	}
	return (0);
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
		printf("HASH_ITEM_DELETE : Index too high.\n");
	while (1)
	{
		if (amount >= table_size)
		{
			printf("Delete : We have gone through whole table and cant find it.\n");
			return (0);
		}
		if (table[index].key == key)
		{
			hash_item_clear(&table[index]);
			return (1);
		}
		++index;
		++amount;
		index %= table_size;
	}
	return (0);
}