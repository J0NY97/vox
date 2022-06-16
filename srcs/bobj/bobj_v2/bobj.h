/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:50:57 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/16 12:18:33jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BOBJ_H
# define BOBJ_H

# define BOBBUG 0

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
 * 'material_index' : index of one of the materials in 't_bobj->materials';
*/
typedef struct s_bobj_mesh
{
	t_bobj_u3		*f;
	unsigned int	index_amount; // how many indices in total aka f_amount * 3 since every face has 3 indices each;
	unsigned int	f_amount; // how many 'f' we have aka faces; every face has 3 indices;

	int				material_index;
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
	char	*map_Bump;
	char	*map_Kd;
	char	*map_Ns;
	char	*refl;
}	t_bobj_material;

/*
 * 'name' : 'o';
*/
typedef struct s_bobj_object
{
	char			*name;

	t_bobj_v3		*v;
	int				v_amount;
	t_bobj_v2		*vt;
	int				vt_amount;
	t_bobj_v3		*vn;
	int				vn_amount;
	
	t_bobj_mesh		*meshes;
	int				meshes_amount;
}		t_bobj_object;

typedef struct s_bobj
{
	char			*root_dir;

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

void	bobj_load(t_bobj *bob, char *file_path);
int		bobj_load_material(t_bobj *bob, char *file_path);

// Help
int		get_next_word(char *result_str, char *src_str);

void	bobj_v3_new(t_bobj_v3 *v3, float x, float y, float z);
void	bobj_v2_new(t_bobj_v2 *v2, float x, float y);
void	bobj_u3_new(t_bobj_u3 *u3, unsigned int x, unsigned int y, unsigned int z);

#endif