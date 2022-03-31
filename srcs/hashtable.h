#ifndef HASHTABLE_H
# include "stdlib.h"
# include "stdio.h"

typedef struct s_hash_item
{
	int	data;
	int	key;
}			t_hash_item;

void		hash_table_clear(t_hash_item *table, int table_size);
void		hash_table_print(t_hash_item *table, int table_size);
int			hash_index(int key, int table_size);
int			hash_is_empty(t_hash_item *item);
t_hash_item	*hash_item_search(t_hash_item *table, int table_size, int key);
int			hash_item_insert(t_hash_item *table, int table_size, int key, int data);
int			hash_item_delete(t_hash_item *table, int table_size, int key);

#endif