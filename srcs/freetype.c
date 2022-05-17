#include "shaderpixel.h"

/*
 * Inits freetype in to 'library';
*/
void	init_freetype(FT_Library *library)
{
	int	error;

	error = FT_Init_FreeType(library);
	if (error)
		LG_ERROR("Couldn\'t init FreeType.");
}