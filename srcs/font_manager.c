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

void	set_font_size(FT_Face face, int size)
{
	int	error;

	// TODO : make the device resolution modular?;
//	error = FT_Set_Char_Size(face, 0, size * 64, 0, 0);
	error = FT_Set_Pixel_Sizes(face, 0, size);
	if (error)
		LG_ERROR("Houston");
}

/*
 * Opens font into 'face';
 *
*/
/*"C:/Windows/Fonts/arial.ttf",*/
void	open_font(FT_Library library, FT_Face *face, const char *font_path)
{
	int	error;

	error = FT_New_Face(library, font_path, 0, face);
	print_face(*face);
	if (error == FT_Err_Unknown_File_Format)
		LG_ERROR("Font was opened and read, but its format is unsupported.");
	else if (error)
		LG_ERROR("Couldn\'t open/read/load font.");
	// Default font size of 12;
	set_font_size(*face, 12);
}

/*
 * TODO : instead of having those 3 'if' in the loop, just create in the beginning new coordinates that 100% are inside the bitmap dimensions;
 * Takes 'src' and places pixels on 'dst' starting at 'x', 'y';
*/
void	cpy_bitmap(t_bitmap *dst, FT_Bitmap *bitmap, int top_left_x, int top_left_y)
{
	Uint32	*dst_pixels;
	Uint8	*src_pixels;
	Uint32	col = 0xff111111; // abgr;

	dst_pixels = (Uint32 *)dst->pixels;
	src_pixels = bitmap->buffer;
	ft_printf("bmp : %d %d\n", bitmap->rows, bitmap->width);
	for (int row = 0; row < bitmap->rows; row++)
	{
		for (int p = 0; p < bitmap->width; p++)
		{
			int y = ((row + top_left_y) * dst->width);
			int x = top_left_x + p;
			int dst_ind = y + x;
			int src_ind = row * bitmap->width + p;
			/*
			ft_printf("(%d %d) %d %d, %d\n",top_left_x + p, top_left_y + row, x,y,dst_ind);
			ft_printf("total bytes : %d\n", dst->bpp * dst->pixel_amount);
			*/
//			col = (col & 0x00ffffff) | (src_pixels[src_ind] << 24);
			ft_printf("0x%08x\n", col);
			dst_pixels[dst_ind] = col;
		}
	//	ft_printf("Didnt crash.\n");
	}
}

/*
 * TODO : Add support for multiple charsets; charset : any of the enum FT_Encoding; FT_Select_Charmap();
 * TODO : Add support for multiple pixel bit modes (24bit & 32bit (more?)); for t_bitmap;
 * TODO : check that font_index is inside the array;
 * TODO : Load all glyphs first, not loading duplicates. Then copy to bitmap; This way you can get the bounding box for the text also;
 * 
 * You get 'font_index' by calling 'font_manager_get_font()';
*/
t_bitmap	*fm_render_text(t_font_manager *fm, int font_index, char *str, Uint32 text_color, Uint32 bg_color)
{
	t_bitmap		*bmp;
	FT_Face			face;
	FT_GlyphSlot	slot;
	FT_UInt			glyph_index;
	Uint32			str_len;
	int				error;
	int				curr_x;
	int				curr_y;

	face = fm->font_faces[font_index];
	bmp = malloc(sizeof(t_bitmap));
	slot = face->glyph;
	str_len = ft_strlen(str);
	bitmap_new(bmp, face->size->metrics.max_advance / 64 * str_len, face->size->metrics.height / 64);
//	bitmap_fill(bmp, bg_color);
	ft_printf("%d %d\n", bmp->width, bmp->height);
	ft_printf("strlen : %d\n", str_len);
	curr_x = 0;
	curr_y = bmp->height;
	for (Uint32 i = 0; i < str_len; i++)
	{
		glyph_index = FT_Get_Char_Index(face, str[i]);
		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
		{
			ft_printf("[FT_Load_Glyph](%s).", FT_Error_String(error));
			continue ;
		}
		error = FT_Render_Glyph(slot, FT_RENDER_MODE_MONO);
		if (error)
		{
			ft_printf("[FT_Render_Glyph](%s).", FT_Error_String(error));
			continue ;
		}
		cpy_bitmap(bmp, &slot->bitmap, curr_x + slot->bitmap_left, curr_y - slot->bitmap_top);

		curr_x += slot->advance.x >> 6;
	//	curr_y += slot->advance.y >> 6;
	}
	return (bmp);
}

void	print_face(FT_Face face)
{
	ft_printf("num_faces : %d\n", face->num_faces);
	ft_printf("face_index : %d\n", face->face_index);
	ft_printf("face_flags : %d\n", face->face_flags);
	ft_printf("style_flags : %d\n", face->style_flags);
	ft_printf("num_glyphs : %d\n", face->num_glyphs);
	ft_printf("family_name : %s\n", face->family_name);
	ft_printf("style_name : %s\n", face->style_name);
	ft_printf("num_fixed_sizes : %d\n", face->num_fixed_sizes);
	ft_printf("num_charmaps : %d\n", face->num_charmaps);
}

/*
 * Init the font_manager into 'fm';
*/
void	font_manager_init(t_font_manager *fm)
{
	init_freetype(&fm->library);
	fm->font_faces = NULL;
	fm->font_face_amount = 0;
	fm->font_faces_allocated = 0;
}

/*
 * Gets font from font manager and returns the index of it;
 * If font doesnt we open the font and try again;
 * IF error we return -1;
 * 
 * Returns index of face in 'font_faces';
*/
int	font_manager_get_font(t_font_manager *fm, char *font_path, int font_size)
{
	// Search for face in the already opened faces;
	// If we havent returned a face yet, we can asssume the face hasnt been opened;
	// Lets open the face;
	if (fm->font_face_amount + 1 > fm->font_faces_allocated)
	{
		LG_WARN("Font faces reallocated (%d => %d)", fm->font_faces_allocated, fm->font_faces_allocated + 1);
		fm->font_faces_allocated += 1;
		fm->font_faces = realloc(fm->font_faces, sizeof(FT_Face) * fm->font_faces_allocated);
	}
	open_font(fm->library, &fm->font_faces[fm->font_face_amount], font_path);
	// IF the current selected font size is not the one requested;
	// We check if the font has that size;
	// if not, we create
	// if has, we change it to that;

	LG_INFO("Success opening font.");
	++fm->font_face_amount;
	return (fm->font_face_amount - 1);
}

/*
 * Returns pixel array where there is a text created with the font 'font';
 * 'font' : correspond to and index in the 'font_faces';
void	create_bitmap(t_fntmngr *fm, int font_index, int font_size);
*/