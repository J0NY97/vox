/*
 * https://github.com/Epicurius/Doom-Not-Doom
 * 
 * Created: 2021/05/14 11:16:38 nneronin
 * Updated: 2022/01/07 14:59:48 nneronin
 */

#include "libpf.h"

int	ft_vprintf(const char *restrict format, va_list ap)
{
	return (ft_vfprintf(stdout, format, ap));
}

int	ft_printf(const char *restrict format, ...)
{
	va_list	ap;
	int		len;
	
	if (!format)
		return (-1);
	va_start(ap, format);
	len = ft_vfprintf(stdout, format, ap);
	va_end(ap);
	return (len);
}
