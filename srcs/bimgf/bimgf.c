#include "bimgf.h"

int	bimgf_load(t_bimgf *image, const char *file_path)
{
	if (strendswith(file_path, ".bmp"))
		return (bimgf_load_bmp(image, file_path));
	else if (strendswith(file_path, ".jpg") || strendswith(file_path, ".jpeg"))
		return (bimgf_load_jpg(image, file_path));
	else
		printf("[%s] File type is not supported. <%s>\n",
			__FUNCTION__, file_path);
	return (0);
}