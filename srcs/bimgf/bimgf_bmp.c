#include "bimgf.h"

typedef struct s_bmp_file_header
{
    uint16_t	type;
    uint32_t	size;
    uint16_t	reserved1;
    uint16_t	reserved2;
    uint32_t	off_bits;
}				t_bmp_file_header;

typedef struct s_bmp_info_header
{
    uint32_t	size;
    int32_t		width;
    int32_t		height;
    uint16_t	planes;
    uint16_t	bit_count;
    uint32_t	compression;
    uint32_t	size_image;
    uint32_t	x_pels_permeter;
    uint32_t	y_pels_permeter;
    uint32_t	clr_used;
    uint32_t	clr_important;
}				t_bmp_info_header;

void	show_header(t_bmp_file_header file_header) 
{
	printf("[BMP Header]\n");
	printf("file type: %x\n", file_header.type);
	printf("BMP file size: %dkb\n", file_header.size / 1024);
	printf("Reserved words must be 0: %d\n",  file_header.reserved1);
	printf("Reserved words must be 0: %d\n",  file_header.reserved2);
	printf("Offset bytes of actual bitmap data: %d\n",  file_header.off_bits);
	printf("\n");
}

void	show_info_header(t_bmp_info_header info_header) 
{
	printf("[BMP Information Header]\n");
	printf("The size of the message header: %d\n" ,info_header.size);
	printf("Bitmap width: %d\n" ,info_header.width);
	printf("Bitmap height: %d\n" ,info_header.height);
	printf("The number of planes of the image (the number of planes is the number of palettes, the default is 1 palette): %d\n" ,info_header.planes);
	printf("Number of bits per pixel: %d\n" ,info_header.bit_count);
	printf("Compression method:%d\n" ,info_header.compression);
	printf("Image size: %d\n" ,info_header.size_image);
	printf("Horizontal resolution: %d\n" ,info_header.x_pels_permeter);
	printf("Vertical resolution: %d\n" ,info_header.y_pels_permeter);
	printf("Number of colors used: %d\n" ,info_header.clr_used);
	printf("Number of important colors: %d\n" ,info_header.clr_important);
	printf("\n");   
}

void	read_bmp_header(const char *image_path)
{
	t_bmp_file_header	file_header;
	t_bmp_info_header	info_header;
	FILE* fp; 
	fp = fopen(image_path, "rb");
	if(fp == NULL)
	{
		printf("Failed to open '%s'!\n", image_path);
		return ;
	}
	fread(&file_header, 1, sizeof(t_bmp_file_header) - 2, fp);
	show_header(file_header);
	fread(&info_header, 1, sizeof(t_bmp_info_header), fp);
	show_info_header(info_header);
	fclose(fp);
}

void	get_header(t_bmp_file_header *header, FILE *fd)
{
	size_t	total_read = 0;

	total_read += fread(&header->type, 1, sizeof(uint16_t), fd);
	total_read += fread(&header->size, 1, sizeof(uint32_t), fd);
	total_read += fread(&header->reserved1, 1, sizeof(uint16_t), fd);
	total_read += fread(&header->reserved2, 1, sizeof(uint16_t), fd);
	total_read += fread(&header->off_bits, 1, sizeof(uint32_t), fd);
	//printf("header : total_read : %d == %d\n", total_read, 14);
}

void	get_info_header(t_bmp_info_header *info, FILE *fd)
{
	size_t	total_read = 0;
	total_read += fread(&info->size, 1, sizeof(uint32_t), fd);
	total_read += fread(&info->width, 1, sizeof(int32_t), fd);
	total_read += fread(&info->height, 1, sizeof(int32_t), fd);
	total_read += fread(&info->planes, 1, sizeof(uint16_t), fd);
	total_read += fread(&info->bit_count, 1, sizeof(uint16_t), fd);
	total_read += fread(&info->compression, 1, sizeof(uint32_t), fd);
	total_read += fread(&info->size_image, 1, sizeof(uint32_t), fd);
	total_read += fread(&info->x_pels_permeter, 1, sizeof(uint32_t), fd);
	total_read += fread(&info->y_pels_permeter, 1, sizeof(uint32_t), fd);
	total_read += fread(&info->clr_used, 1, sizeof(uint32_t), fd);
	total_read += fread(&info->clr_important, 1, sizeof(uint32_t), fd);
	//printf("info header : total_read : %d == %d\n", total_read, info->size);
}

/*
 * Returns 1/0 if the loading was successful;
*/
int	bimgf_load_bmp(t_bimgf *image, const char *file_path)
{
	FILE				*fd;
	t_bmp_file_header	header;
	t_bmp_info_header	info;
	int					skip_bytes;
	char				tmp;

	if (!file_path)
	{
		printf("[%s] No file path given.\n", __FUNCTION__);
		return (0);
	}
	fd = fopen(file_path, "rb");
	if (!fd)
	{
		printf("[%s] Couldnt open image path: %s\n", __FUNCTION__, file_path);
		return (0);
	}
	get_header(&header, fd);
	//show_header(header);
	if (header.type != 0x4d42)
	{
		fclose(fd);
		return (0);
	}
	get_info_header(&info, fd);
	//show_info_header(info);
	memset(image, 0, sizeof(t_bimgf));

	image->bpp = info.bit_count / 8; // bytes per pixel
	image->line_w = (info.width * image->bpp);

	int	extra = image->line_w % 4;
	skip_bytes = 0;
	if (extra != 0)
		skip_bytes = 4 - extra;

	image->w = info.width;
	image->h = info.height;
	image->size = info.size_image;
	image->pixels = malloc(image->size);
	if (image->bpp == 4)
		image->format = BIMGF_RGBA;
	else
		image->format = BIMGF_RGB;
	image->flip = 0;
	image->rgb = 1;

	if (image->flip == 0)
	{
		int	y;

		y = 0;
		while (y < image->h)
		{
			fread(&image->pixels[y * image->line_w], image->w, image->bpp, fd);
			if (skip_bytes > 0)
				fread(&tmp, 1, skip_bytes, fd);
			y++;
		}
	}
	else if (image->flip == 1)
	{
		int	y;

		y = image->h - 1;
		while (y >= 0)
		{
			fread(&image->pixels[y * image->line_w], image->w, image->bpp, fd);
			if (skip_bytes > 0)
				fread(&tmp, 1, skip_bytes, fd);
			y--;
		}
	}

	if (image->rgb) // convert to RGB // or RGBA
	{
//		image->format = 1;
		char			temp;
		unsigned char	*ptr;

		for (int y = 0; y < image->h; y++)
		{
			ptr = image->pixels + y * image->line_w;
			for (int x = image->w; x > 0; x--, ptr += image->bpp)
			{
				temp = ptr[0];
				ptr[0] = ptr[2];
				ptr[2] = temp;
			}
		}
	}
	fclose(fd);
	return (1);
}

void	bimgf_free(t_bimgf *image)
{
	free(image->pixels);
	//free(image); // if its malloced the user has to free it;
}
