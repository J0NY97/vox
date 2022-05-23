/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2022/01/05 15:31:28 nneronin
 * Updated: 2022/01/07 15:04:09 nneronin
 */

#include "libpf.h"

void	pf_init(t_pf *p, FILE *fp, char *buffer, int size)
{
	ft_memset(p, 0, sizeof(t_pf));
	p->chars = 0;
	p->print_len = 0;
	p->fp = fp;
	p->buffer = buffer;
	p->size = size;
	reset_options(p);
}

