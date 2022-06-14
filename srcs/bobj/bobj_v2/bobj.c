/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:56:52 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/14 14:58:33 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bobj.h"
#include "libft.h"
#include "liblg.h"

/*
 * Returns array of t_bobj;
 * Since one .obj can have multiple objects in it.
*/
void	bobj_load(t_bobj *bobj, char *file_path)
{
	LG_INFO("Start");

	// Read whole file;
	char	*file_content;
	file_content = get_file_content(file_path);
	if (!file_content)
		LG_WARN("Couldn\'t read file <%s>", file_path);

	// Get amounts;
	// Store index of starting locations for every v,vn,vt,f of the current object;
	// you can get the correct *_start with the index of o;
	int	v_start[256];
	int	vn_start[256];
	int	vt_start[256];
	int	f_start[256];
	int	o_start[256];
	int	o_amount = 0;
	int	i = -1;
	v_start[o_amount] = -1;
	vn_start[o_amount] = -1;
	vt_start[o_amount] = -1;
	f_start[o_amount] = -1;
	o_start[o_amount] = 0;
	while (file_content[i++])
	{
		// Continue if not newline, we dont care about the values for right now;
		if (!(i - 1 <= 0 || file_content[i - 1] == '\n'))
			continue ;
		if (file_content[i] == 'o')
		{
			v_start[o_amount] = -1;
			vn_start[o_amount] = -1;
			vt_start[o_amount] = -1;
			f_start[o_amount] = -1;
			o_start[o_amount] = i;
			o_amount++;
		}
		if (file_content[i] == 'v')
		{
			o_amount = ft_max(o_amount, 1);
			if (file_content[i + 1] == 't')
			{
				if (vt_start[o_amount - 1] == -1)
					vt_start[o_amount - 1] = i;
			}
			else if (file_content[i + 1] == 'n')
			{
				if (vn_start[o_amount - 1] == -1)
					vn_start[o_amount - 1] = i;
			}
			else
			{
				if (v_start[o_amount - 1] == -1)
					v_start[o_amount - 1] = i;
			}
		}
		else if (file_content[i] == 'f')
			f_start[o_amount - 1] = i;
	}

	// All object files dont have a single 'o' in the file, but should still
	//	work the same;
	bobj->objects = malloc(sizeof(t_bobj_object) * o_amount);
	

	LG_INFO("End");
}