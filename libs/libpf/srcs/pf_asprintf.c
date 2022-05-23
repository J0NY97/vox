/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2022/01/07 14:52:22 nneronin
 * Updated: 2022/01/07 14:52:43 nneronin
 */

#include "libpf.h"

int	ft_asprintf(char **restrict ptr, const char *restrict format, ...)
{
	t_pf	p;
	char	buff[PF_BUFF_SIZE];

	if (!format)
		return (-1);
	pf_init(&p, stdout, buff, PF_BUFF_SIZE);
	va_start(p.ap, format);
	pf_read_format((char *)format, &p);
	va_end(p.ap);
	*ptr = ft_strndup(p.buffer, p.chars);
	return (p.print_len);
}
