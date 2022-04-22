#ifndef HASHTABLE_H
# include "stdlib.h"
# include "stdio.h"

typedef struct s_hash_item
{
	int	data;
	int	key;
	int	is_empty;
}			t_hash_item;

int			hash_item_is_empty(t_hash_item *item);
void		hash_item_clear(t_hash_item *item);
void		hash_item_populate(t_hash_item *item, int data, int key);
void		hash_table_clear(t_hash_item *table, int table_size);
void		hash_table_print(t_hash_item *table, int table_size);
int			hash_index(int key, int table_size);
t_hash_item	*hash_item_search(t_hash_item *table, int table_size, int key);
int			hash_item_insert(t_hash_item *table, int table_size, int key, int data);
int			hash_item_delete(t_hash_item *table, int table_size, int key);

#endif