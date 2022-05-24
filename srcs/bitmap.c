#include "bitmap.h"

void	bitmap_new(t_bitmap *bmp, int w, int h)
{
	memset(bmp, 0, sizeof(t_bitmap));
	bmp->height = h;
	bmp->width = w;
	bmp->bpp = 4;
	bmp->pitch = 4 * bmp->width;
	bmp->pixel_amount = bmp->width * bmp->height;
	bmp->pixels = malloc(bmp->pitch * bmp->height);
}

void	bitmap_free(t_bitmap *bmp)
{
	free(bmp->pixels);
}

void	bitmap_fill(t_bitmap *bmp, Uint32 col)
{
	Uint32	rgba;
	Uint32	*pix;

	rgba = ((col & 0xFF000000) >> 24) |
		((col & 0x00FF0000) >> 8) |
		((col & 0x0000FF00) << 8) |
		((col & 0x000000FF) << 24);
	pix = (Uint32 *)bmp->pixels;
	for (int i = 0; i < bmp->pixel_amount; i++)
		pix[i] = rgba;
}

void	bitmap_set_pixel(t_bitmap *bmp, int x, int y, Uint32 col)
{
	((Uint32 *)bmp->pixels)[y * bmp->width + x] = col;
}

void	bitmap_duplicate(t_bitmap *bmp, void *pixels, int w, int h)
{
	bitmap_new(bmp, w, h);
	bitmap_copy(bmp, pixels);
}

/*
 * The 'bmp' should already be created with 'bitmap_new()';
 * So width / height / bpp should all be the same for 'bmp' and 'pixels';
*/
void	bitmap_copy(t_bitmap *bmp, void *pixels)
{
	Uint32	*dst;
	Uint32	*src;

	dst = (Uint32 *)bmp->pixels;
	src = (Uint32 *)pixels;
	for (int i = 0; i < bmp->pixel_amount; i++)
		dst[i] = src[i];
}

void	bitmap_print(t_bitmap *bmp)
{
	Uint32	*pixels;

	pixels = (Uint32 *)bmp->pixels;
	for (int i = 0; i < bmp->height; i++)
	{
		for (int j = 0; j < bmp->width; j++)
		{
			ft_printf("0x%08x ", pixels[i * bmp->width + j]);
		}
		ft_printf("\n");
	}
}