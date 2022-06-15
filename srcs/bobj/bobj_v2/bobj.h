/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:50:57 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/15 14:39:35 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOBJ_H
# define BOBJ_H

typedef struct	s_bobj_v3
{
	float	x;
	float	y;
	float	z;
}			t_bobj_v3;

typedef struct	s_bobj_v2
{
	float	x;
	float	y;
}			t_bobj_v2;

typedef struct	s_bobj_u3
{
	unsigned int	i0;
	unsigned int	i1;
	unsigned int	i2;
}			t_bobj_u3;

/*
*/
typedef struct s_bobj_mesh
{
	char			*name;

	t_bobj_u3		*f;
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

	t_bobj_v3		*v;
	int				v_amount;
	t_bobj_v3		*vt;
	int				vt_amount;
	t_bobj_v3		*vn;
	int				vn_amount;
	
	t_bobj_mesh		*meshes;
	int				meshes_amount;
}		t_bobj_object;

typedef struct s_bobj
{
	t_bobj_object	*objects;
	int				objects_amount;

	t_bobj_material	*materials;
	int				materials_amount;
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

// Help
int		get_next_word(char *result_str, char *src_str);

void	bobj_v3_new(t_bobj_v3 *v3, float x, float y, float z);
void	bobj_v2_new(t_bobj_v2 *v2, float x, float y);

#endif