#include "shaderpixel.h"
#include "font_manager.h"

/*
 * Inits freetype in to 'library';
 * With face in to 'face';
*/
void	init_freetype(FT_Library *library)
{
	int	error;

	error = FT_Init_FreeType(library);
	if (error)
		LG_ERROR("Couldn\'t init FreeType.");
}

/*
 * Opens font into 'face';
 *
*/
/*"C:/Windows/Fonts/Arial.ttf",*/
void	open_font(FT_Library library, FT_Face face, const char *font_path, int size)
{
	int	error;

	error = FT_New_Face(library, font_path, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
		LG_ERROR("Font was opened and read, but its format is unsupported.");
	else
		LG_ERROR("Couldn\'t open/read/load font.");
	// TODO : make the device resolution modular?;
//	error = FT_Set_Char_Size(*face, 0, size * 64, 0, 0);
	error = FT_Set_Pixel_Sizes(face, 0, size);
	if (error)
		LG_ERROR("Houston");
}

/*
 * TODO : instead of having those 3 'if' in the loop, just create in the beginning new coordinates that 100% are inside the bitmap dimensions;
 * Takes 'src' and places pixels on 'dst' starting at 'x', 'y';
*/
void	cpy_bitmap(t_bitmap *dst, FT_Bitmap *bitmap, int top_left_x, int top_left_y)
{
	Uint32	*dst_pixels;
	Uint32	*src_pixels;

	dst_pixels = (Uint32 *)dst->pixels;
	src_pixels = (Uint32 *)bitmap->buffer;
	for (int row = 0; row < bitmap->rows; row++)
	{
		for (int p = 0; p < bitmap->width; p++)
		{
			if (!(top_left_y >= 0 && top_left_y < dst->rows)) // break if y is not on dst bitmap;
				break ;
			if (top_left_x + p >= dst->width) // break if x is past the width of the dst bitmap;
				break ;
			if (top_left_x + p < 0) // but continue if it has not yet come to the left side on the x, since it can come there another iteration in the loop;
				continue ;
			dst_pixels[top_left_y * dst->pitch + top_left_x + p]
				= src_pixels[row * bitmap->pitch + p];
		}
	}
}

/*
 * TODO : Add support for multiple charsets;
 * charset : any of the enum FT_Encoding; FT_Select_Charmap();
 * TODO : Add support for multiple pixel bit modes (24bit & 32bit (more?));
*/
void	render_face(FT_Face face, char *str, t_bitmap *dst_bmp, int x, int y)
{
	FT_GlyphSlot	slot;
	Uint32			str_len;
	FT_UInt			glyph_index;
	int				error;
	int				curr_x;
	int				curr_y;

	slot = face->glyph;
	str_len = ft_strlen(str);
	curr_x = x;
	curr_y = y;
	for (Uint32 i = 0; i < str_len; i++)
	{
		glyph_index = FT_Get_Char_Index(face, str[i]);
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
			continue ;
		cpy_bitmap(dst_bmp, &slot->bitmap, curr_x + slot->bitmap_left, curr_y - slot->bitmap_top);

		curr_x += slot->advance.x >> 6;
		curr_y += slot->advance.y >> 6;
	}
}

/*
 * Init the font_manager into 'fm';
int		font_manager_init(t_font_manager *fm);

 * Returns index of face in 'font_faces';
int		new_font(t_fntmngr *fm, char *font_path, int font_size);

 * Returns pixel array where there is a text created with the font 'font';
 * 'font' : correspond to and index in the 'font_faces';
void	create_bitmap(t_fntmngr *fm, int font_index, int font_size);
*/