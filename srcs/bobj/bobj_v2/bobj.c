/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:56:52 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/14 14:02:45 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bobj.h"

/*
 * Returns array of t_bobj;
 * Since one .obj can have multiple objects in it.
*/
void	bobj_load(t_bobj *bobj, char *file_path)
{
	// TODO : figure out what to do if you have multiple material files...
	/*
		read whole file

		read from buffer and store amount of o, v, vn, vt, f.

		malloc to the result bobj array amount of 'o'
		into those malloc the correct amount of the rest.
		
		start filling the arrays
	*/
}