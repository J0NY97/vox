/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 12:05:37 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 12:06:46 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef KEY_H
# define KEY_H

///////////////////
//	KEYS
///////////////////

enum e_mouse_button_state
{
	BUTTON_PRESS, // the framei t was pressed;
	BUTTON_HOLD, // if held for more than a frame;
	BUTTON_RELEASE, // the frame it was released;
	BUTTON_NONE // if not one of the above (aka not pressed?);
};

typedef struct	s_key
{
	int		state;
}	t_key;

void		update_all_keys(t_key *keys, t_key *mouse, GLFWwindow *win);

#endif