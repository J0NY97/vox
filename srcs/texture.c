#include "shaderpixel.h"

int	new_texture(GLuint *texture, const char *image_file)
{
	t_bimgf	image;
	char	buffer[256];
	int		orig_img_format = GL_RGB;

	if (!bimgf_load(&image, image_file))
	{	
		LG_WARN("Couldnt load image <%s>", image_file);
		return (0);
	}
	glBindTexture(GL_TEXTURE_2D, *texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.w, image.h,
		0, orig_img_format, GL_UNSIGNED_BYTE, image.pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	bimgf_free(&image);
	return (1);
}