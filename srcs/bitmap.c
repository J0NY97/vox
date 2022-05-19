#include "bitmap.h"

void	new_bitmap(t_bitmap *bmp, int w, int h)
{
	memset(bmp, 0, sizeof(t_bitmap));
	bmp->rows = h;
	bmp->width = w;
	bmp->pitch = sizeof(Uint32) * bmp->width;
	bmp->pixels = malloc(bmp->pitch * bmp->rows);
}