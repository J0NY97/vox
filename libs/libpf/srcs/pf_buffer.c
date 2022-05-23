/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2021/05/14 11:47:04 nneronin
 * Updated: 2022/01/07 15:03:40 nneronin
 */

#include "libpf.h"

void	fill_buffer(t_pf *p, const char *s, unsigned int size)
{
	unsigned int	i;

	i = 0;
	if (p->print_len != -1)
		p->print_len += size;
	if (p->chars + size > MAX_INT)
		p->print_len = -1;
	if (p->chars + size >= p->size)
	{
		fwrite(p->buffer, p->chars, 1, p->fp);
		p->chars = 0;
	}
	if (size < p->size)
	{
		while (i < size)
		{
			p->buffer[p->chars] = s[i];
			i++;
			p->chars++;
		}
	}
	else
		fwrite(s, size, 1, p->fp);
}
