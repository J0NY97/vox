/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:56:52 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/15 15:12:32 by jsalmi           ###   ########.fr       */
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
	char	tmp[256];
	char	root_dir[256];

	// Reset
	bobj->objects = NULL;
	bobj->objects_amount = 0;
	bobj->materials = NULL;
	bobj->materials_amount = 0;

	// Read whole file;
	char	*file_content;
	file_content = get_file_content(file_path);
	if (!file_content)
		LG_WARN("Couldn\'t read file <%s>", file_path);
	
	// Lets get the root dir, which we need for the material file;
	int	len = ft_strlen(file_path);
	ft_strcpy(root_dir, file_path);
	while (root_dir[--len] != '/')
		root_dir[len] = 0;
	
	ft_printf("root : %s\n", root_dir);

	// Get amounts;
	// Store index of starting locations for every v,vn,vt,f of the current object;
	// you can get the correct *_start with the index of o;
	int	v_start[16];
	int	v_amount[16];

	int	vn_start[16];
	int	vn_amount[16];

	int	vt_start[16];
	int	vt_amount[16];

	int	material_index[16][256]; // every mesh can have a material;

	int	mesh_start[16][256]; // every o can have multiple meshi;
	int	mesh_amount[16];

	int	f_start[16];
	int	f_amount[16];

	int	o_start[16];
	int	o_amount = 0;
	int	i = -1;

	v_start[o_amount] = -1;
	v_amount[o_amount] = 0;

	vn_start[o_amount] = -1;
	vn_amount[o_amount] = 0;

	vt_start[o_amount] = -1;
	vt_amount[o_amount] = 0;

	f_start[o_amount] = -1;
	f_amount[o_amount] = 0;

	o_start[o_amount] = 0;
	while (file_content[i++])
	{
		// Continue if not newline, we dont care about the values for right now;
		if (i - 1 < 0 || file_content[i - 1] != '\n')
			continue ;
		if (file_content[i] == 'o')
		{
			v_start[o_amount] = -1;
			v_amount[o_amount] = 0;

			vn_start[o_amount] = -1;
			vn_amount[o_amount] = 0;

			vt_start[o_amount] = -1;
			vt_amount[o_amount] = 0;

			f_start[o_amount] = -1;
			f_amount[o_amount] = 0;

			material_index[o_amount][0] = -1;
			mesh_start[o_amount][0] = -1;
			mesh_amount[o_amount] = 1;

			o_start[o_amount] = i;
			o_amount++;
		}
		else if (file_content[i] == 'v')
		{
			o_amount = ft_max(o_amount, 1);
			if (file_content[i + 1] == 't')
			{
				++vt_amount[o_amount - 1];
				if (vt_start[o_amount - 1] == -1)
					vt_start[o_amount - 1] = i;
			}
			else if (file_content[i + 1] == 'n')
			{
				++vn_amount[o_amount - 1];
				if (vn_start[o_amount - 1] == -1)
					vn_start[o_amount - 1] = i;
			}
			else
			{
				++v_amount[o_amount - 1];
				if (v_start[o_amount - 1] == -1)
					v_start[o_amount - 1] = i;
			}
		}
		else if (file_content[i] == 'f')
		{
			++f_amount[o_amount - 1];
			if (f_start[o_amount - 1] == -1)
				f_start[o_amount - 1] = i;
		}
		else if (file_content[i] == 'm')
		{
			i += get_next_word(tmp, file_content + i + 6);	
			ft_printf("mtllib %s\n", tmp);
			bobj->materials_amount = bobj_load_material(&bobj->materials, tmp);
			if (bobj->materials_amount <= 0)
			{
				char	tmp2[256];
				ft_strcpy(tmp2, root_dir);
				ft_strcpy(tmp2 + ft_strlen(tmp2), tmp);
				bobj->materials_amount = bobj_load_material(&bobj->materials, tmp2);
			}
			ft_printf("material amount : %d\n", bobj->materials_amount);
		}
		else if (file_content[i] == 'u') // usemtl
		{
			i += get_next_word(tmp, file_content + i + 6);
			// get material index with name;
			ft_printf("usemtl %s\n", tmp);
		}
	}

	ft_printf("o_amount : %d\n", o_amount);
	for (int i = 0; i < o_amount; i++)
	{
		ft_printf("\no #%d\n", i);
		ft_printf("\tv_start : %d, v_amount : %d\n", v_start[i], v_amount[i]);
		ft_printf("\tvt_start : %d, vt_amount : %d\n", vt_start[i], vt_amount[i]);
		ft_printf("\tvn_start : %d, vn_amount : %d\n", vn_start[i], vn_amount[i]);
		ft_printf("\tf_start : %d, f_amount : %d\n", f_start[i], f_amount[i]);
	}

	// Get the most amount of v/vt/vn, malloc that amount into tmp;
	int	most_v = 0;
	int	most_vt = 0;
	int	most_vn = 0;
	for (int i = 0; i < o_amount; i++)
	{
		if (v_amount[i] > most_v)
			most_v = v_amount[i];
		if (vt_amount[i] > most_vt)
			most_vt = vt_amount[i];
		if (vn_amount[i] > most_vn)
			most_vn = vn_amount[i];
	}
	t_bobj_v3	*tmp_v = malloc(sizeof(t_bobj_v3) * most_v);
	t_bobj_v2	*tmp_vt = malloc(sizeof(t_bobj_v2) * most_vt);
	t_bobj_v3	*tmp_vn = malloc(sizeof(t_bobj_v3) * most_vn);
	// All object files dont have a 'o' in the file, but they should still;
	//	work the same;
	bobj->objects = malloc(sizeof(t_bobj_object) * o_amount);

	// Then go through the 'file_content' again, starting at 'o_start[i]',
	//	and start filling the values;
	// NOTE : We dont care about duplicates;
	int		j;
	int		word;
	char	tmp_str[4][16];
	for (int i = 0; i < o_amount; i++)
	{

		int		tmp_v_amount = -1;
		int		tmp_vt_amount = -1;
		int		tmp_vn_amount = -1;
	
		// Getting vertices;
		ft_printf("Vertices :\n");
		j = v_start[i] + 1;
		while (j > 0 && file_content[j] && ++tmp_v_amount < v_amount[i])
		{
			word = -1;
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			bobj_v3_new(&tmp_v[tmp_v_amount],
				ft_atof(tmp_str[0]), ft_atof(tmp_str[1]), ft_atof(tmp_str[2]));
			ft_printf("\ttmp_v_amount %d / %d v_amount[%d] (%f %f %f)\n", tmp_v_amount, v_amount[i], i, tmp_v[tmp_v_amount].x, tmp_v[tmp_v_amount].y, tmp_v[tmp_v_amount].z);
			j += 2;
		}

		// Getting uvs
		ft_printf("Uvs :\n");
		j = vt_start[i] + 2; // 'vt' is 2 characters, the same thing is taken into account in the end of this loop;
		while (j > 0 && file_content[j] && ++tmp_vt_amount < vt_amount[i])
		{
			word = -1;
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			bobj_v2_new(&tmp_vt[tmp_vt_amount],
				ft_atof(tmp_str[0]), ft_atof(tmp_str[1]));
			ft_printf("\ttmp_vt_amount %d / %d vt_amount[%d] (%f %f)\n", tmp_vt_amount, vt_amount[i], i, tmp_vt[tmp_vt_amount].x, tmp_vt[tmp_vt_amount].y);
			j += 3;
		}

		// Getting normals
		ft_printf("Normals :\n");
		j = vn_start[i] + 2;
		while (j > 0 && file_content[j] && ++tmp_vn_amount < vn_amount[i])
		{
			word = -1;
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			bobj_v3_new(&tmp_vn[tmp_vn_amount],
				ft_atof(tmp_str[0]), ft_atof(tmp_str[1]), ft_atof(tmp_str[2]));
			ft_printf("tmp_vn_amount %d / %d vn_amount[%d]\n", tmp_vn_amount, vn_amount[i], i);
			j += 3;
		}
		ft_printf("#%d Done getting to tmp.\n", i);

		// Malloc the correct amount of vertices/uvs/normals;
		bobj->objects[i].v = malloc(sizeof(t_bobj_v3) * f_amount[i] * 3);
		bobj->objects[i].vt = malloc(sizeof(t_bobj_v2) * f_amount[i] * 3);
		bobj->objects[i].vn = malloc(sizeof(t_bobj_v3) * f_amount[i] * 3);

		// TODO : mesh stuff here;
	}
	ft_printf("All Done.\n");

	free(tmp_v);
	free(tmp_vt);
	free(tmp_vn);
	free(file_content);
	LG_INFO("End");
	exit(0);
}

/*
 * NOTE: remember to have set the 'mats' to NULL;
 *
 * Returns material amount; so 0 if unsuccesfull;
*/
int	bobj_load_material(t_bobj_material *mats, char *file_path)
{
	char	*file_content;
	int		mat_amount;
	char	tmp[256];

	file_content = get_file_content(file_path);
	if (!file_content)
	{
		LG_WARN("Couldn\'t open <%s>", file_path);
		return (0);
	}
	LG_INFO("Success opening <%s>", file_path);

	// Get amounts of material;
	int i = 0;
	mat_amount = 0;
	while (file_content[++i])
	{
		if (i - 1 < 0 || file_content[i - 1] != '\n')
			continue ;
		if (file_content[i] == 'n') // newmtl;
			++mat_amount;
	}

	mats = malloc(sizeof(t_bobj_material) * mat_amount);

	// Get the values for the materials;
	i = 0;
	int	curr_mat = 0;
	int	move = 0;
	while (file_content[++i])
	{
		if (i - 1 < 0 || file_content[i - 1] != '\n')
			continue ;
		if (file_content[i] == 'n') // newmtl
			++curr_mat;
		else if (file_content[i] == 'N') // Ns/Ni;
		{
			move = get_next_word(tmp, file_content + i + 2);
			write(1, file_content + i, 5);
			ft_putstr(tmp);
			if (file_content[i + 1] == 's')
				mats[curr_mat].Ns = ft_atof(tmp);
			else if (file_content[i + 1] == 'i')
				mats[curr_mat].Ni = ft_atof(tmp);
			else
				LG_WARN("This value is not supported <%c%c>", file_content[i], file_content[i + 1]);
			i += move;
		}
	}

	free(file_content);
	return (mat_amount);
}

/*
 * 'result_str' : should already be allocated;
 *
 * Returns how many characters we moved forward;
*/
int	get_next_word(char *result_str, char *src_str)
{
	int	move = 0;
	int	c = -1;

	while (src_str[++move])
	{
		if (ft_isspace(src_str[move]))
		{
			result_str[++c] = 0;
			return (move);
		}
		result_str[++c] = src_str[move];
	}
	return (move);
}

void	bobj_v3_new(t_bobj_v3 *v3, float x, float y, float z)
{
	v3->x = x;
	v3->y = y;
	v3->z = z;
}

void	bobj_v2_new(t_bobj_v2 *v2, float x, float y)
{
	v2->x = x;
	v2->y = y;
}