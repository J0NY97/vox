#include "ui.h"

void	init_hotbar(t_ui *ui)
{
	t_bimgf	tex;

	bimgf_load(&tex, TEXTURE_PATH"hotbar.bmp");
	bitmap_duplicate(&ui->hotbar_bmp, tex.pixels, tex.w, tex.h);
	bimgf_free(&tex);
	bimgf_load(&tex, TEXTURE_PATH"hotbar_select.bmp");
	bitmap_duplicate(&ui->hotbar_select_bmp, tex.pixels, tex.w, tex.h);
	bimgf_free(&tex);
	ui->selected_hotbar = 0;
}

void	draw_hotbar(t_ui *ui)
{
	GLint	viewport[4];
	float	hotbar_pos[4];
	float	orig_ratio;

	glGetIntegerv(GL_VIEWPORT, viewport);
	orig_ratio = ((float)ui->hotbar_bmp.height / (float)ui->hotbar_bmp.width);
	hotbar_pos[2] = viewport[2] / 3;
	hotbar_pos[3] = hotbar_pos[2] * orig_ratio;
	hotbar_pos[0] = viewport[2] / 2 - hotbar_pos[2] / 2;
	hotbar_pos[1] = viewport[3] - hotbar_pos[3] - viewport[3] / 24;
	ui_draw_bitmap(ui->manager, hotbar_pos, &ui->hotbar_bmp);

	hotbar_pos[2] = hotbar_pos[2] / 9;
	hotbar_pos[0] = hotbar_pos[0] + (ui->selected_hotbar * hotbar_pos[2]);
	ui_draw_bitmap(ui->manager, hotbar_pos, &ui->hotbar_select_bmp);
}

/////////////////////////////

void	ui_init(t_ui *ui)
{
	init_hotbar(ui);
}

void	ui_draw(t_ui *ui)
{
	draw_hotbar(ui);
}

