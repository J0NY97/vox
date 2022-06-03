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
	t_bitmap	bmp;
	GLint		viewport[4];
	float		hotbar_pos[4];	// xywh
	float		slot_dim[2];	// wh
	float		item_dim[2];	// wh
	float		name_pos[4];
	float		orig_ratio;

	glGetIntegerv(GL_VIEWPORT, viewport);

	// HOTBAR
	orig_ratio = ((float)ui->hotbar_bmp.height / (float)ui->hotbar_bmp.width);
	hotbar_pos[2] = viewport[2] / 3;
	hotbar_pos[3] = hotbar_pos[2] * orig_ratio;
	hotbar_pos[0] = viewport[2] / 2 - hotbar_pos[2] / 2;
	hotbar_pos[1] = viewport[3] - hotbar_pos[3] - viewport[3] / 24;
	ui_draw_bitmap(ui->manager, hotbar_pos, &ui->hotbar_bmp);

	// Getting slot dimensions;
	slot_dim[0] = hotbar_pos[2] / 9;
	slot_dim[1] = hotbar_pos[3];

	// ITEMS
	int	texture_amount_w = 24;
	int	texture_amount_h = 15;
	int	total_textures = texture_amount_w * texture_amount_h;
	item_dim[0] = slot_dim[0] * 0.50f;
	item_dim[1] = slot_dim[1] * 0.50f;
	float w = (slot_dim[0] - item_dim[0]) / 2;
	float h = (slot_dim[1] - item_dim[1]) / 2;
	for (int i = 0; i < 9; i++)
	{
		if (ui->hotbar_item_id[i] == -1)
			continue ;
		int	tex_id = g_block_data[ui->hotbar_item_id[i]].texture[0];
		int x = /*texture_amount_w -*/ (tex_id % texture_amount_w);
		int y = texture_amount_h - (tex_id / texture_amount_w);
		if (x >= texture_amount_w || y >= texture_amount_h)
			continue ;
		bitmap_duplicate_rect(&bmp, &ui->block_texture_bmp,
			(int []){x * 16, y * 16, 16, 16});
		ui_draw_bitmap(ui->manager,
			(float []){hotbar_pos[0] + (i * slot_dim[0]) + w, hotbar_pos[1] + h,
				item_dim[0], item_dim[1]}, &bmp);
		bitmap_free(&bmp);
	}

	if (ui->selected_hotbar < 0 || ui->selected_hotbar > 9)
		return ;

	// Selected block name;
	fm_render_text(&bmp, &ui->manager->font_manager, 0,
		g_block_data[ui->hotbar_item_id[ui->selected_hotbar]].readable_name,
		0xffffffff, 0x00);
	orig_ratio = ((float)bmp.height / (float)bmp.width);
	name_pos[3] = hotbar_pos[3] * 0.5f;
	name_pos[2] = name_pos[3] / orig_ratio;
	name_pos[0] = hotbar_pos[0] + hotbar_pos[2] / 2 - name_pos[2] / 2;
	name_pos[1] = hotbar_pos[1] - bmp.height - (name_pos[3] * 0.5f);
	ui_draw_bitmap(ui->manager, name_pos, &bmp);
	bitmap_free(&bmp);
	// SELECTED
	hotbar_pos[2] = slot_dim[0];
	hotbar_pos[0] = hotbar_pos[0] + (ui->selected_hotbar * hotbar_pos[2]);
	ui_draw_bitmap(ui->manager, hotbar_pos, &ui->hotbar_select_bmp);
}

/////////////////////////////

void	ui_init(t_ui *ui)
{
	t_bimgf				tex;
	t_bimgf_settings	img_set;

	img_set.flip = 1;
	img_set.rgb = 1;
	bimgf_load_bmp(&tex, MODEL_PATH"cube/version_3_texture_alpha.bmp", img_set);
	bitmap_duplicate(&ui->block_texture_bmp, tex.pixels, tex.w, tex.h);
	bimgf_free(&tex);

	init_hotbar(ui);
}

void	ui_draw(t_ui *ui)
{
	draw_hotbar(ui);
}

