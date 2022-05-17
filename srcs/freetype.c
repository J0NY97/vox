#include "shaderpixel.h"

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
 * Takes 'src' and places pixels on 'dst' starting at 'x', 'y';
*/
void	cpy_bitmap(Uint32 *dst, FT_Bitmap bitmap, int top_left_x, int top_left_y)
{
	
}

/*
 * TODO : Add support for multiple charsets;
 * charset : any of the enum FT_Encoding; FT_Select_Charmap();
*/
void	render_face(FT_Face face, char *str, Uint32 *dst_pixels, int x, int y)
{
	FT_GlyphSlot	slot;
	Uint32			str_len;
	FT_Uint			glyph_index;
	int				error;

	slot = face->glyph;
	str_len = ft_strlen(str);
	for (int i = 0; i < str_len; i++)
	{
		glyph_index = FT_Get_Char_Index(face, str[i]);
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
			continue ;
		cpy_bitmap(dst_pixels, &slot->bitmap, x + slot->bitmap_left, y - slot->bitmap_top);

		x += slot->advance.x >> 6;
		y += slot->advance.y >> 6;
	}
}

typedef struct	s_fntmngr
{
	FT_Library	library;
	FT_Face		*font_faces;	//	array of font faces;
}	t_fntmngr;
/*
 * Returns index of face in 'font_faces';
int		new_font(t_fntmngr *fm, char *font_path, int font_size);

 * Returns pixel array where there is a text created with the font 'font';
 * 'font' : correspond to and index in the 'font_faces';
void	create_bitmap(t_fntmngr *fm, int font_index, int font_size);
*/