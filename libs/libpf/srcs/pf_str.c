/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2021/05/14 12:14:03 nneronin
 * Updated: 2022/01/05 12:45:08 nneronin
 */

#include "libpf.h"

static void	set_padding(t_pf *p, char *s)
{
	if (p->precision)
		p->padding.size = ft_strlen(s);
	if (p->precision > 0 && p->precision < p->padding.size)
		p->padding.size = p->precision;
	if (!p->flag.minus && p->flag.zero)
		p->padding.zeros = p->min_width - p->padding.size;
	space_padding(p, 0);
}

void	pf_str(t_pf *p)
{
	char	*s;

	s = va_arg(p->ap, char *);
	if (s == NULL)
		s = "(null)";
	set_padding(p, s);
	put_left_spaces(p);
	put_zeros(p);
	if (p->precision != 0)
		fill_buffer(p, s, p->padding.size);
	put_right_spaces(p);
}
