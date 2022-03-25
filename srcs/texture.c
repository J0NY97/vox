#include "shaderpixel.h"

int	new_texture(GLuint *texture, const char *image_file)
{
	t_bimgf	image;
	char	buffer[256];
	int		orig_img_format;

	if (!bimgf_load(&image, image_file))
	{	
		LG_WARN("Couldnt load image <%s>", image_file);
		return (0);
	}
	if (image.format == BIMGF_RGBA)
		orig_img_format = GL_RGBA;
	else
		orig_img_format = GL_RGB;
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// NOTE: GL_NEAREST looks pixelated when you have hd textures;
	// TODO: At some point have so you can change these modes in a modular way;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.w, image.h,
		0, orig_img_format, GL_UNSIGNED_BYTE, image.pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	bimgf_free(&image);
	return (1);
}