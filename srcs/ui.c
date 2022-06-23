/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/23 10:30:34 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/23 12:24:40 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ui.h"

/// Init ////

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

void	init_inventory(t_ui *ui)
{
	t_bimgf				tex;
	t_bimgf_settings	img_set;

	img_set.flip = 1;
	bimgf_load_bmp(&tex, TEXTURE_PATH"inventory.bmp", img_set);
	bitmap_duplicate(&ui->inventory_bmp, tex.pixels, tex.w, tex.h);
	bimgf_free(&tex);
	ui->inventory_slot = -1;
	ui->inventory_open = 0;
}

/// DRAW ////

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
	/*
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
	*/
	// SELECTED
	hotbar_pos[2] = slot_dim[0];
	hotbar_pos[0] = hotbar_pos[0] + (ui->selected_hotbar * hotbar_pos[2]);
	ui_draw_bitmap(ui->manager, hotbar_pos, &ui->hotbar_select_bmp);
}

/*
 * hotbar : 0 - 8
 * inventory : starting from top left, 9 - 35 inclusive;
*/
void	draw_inventory(t_ui *ui)
{
	GLint	viewport[4];
	float	highlight_pos[4];
	float	inventory_pos[4];
	float	orig_ratio;

	glGetIntegerv(GL_VIEWPORT, viewport);

	orig_ratio = ((float)ui->inventory_bmp.height / (float)ui->inventory_bmp.width);
	inventory_pos[2] = viewport[2] / 4 * 3;
	inventory_pos[3] = viewport[3] * orig_ratio;
	inventory_pos[0] = viewport[2] / 2 - inventory_pos[2] / 2;
	inventory_pos[1] = viewport[3] / 2 - inventory_pos[3] / 2;

	ui_draw_bitmap(ui->manager, inventory_pos, &ui->inventory_bmp);
	
	if (point_in_rectangle((float []){ui->mouse_x, ui->mouse_y}, inventory_pos))
	{
		float	ratio_w = ui->inventory_bmp.width / inventory_pos[2];
		float	ratio_h = ui->inventory_bmp.height / inventory_pos[3];
		int		x_offset = 8;
		int		y_offset = 8;

		highlight_pos[2] = 16 / ratio_w;
		highlight_pos[3] = 16 / ratio_h;

		// Inventory slot;
		ui->inventory_slot = -1;
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				highlight_pos[0] = (i * 16 + i * 2 + x_offset) / ratio_w + inventory_pos[0];
				highlight_pos[1] = (j * 16 + j * 2 + y_offset) / ratio_h + inventory_pos[1];
				if (point_in_rectangle((float []){ui->mouse_x, ui->mouse_y}, highlight_pos))
					ui->inventory_slot = j * 9 + i;
			}
		}
		// hotbar slot;
		if (ui->inventory_slot == -1)
		{
			highlight_pos[1] = 66 / ratio_h + inventory_pos[1];
			for (int i = 0; i < 9; i++)
			{
				highlight_pos[0] = (i * 16 + i * 2 + x_offset) / ratio_w + inventory_pos[0];
				if (point_in_rectangle((float []){ui->mouse_x, ui->mouse_y}, highlight_pos))
					ui->inventory_slot = 3 * 9 + i;
			}
				
		}

		if (ui->inventory_slot != -1)
		{
			int	inv_x = ui->inventory_slot % 9;
			int	inv_y = ui->inventory_slot / 9;

			highlight_pos[0] = (inv_x * 16 + inv_x * 2 + x_offset) / ratio_w + inventory_pos[0];
			if (ui->inventory_slot >= 0 && ui->inventory_slot <= 26)
				highlight_pos[1] = (inv_y * 16 + inv_y * 2 + y_offset) / ratio_h + inventory_pos[1];
			else
				highlight_pos[1] = 66 / ratio_h + inventory_pos[1];

			ui_draw_filled_rect(ui->manager, highlight_pos, (Uint8 []){200, 200, 255, 200});
		}
	}
}

/////////////////////////////

void	ui_init(t_ui *ui)
{
	t_bimgf				tex;
	t_bimgf_settings	img_set;

	ui->mouse_x = 0;
	ui->mouse_y = 0;

	img_set.flip = 1;
	img_set.rgb = 1;
	bimgf_load_bmp(&tex, TEXTURE_PATH"version_3_texture_alpha.bmp", img_set);
	bitmap_duplicate(&ui->block_texture_bmp, tex.pixels, tex.w, tex.h);
	bimgf_free(&tex);

	init_hotbar(ui);
	init_inventory(ui);
}

void	ui_update(t_ui *ui, GLFWwindow *win)
{
	glfwGetCursorPos(win, &ui->mouse_x, &ui->mouse_y);
}

void	ui_draw(t_ui *ui)
{
	draw_hotbar(ui);

	if (ui->inventory_open)
		draw_inventory(ui);
}
