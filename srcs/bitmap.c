#include "bitmap.h"

void	bitmap_new(t_bitmap *bmp, int w, int h)
{
	memset(bmp, 0, sizeof(t_bitmap));
	bmp->height = h;
	bmp->width = w;
	bmp->pitch = sizeof(Uint32) * bmp->width;
	bmp->pixel_amount = bmp->width * bmp->height;
	bmp->pixels = malloc(bmp->pitch * bmp->height);
}

void	bitmap_fill(t_bitmap *bmp, Uint32 col)
{
	Uint32	*pix;

	pix = (Uint32 *)bmp->pixels;
	for (int i = 0; i < bmp->pixel_amount; i++)
		pix[i] = col;
}