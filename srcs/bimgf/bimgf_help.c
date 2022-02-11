#include "bimgf.h"

/*
 * Returns 1 if true, 0 if not;
 * NOTE: in the ft_ version, its flipped;
*/
int	strendswith(char const *src, char const *comp)
{
	int	src_len;
	int	comp_len;

	if (!src || !comp)
		return (0);
	src_len = strlen(src);
	comp_len = strlen(comp);
	if (src_len < comp_len)
		return (0);
	return (!strcmp(src + src_len - comp_len, comp));
}