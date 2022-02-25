#include "bobj_loader.h"

void	bobj_element_print(t_element *elem)
{
	printf("[%s]\n", __FUNCTION__);
	printf("\tindices_size : %d\n", elem->indices_size);
	printf("\tindiecs_value_amount : %d\n", elem->indices_value_amount);
	printf("\tindex_amount : %d\n", elem->index_amount);
	printf("\tindex_value_amount : %d\n", elem->index_value_amount);
	printf("\tindex_allocated : %d\n", elem->index_allocated);
}