/*
 * Niklas Neronin <niklas.neronin@gmail.com>
 * https://github.com/Epicurius
 * 
 * Created: 2022/01/05 12:40:52 Niklas Neronin
 * Updated: 2022/01/07 2:37:55 Niklas Neronin
 * Copyright (c) 2022
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../libft/libft.h"
#include "./libpf.h"

# define TEST(format, ...)	ft_printf(format, __VA_ARGS__), printf(format, __VA_ARGS__), format

unsigned char err = 0;

void check(int a, int b, char *format)
{
	if (a == b)
	{
		int count = 0;
		for (int i = 0; format[i]; i++)
    		if (format[i] == '\n')
       		   count++;
		for (int i = 0; i < count; i++)
			printf("\e[F\e[J");
		for (int i = 0; i < count; i++)
			printf("\e[F\e[J");
		printf("\e[0;32mOK\e[0m [%d : %d]\n", a, b);
	}
	else
	{
		err = 1;
	 	printf("\e[0;31mKO\e[0m[%d : %d]\n", a, b);
	}
}

int	main(void)
{
	char *asd = "Hello World!";
	int x = 12359;
	char a = 'a';
	unsigned int t = 96;
	unsigned short int tt = 948;
	long double dd = 11112312.123123123;
	unsigned char xx = 22;
	unsigned long int  uli = 234;
	unsigned long long int ulli = 1242;
	signed char hh = a;
	short int h = -1234;
	long int l = 1234;
	long long int ll = -9223372036854775807;
	long long int lll = -9223372036854775807;
	long fgh = 123;
	long long tyu = 12345;
	long double asd1 = -3123.82312;
	double d = -11.1;

	check(TEST("2 |%c|\n", 'j'));
	check(TEST("s: %s %s\n", asd, asd));
	check(TEST("c: %c\n", a));
	check(TEST("d: %.d\n", x));	
	check(TEST("i: %i %i %i\n", 12, 012, 0x12));
	check(TEST("u: %u\n", t));
	check(TEST("o: %20.10o\n", x));
	check(TEST("x: %x\n", x));
	check(TEST("X: %20.10X\n", x));
	check(TEST("%% procent: %d\n", x)); 
	check(TEST("d: %+7.5d\n", -123));
	check(TEST("d: %+7.3d\n", 12345));
	check(TEST("d: %+5.3d\n", 1234567));
	check(TEST("d: %+5.7d\n", 123));
	check(TEST("d: %+3.7d\n", 12345));
	check(TEST("d: %+3.5d\n", 1234567));
	check(TEST("ld: %+7.5ld\n", fgh));
	check(TEST("lld: %+7.3lld\n", tyu));
	check(TEST("many: %d %i %5.3d%s %lx\n idk what more to test\n", 1, x, 1234567, asd, uli));
	check(TEST("Characters: %c %c \n", 'a', 65));
   	check(TEST("Decimals: %d %ld\n", 1977, 650000L));
    check(TEST("Preceding with blanks: %10d \n", 1977));
	check(TEST("Preceding with zeros: %010d \n", 1977));
    check(TEST("Some different radices: %d %x %o %#10.5xh %7.5o \n", 100, 100, 100, 100, 100));
    check(TEST("floats: %4.2f \n", 3.1416));
    check(TEST("Width trick: %*d \n", 5, 10));
    check(TEST("%s \n", "A string"));	
	check(TEST("o: %#o\n", x));
	check(TEST("x: %#x\n", x));
	check(TEST("X: %#X\n", x));
	check(TEST("o: %#20o\n", x));
	check(TEST("x: %#20x\n", x));
	check(TEST("X: %#20X\n", x));
	check(TEST("o: %#20.10o\n", x));
	check(TEST("x: %#20.10x\n", x));
	check(TEST("X: %#20.10X\n", x));
	check(TEST("f: %f\n", d));
	check(TEST(".14f: %.18f\n", d));
	check(TEST("+f: %+f\n", d));
	check(TEST("f: %.3f\n", d));
	check(TEST("f: %.18lf\n", d));
	check(TEST("s: %s\n", NULL));
	check(TEST("p3 %.4s\n", NULL));
	check(TEST("%.2s is a string\n", ""));
	check(TEST("%5.2s is a string\n", ""));
	check(TEST("%-5.2s is a string\n", ""));
	check(TEST("%#08x < Aidsssss\n", 42));
	//check(TEST("%#-08x\n", 42));
	check(TEST("%#8x\n", 42));
	check(TEST("Characters: %c %c \n", 'a', 65));
   	check(TEST("Decimals: %d %ld\n", 1977, 650000L));
    check(TEST("Some different radices: %d %x %o %#10.5xh %7.5o \n", 100, 100, 100, 100, 100));
    check(TEST("floats: %4.2f \n", 3.1416));
    check(TEST("Width trick: %*d \n", 5, 10));
	check(TEST("|%s|\n", "abc"));
	check(TEST("|%4.s|\n", "42"));
	check(TEST("|%4.1s|\n", "42"));
	check(TEST("|%4s|\n", "42"));
	check(TEST("|%.01s|\n", ""));
	check(TEST("|%.02s|\n", ""));
	check(TEST("|%2s|\n", ""));
	check(TEST("|%01.u|\n", 0));
	check(TEST("|%01.d|\n", 0));
	check(TEST("|%f|\n", -12547.58));
	//check(TEST("|%0.s|\n", "hi"));
	//check(TEST("|%---12s|\n", 0));
	check(TEST("{%#5o}\n", 1));
	check(TEST("{%#5o}\n", 0));
	check(TEST("{%5o}\n", 0));
	check(TEST("{%#.o, %#.0o}\n", 0, 0));
	check(TEST("{%.o, %.0o}\n", 0, 0));
	check(TEST("{%#.o, %#.0o}\n", 1, 1));
	check(TEST("{%.o, %.0o}\n", 1, 1));
	check(TEST("{%#05o, %#05o}\n", 1, 1));
	check(TEST("|%#7.5o|\n", 0xab));
	check(TEST("|%7.5o|\n", 0xab));
	check(TEST("|%#7.5o|\n", 0xab));
	check(TEST("{%#.5o}\n", 1));
	check(TEST("{%.5o}\n", 1));
	check(TEST("{%5.5o}\n", 1));
	check(TEST("{%#.5o}\n", 0));
	check(TEST("@moulitest: |%o|\n", 0));
	check(TEST("|%#o|\n", 0));
	check(TEST("|%#.o|\n", 0));
	check(TEST("|%#6o|\n", 2500));
	check(TEST("|%-#6o|\n", 2500));
	check(TEST("J'ai %i %s et %c yeux et 0.1%% de capacite cerebrale\n\n", 4, "dents", '2'));
	check(TEST("check(TEST: uint:	%u\n", 55555));
	check(TEST("check(TEST: hexa:	%x\n", 55555));
	check(TEST("check(TEST: hexa maj:	%X\n", 55555));
	check(TEST("check(TEST: octal:	%o\n", 55555));
	double nbd = 12345.123456789123456789123456789;
	check(TEST("|%-10.0lf|\n", nbd));
	check(TEST("|%-1.10lf|\n", nbd));
	check(TEST("|%3.2lf|\n", nbd));
	//check(TEST("|%-05lf|\n", nbd));
	check(TEST("|%09lf|\n", nbd));
	check(TEST("|%03lf|\n", nbd));
	check(TEST("|%-5.0lf|\n", nbd));
	check(TEST("|%#-5.0lf|\n", nbd));
	check(TEST("|%#-10.0lf|\n", nbd));
	check(TEST("|%#10.0lf|\n", nbd));
	check(TEST("|%#10.lf|\n", nbd));
	check(TEST("|%+#10.lf|\n", nbd));
	check(TEST("|%+-#10.lf|\n", nbd));
	check(TEST("|%+-#10.9lf|\n", nbd));
	check(TEST("|% -#10.9lf|\n", nbd));
	check(TEST("|% -#10.lf|\n", nbd));
	check(TEST("|% -#10.17lf|\n", nbd));
	nbd = -12547.58;
	check(TEST("|% f|\n", nbd));
	check(TEST("|%+f|\n", nbd));
	check(TEST("|%#f|\n", nbd));
	check(TEST("|%.2f|\n", nbd));
	check(TEST("|%.14f|\n", nbd));
	check(TEST("|%#.0f|\n", nbd));
	check(TEST("|% .5f|\n", nbd));
	check(TEST("|%# .0f|\n", nbd));
	check(TEST("|% #.0f|\n", nbd));
	check(TEST("|%+.5f|\n", nbd));
	check(TEST("|%+.0f|\n", nbd));
	check(TEST("|%#+.0f|\n", nbd));
	check(TEST("|%0.5f|\n", nbd));
	check(TEST("|%-.5f|\n", nbd));
	check(TEST("|%5f|\n", nbd));
	check(TEST("|% 5f|\n", nbd));
	check(TEST("|%+5f|\n", nbd));
	check(TEST("|%# 5f|\n", nbd));
	check(TEST("|%#+5f|\n", nbd));
	check(TEST("|%-5f|\n", nbd));
	check(TEST("|%05f|\n", nbd));
	check(TEST("|%+05f|\n", nbd));
	check(TEST("|%0+5f|\n", nbd));
	check(TEST("|%05.3f|\n", nbd));
	check(TEST("|%0#5.0f|\n", nbd));
	check(TEST("|%-5.3f|\n", nbd));
	check(TEST("|%-#5.0f|\n", nbd));
	check(TEST("|%+05.3f|\n", nbd));
	check(TEST("|%0+#5.0f|\n", nbd));
	check(TEST("|%0 5.3f|\n", nbd));
	check(TEST("|% 05.3f|\n", nbd));
	check(TEST("|%#0 5.0f|\n", nbd));
	check(TEST("|%-+5.3f|\n", nbd));
	check(TEST("|%#+5.0f|\n", nbd));
	check(TEST("{% 5.f}\n", 137.0));
	check(TEST("{%05.f}\n", 137.0));
	//check(TEST("{%-05.f}\n", 137.0));
	check(TEST("{%+5.f}\n", 137.0));
	check(TEST("{%+#5.f}\n", 137.0));
	check(TEST("{%-5.f}\n", 137.0));
	check(TEST("{%#5.f}\n", 137.0));
	check(TEST("{%5.f}\n", 137.0));
	check(TEST("{%5.f}\n", 0.0));
	check(TEST("{%#5f}\n", 1.0));
	check(TEST("{%#5f}\n", 0.0));
	check(TEST("{%5f}\n", 0.0));
	check(TEST("{%#.f, %#.0f}\n", 0.0, 0.0));
	check(TEST("{%.f, %.0f}\n", 0.0, 0.0));
	check(TEST("{%#.f, %#.0f}\n", 1.0, 1.0));
	check(TEST("{%.f, %.0f}\n", 1.0, 1.0));
	check(TEST("{%#05f, %#05f}\n", 1.0, 1.0));
	check(TEST("|%#07.5f|\n", 175.0));
	check(TEST("|%7.5f|\n", 175.0));
	check(TEST("{%#.5f}\n", 1.0));
	check(TEST("{%.5f}\n", 1.0));
	check(TEST("{%#.5f}\n", 0.0));
	check(TEST("{%.f}\n", 0.0));
	check(TEST("{%5f}\n", 1.5));
	check(TEST("{%.5f}\n", 1.5));
	check(TEST("{%5f}\n", 1500.5));
	check(TEST("{%.5f}\n", 1500.5));
	check(TEST("{%5.5f}\n", 0.0));
	check(TEST("2147483648.123450012: %.0f\n", 2147483648.123450012));
	check(TEST("0.01: %.15f\n", 0.01));
	check(TEST("0.0: %f\n", 0.0));
	check(TEST("1.0: %f\n", 1.0));
	check(TEST("01.01: %.4f\n", 01.01));
	check(TEST("0.00000000000000000001: %f\n", 0.00000000000000000001));
	check(TEST("0.1234567: %f\n", 0.1234567));
	check(TEST("0.1234564: %f\n", 0.1234564));
	check(TEST("-1.5: %.2f\n", -1.5));
	check(TEST("-0.05: %f\n", -0.05));

	if (err)
		printf("\e[0;31m\n---ERROR----\e[0m\n");
	else
		printf("\e[0;32m\n---PASSED---\e[0m\n");

	printf("\n----------ft_sprintf---------\n");
	char	s[PF_BUFF_SIZE];
	int		r;
	r = ft_sprintf(s, "many: %d %i %5.3d%s\n %lx idk what more to test", 1, x, 1234567, asd, uli);
	ft_printf("%s\t|%d|\n", s);
	r = ft_sprintf(s, "Characters: %c %c ", 'a', 65);
	ft_printf("%s\t|%d|\n", s);
   	r = ft_sprintf(s, "Decimals: %d %ld", 1977, 650000L);
	ft_printf("%s\t|%d|\n", s);
    r = ft_sprintf(s, "Some different radices: %d %x %o %#10.5xh %7.5o", 100, 100, 100, 100, 100);
	ft_printf("%s\t|%d|\n", s);
    r = ft_sprintf(s, "floats: %4.2f ", 3.1416);
	ft_printf("%s\t|%d|\n", s);
    r = ft_sprintf(s, "Width trick: %*d ", 5, 10);
	ft_printf("%s\t|%d|\n", s);

	printf("\n----------ft_asprintf---------\n");

	char *as;

	r = ft_asprintf(&as, "many: %d %i %5.3d%s\n %lx idk what more to test", 1, x, 1234567, asd, uli);
	ft_printf("%s\t|%d|\n", as, r);
	free(as);
	r = ft_asprintf(&as, "Characters: %c %c ", 'a', 65);
	ft_printf("%s\t|%d|\n", as, r);
	free(as);
   	r = ft_asprintf(&as, "Decimals: %d %ld", 1977, 650000L);
	ft_printf("%s\t|%d|\n", as, r);
	free(as);
    r = ft_asprintf(&as, "Some different radices: %d %x %o %#10.5xh %7.5o", 100, 100, 100, 100, 100);
	ft_printf("%s\t|%d|\n", as, r);
	free(as);
    r = ft_asprintf(&as, "floats: %4.2f ", 3.1416);
	ft_printf("%s\t|%d|\n", as, r);
	free(as);
    r = ft_asprintf(&as, "Width trick: %*d ", 5, 10);
	ft_printf("%s\t|%d|\n", as, r);
	free(as);

	printf("\n----------ft_snprintf---------\n");

	#define SN_BUFF 100
	char sn[SN_BUFF];

	r = ft_snprintf(sn, SN_BUFF, "many: %d %i %5.3d%s\n %lx idk what more to test", 1, x, 1234567, asd, uli);
	ft_printf("%s\t|%d|\n", sn, r);
	r = ft_snprintf(sn, SN_BUFF, "Characters: %c %c ", 'a', 65);
	ft_printf("%s\t|%d|\n", sn, r);
   	r = ft_snprintf(sn, SN_BUFF, "Decimals: %d %ld", 1977, 650000L);
	ft_printf("%s\t|%d|\n", sn, r);
    r = ft_snprintf(sn, SN_BUFF, "Some different radices: %d %x %o %#10.5xh %7.5o", 100, 100, 100, 100, 100);
	ft_printf("%s\t|%d|\n", sn, r);
    r = ft_snprintf(sn, SN_BUFF, "floats: %4.2f ", 3.1416);
	ft_printf("%s\t|%d|\n", sn, r);
    r = ft_snprintf(sn, SN_BUFF, "Width trick: %*d ", 5, 10);
	ft_printf("%s\t|%d|\n", sn, r);

	printf("\n----------------------\n");
	printf("sizeof(va_list) == %lu\n", sizeof(va_list));
	FILE *file = fopen("file_pointer", "w");
	r = ft_fprintf(file, "many: %d %i %5.3d%s %lx\n idk what more to test\n", 1, x, 1234567, asd, uli);
	printf("|%d|\n", r);
	fclose(file);
	int fd = open("file_descriptor", O_CREAT | O_WRONLY | O_TRUNC);
	r = ft_dprintf(fd, "many: %d %i %5.3d%s %lx\n idk what more to test\n", 1, x, 1234567, asd, uli);
	printf("|%d|\n", r);
	close(fd);
}
