#ifndef BIMGF_H
# define BIMGF_H
# include "stdio.h"
# include "stdlib.h"
# include "string.h"
# include "stdint.h"
# include "math.h"
# include "pthread.h"

# include "liblg.h"
# include "huffman.h"

# define BEDUGGER 1

enum e_bimgf_formats
{
	BIMGF_RGB,
	BIMGF_RGBA,
	BIMGF_BGR,
	BIMGF_ABGR
};

typedef struct s_bimgf
{
	unsigned char	*pixels;
	int				w; // pixels
	int				h; // pixels
	unsigned int	size; // in bytes
	unsigned int	line_w; // in bytes
	unsigned int	bpp; // bits per pixel
	unsigned int	format; // one of e_bimgf_formats
	unsigned int	flip; // 1 if you want top left to be 0, 0 other wise bot left is
	unsigned int	rgb; // 1 if you want to convert from bgr to rgb 
}					t_bimgf;

int					bimgf_load(t_bimgf *image, const char *file_path);
void				bimgf_free(t_bimgf *image);
void				read_bmp_header(const char *image_path);

int					bimgf_load_bmp(t_bimgf *image, const char *file_path);
int					bimgf_load_jpg(t_bimgf *image, const char *file_path);

// Help
int					strendswith(char const *src, char const *comp);

#endif
