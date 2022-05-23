/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2022/01/07 14:51:06 nneronin
 * Updated: 2022/05/23 2:24:49 Epicurius
 */

#include "libpf.h"

int	ft_vfprintf(FILE *fp, const char *restrict format, va_list ap)
{
	t_pf	p;
	char	buff[PF_BUFF_SIZE];

	if (!format)
		return (-1);
	pf_init(&p, fp, buff, PF_BUFF_SIZE);
	va_copy(p.ap, ap);
	pf_read_format((char *)format, &p);
	fwrite(p.buffer, p.chars, 1, p.fp);
	return (p.print_len);
}

int	ft_fprintf(FILE *fp, const char *restrict format, ...)
{
	va_list	ap;
	int		len;
	
	if (!format)
		return (-1);
	va_start(ap, format);
	len = ft_vfprintf(fp, format, ap);
	va_end(ap);
	return (len);
}
