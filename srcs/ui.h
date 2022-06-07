/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ui.h                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/24 13:14:06 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/07 16:25:30 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UI_H
# define UI_H

# include "shaderpixel.h"
# include "bitmap.h"
# include "ui_manager.h"
# include "block.h"

typedef struct s_ui
{
	t_ui_manager	*manager;

	t_bitmap	block_texture_bmp;

	t_bitmap	hotbar_bmp;
	t_bitmap	hotbar_select_bmp;
	Uint8		selected_hotbar;
	int			*hotbar_item_id; // pointer to the t_player_info hotbar_item_id;

}	t_ui;

void	ui_init(t_ui *ui);
void	ui_draw(t_ui *ui);

#endif
