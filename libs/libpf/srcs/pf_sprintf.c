/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2022/01/07 14:51:55 nneronin
 * Updated: 2022/01/07 14:58:37 nneronin
 */

#include "libpf.h"

int	ft_sprintf(char *str, const char *restrict format, ...)
{
	t_pf	p;

	if (!format)
		return (-1);
	pf_init(&p, NULL, str, PF_BUFF_SIZE);
	va_start(p.ap, format);
	pf_read_format((char *)format, &p);
	va_end(p.ap);
	str[p.chars] = '\0';
	return (p.print_len);
}

int	ft_snprintf(char *str, int len, const char *restrict format, ...)
{
	t_pf	p;

	if (!format)
		return (-1);
	pf_init(&p, NULL, str, len);
	va_start(p.ap, format);
	pf_read_format((char *)format, &p);
	va_end(p.ap);
	str[len] = '\0';
	if ((int)p.chars < len)
		str[p.chars] = '\0';
	return (p.print_len);
}

