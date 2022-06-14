/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:50:57 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/14 14:11:45 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOBJ_H
# define BOBJ_H

/*
*/
typedef struct s_bobj_mesh
{
	char			*name;

	float			*v;
	int				v_amount;

	float			*vt;
	int				vt_amount;

	float			*vn;
	int				vn_amount;

	unsigned int	*f;
	int				f_amount;

	char			*usemtl;
}	t_bobj_mesh;

/*
 * 'name' : newmtl;
 * all the other ones have the same name as in the 'mtl' file;
*/
typedef struct s_bobj_material
{
	char	*name;
	float	Ns;
	float	Ka[3];
	float	Kd[3];
	float	Ks[3];
	float	Ke[3];
	float	Ni;
	float	d;
	int		illum;
	char	*map_Kd;
}	t_bobj_material;

/*
 * 'name' : 'o';
*/
typedef struct s_bobj_object
{
	char			*name;

	t_bobj_mesh		*meshes;
	int				meshes_amount;

	t_bobj_material	*materials;
	int				materials_amount;
}		t_bobj_object;

typedef struct s_bobj
{
	t_bobj_object	*objects;
	int				objects_amount;
	// maybe store materials here;
}		t_bobj;

/*
	t_bobj : one file;
	t_bobj_object : 'o';
	t_bobj_material : 'mtllib';
	t_bobj_mesh : 'usemtl';
	indices : 'f';
	vertices : 'v';
	normals : 'vn';
	uvs : 'vt';
*/

void	bobj_load(t_bobj *bobj, char *file_path);

#endif