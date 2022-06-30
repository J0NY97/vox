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
	for (unsigned int i = 0; i < bmp->pixel_amount; i++)
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
 * NOTE: both 'out' and 'rect' should be already allocated;
 *
 * Creates a xywh rect that fits for sure inside w/h;
*/
void	get_forsure(int *out, int *rect, int w, int h)
{
	out[0] = rect[0];
	out[1] = rect[1];
	out[2] = rect[2];
	out[3] = rect[3];
	// adjust x
	if (rect[0] < 0)
	{
		out[0] = 0;
		out[2] = rect[2] + rect[0];
	}
	// adjust y
	if (rect[1] < 0)
	{
		out[1] = 0;
		out[3] = rect[3] + rect[1];
	}
	// adjust w
	if (rect[2] >= w)
		out[2] = w - 1;
	// adjust h
	if (rect[3] >= h)
		out[3] = h - 1;
	
	if (out[2] <= 0 || out[3] <= 0)
	{
		out[2] = 1;	
		out[3] = 1;	
	}
}

/*
 * From 'src' take dimensions 'rect' and create a new bitmap into 'dst';
 *
 * 'rect' : xywh;
 * 
 * Instead of checking every loop if the coordinates are inside src,
 *  we can create a new rect that is for sure inside;
*/
void	bitmap_duplicate_rect(t_bitmap *dst, t_bitmap *src, int *rect)
{
	Uint32	*dst_pix;
	Uint32	*src_pix;
	int		forsure[4];

	get_forsure(forsure, rect, src->width, src->height);
	bitmap_new(dst, forsure[2], forsure[3]);
	dst_pix = dst->pixels;
	src_pix = src->pixels;
	for (int y = 0; y < forsure[3]; y++)
	{
		for (int x = 0; x < forsure[2]; x++)
		{
			dst_pix[y * dst->width + x] = src_pix[(y + forsure[1]) * src->width + (x + forsure[0])];
		}
	}
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
	for (unsigned int i = 0; i < bmp->pixel_amount; i++)
		dst[i] = src[i];
}

/*
void	bitmap_print(t_bitmap *bmp)
{
	Uint32	*pixels;

	pixels = (Uint32 *)bmp->pixels;
	for (unsigned int i = 0; i < bmp->height; i++)
	{
		for (unsigned int j = 0; j < bmp->width; j++)
		{
			ft_printf("0x%08x ", pixels[i * bmp->width + j]);
		}
		ft_printf("\n");
	}
}
*/