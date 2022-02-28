#include "bimgf.h"

int main(void)
{
	t_bimgf	img;
	ft_timer_start();
	if (!bimgf_load(&img, "/Users/jsalmi/Downloads/seagull1.jpeg"))
		LG_ERROR("Errorskiduudle.");
	printf("time : %f\n", ft_timer_end());
	return (0);
}
