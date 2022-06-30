/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bobj.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/14 13:56:52 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/30 13:46:49 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "bobj.h"
#include "libft.h"
#include "liblg.h"
#include "libpf.h"

/*
 * Returns array of t_bobj;
 * Since one .obj can have multiple objects in it.
*/
void	bobj_load(t_bobj *bob, char *file_path)
{
	LG_INFO("Start");
	char	tmp[256];

	// Reset
	bob->objects = NULL;
	bob->objects_amount = 0;
	bob->materials = NULL;
	bob->materials_amount = 0;

	// Read whole file;
	char	*file_content;
	file_content = get_file_content(file_path);
	if (!file_content)
		LG_WARN("Couldn\'t read file <%s>", file_path);
	
	// Lets get the root dir, which we need for the material file;
	// And the texture files (if they are not in the same dir as the exe)
	int	len = ft_strlen(file_path);
	while (file_path[--len] != '/')
		;
	bob->root_dir = ft_strndup(file_path, len + 1);
# if BOBBUG
	ft_printf("root_dir : %s\n", bob->root_dir);
# endif

	// Get amounts;
	// Store index of starting locations for every v,vn,vt,f of the current object;
	// you can get the correct *_start with the index of o;
	int	v_start[16];
	int	v_amount[16];

	int	vn_start[16];
	int	vn_amount[16];

	int	vt_start[16];
	int	vt_amount[16];

	int	mesh_start[16][256]; // every o can have multiple meshi; mesh start where there is 'usemtl'
	int	mesh_amount[16];

	int	f_start[16];
	int	f_amount[16];

//	int	o_start[16];
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

//	o_start[o_amount] = 0;
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

			mesh_start[o_amount][0] = -1;
			mesh_amount[o_amount] = 0;

//			o_start[o_amount] = i;
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
			// This means the mesh doesnt have a material, aka we dont have 'usemtl' in the file;
			if (mesh_amount[o_amount - 1] == 0)
			{
				mesh_start[o_amount - 1][mesh_amount[o_amount - 1]] = i;
				++mesh_amount[o_amount - 1];
			}
			++f_amount[o_amount - 1];
			if (f_start[o_amount - 1] == -1)
				f_start[o_amount - 1] = i;
		}
		else if (file_content[i] == 'm')
		{
			i += get_next_word(tmp, file_content + i + 6);	
# if BOBBUG
			ft_printf("mtllib %s\n", tmp);
# endif
			bob->materials_amount = bobj_load_material(bob, tmp);
			if (bob->materials_amount <= 0)
			{
				char	tmp2[256];
				ft_strcpy(tmp2, bob->root_dir);
				ft_strcpy(tmp2 + ft_strlen(tmp2), tmp);
				bob->materials_amount = bobj_load_material(bob, tmp2);
			}
# if BOBBUG
			ft_printf("material amount : %d\n", bob->materials_amount);
# endif
		}
		else if (file_content[i] == 'u') // usemtl (new mesh)
		{
			mesh_start[o_amount - 1][++mesh_amount[o_amount - 1] - 1] = i;
		}
	}

# if BOBBUG
	ft_printf("o_amount : %d\n", o_amount);
	for (int i = 0; i < o_amount; i++)
	{
		ft_printf("\no #%d\n", i);
		ft_printf("\tv_start : %d, v_amount : %d\n", v_start[i], v_amount[i]);
		ft_printf("\tvt_start : %d, vt_amount : %d\n", vt_start[i], vt_amount[i]);
		ft_printf("\tvn_start : %d, vn_amount : %d\n", vn_start[i], vn_amount[i]);
		ft_printf("\tf_start : %d, f_amount : %d\n", f_start[i], f_amount[i]);
		ft_printf("\tmesh_amount : %d\n", mesh_amount[i]);
		for (int m = 0; m < mesh_amount[i]; m++)
			ft_printf("\t\tstart : %d\n", mesh_start[i][m]);
	}
# endif

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
	bob->objects = malloc(sizeof(t_bobj_object) * o_amount);

	// Then go through the 'file_content' again, starting at 'o_start[i]',
	//	and start filling the values;
	// NOTE : We dont care about duplicates;
	int		j;
	int		word;
	char	tmp_str[4][16];
	for (int o = 0; o < o_amount; o++)
	{

		int		tmp_v_amount = -1;
		int		tmp_vt_amount = -1;
		int		tmp_vn_amount = -1;
	
		// Getting vertices;
# if BOBBUG
		ft_printf("Vertices :\n");
# endif
		j = v_start[o] + 1;
		while (j > 0 && file_content[j] && ++tmp_v_amount < v_amount[o])
		{
			word = -1;
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			bobj_v3_new(&tmp_v[tmp_v_amount],
				ft_atof(tmp_str[0]), ft_atof(tmp_str[1]), ft_atof(tmp_str[2]));
# if BOBBUG
			ft_printf("\ttmp_v_amount %d / %d v_amount[%d] (%f %f %f)\n", tmp_v_amount, v_amount[o], o, tmp_v[tmp_v_amount].x, tmp_v[tmp_v_amount].y, tmp_v[tmp_v_amount].z);
# endif
			j += 2;
		}

		// Getting uvs
# if BOBBUG
		ft_printf("Uvs :\n");
# endif
		j = vt_start[o] + 2; // 'vt' is 2 characters, the same thing is taken into account in the end of this loop;
		while (j > 0 && file_content[j] && ++tmp_vt_amount < vt_amount[o])
		{
			word = -1;
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			bobj_v2_new(&tmp_vt[tmp_vt_amount],
				ft_atof(tmp_str[0]), ft_atof(tmp_str[1]));
# if BOBBUG
			ft_printf("\ttmp_vt_amount %d / %d vt_amount[%d] (%f %f)\n", tmp_vt_amount, vt_amount[o], o, tmp_vt[tmp_vt_amount].x, tmp_vt[tmp_vt_amount].y);
# endif
			j += 3;
		}

		// Getting normals
# if BOBBUG
		ft_printf("Normals :\n");
# endif
		j = vn_start[o] + 2;
		while (j > 0 && file_content[j] && ++tmp_vn_amount < vn_amount[o])
		{
			word = -1;
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			j += get_next_word(tmp_str[++word], file_content + j);
			bobj_v3_new(&tmp_vn[tmp_vn_amount],
				ft_atof(tmp_str[0]), ft_atof(tmp_str[1]), ft_atof(tmp_str[2]));
# if BOBBUG
			ft_printf("tmp_vn_amount %d / %d vn_amount[%d]\n", tmp_vn_amount, vn_amount[o], o);
# endif
			j += 3;
		}
# if BOBBUG
		ft_printf("#%d Done getting to tmp.\n", o);
# endif

		// Malloc the correct amount of vertices/uvs/normals;
		// NOTE : these are the maxiumum amounts of values, if we want to remove
		//		duplicates at some point, no need to adjust this, we can just
		//		realloc in the beginning (if we dont come up with better
		//		solution)
		bob->objects[o].v = malloc(sizeof(t_bobj_v3) * f_amount[o] * 3);
		bob->objects[o].vt = malloc(sizeof(t_bobj_v2) * f_amount[o] * 3);
		bob->objects[o].vn = malloc(sizeof(t_bobj_v3) * f_amount[o] * 3);
		bob->objects[o].v_amount = 0;
		bob->objects[o].vt_amount = 0;
		bob->objects[o].vn_amount = 0;

		// Mesh stuff here;
		bob->objects[o].meshes_amount = ft_max(mesh_amount[o], 1);
		bob->objects[o].meshes = malloc(sizeof(t_bobj_mesh) * bob->objects[o].meshes_amount);
		
		for (int m = 0; m < bob->objects[o].meshes_amount; m++)
		{
			if (mesh_start[o][m] < 0)
				LG_ERROR("WTF! mesh_start index is less than 0..");

			bob->objects[o].meshes[m].f = malloc(sizeof(t_bobj_u3) * f_amount[o]);
			bob->objects[o].meshes[m].f_amount = 0;
			bob->objects[o].meshes[m].index_amount = 0;

			j = mesh_start[o][m];
			bob->objects[o].meshes[m].material_index = -1;
			if (file_content[j] == 'u') // 'usemtl'
			{
				j += 6;
				j += get_next_word(tmp_str[0], file_content + j);
				bob->objects[o].meshes[m].material_index = get_material_index(bob, tmp_str[0]);
			}
# if BOBBUG
			LG_INFO("mesh mat index %d", bob->objects[o].meshes[m].material_index);
# endif
			while (j > 0 && file_content[j] && m < bob->objects[o].meshes_amount)
			{
				if (file_content[j - 1] != '\n' || file_content[j] != 'f')
				{
					++j;
					continue ;
				}
				j += 1;
				// TODO : here do the while 'get_next_word()' returns > 0
				//		(because a face can be more than 3 vertices in the object file);
				for (int i = 0; i < 3; i++)
				{
					j += get_next_word(tmp_str[i], file_content + j);

					// 0 : we have only vertex
					// 1 : we have vertex && uv
					// 2 : we have vertex && normals && uv
					int slash = 0;
					for (int c = 0; tmp_str[i][c]; c++)
						slash += tmp_str[i][c] == '/';
					
					// Decide which values the face point has;
					int	has_v = 1;
					int	has_vn = 1;
					int	has_vt = 1;

					int	v_index = -1;
					int	vt_index = -1;
					int	vn_index = -1;

					char	**arr;
					arr = ft_strsplit(tmp_str[i], '/');
					if (slash == 0 || (arr[2] == NULL && slash == 2))
						has_vt = 0;
					if (slash == 0 || slash == 1)
						has_vn = 0;

					// Get indices;
					if (has_v)
						v_index = ft_atoi(arr[0]);
					if (has_vt)
						vt_index = ft_atoi(arr[1]);
					if (has_vn)
					{
						if (has_vt)
							vn_index = ft_atoi(arr[2]);
						else
							vn_index = ft_atoi(arr[1]);
					}
					// Because .obj is stupid and thinks 1 is the first index;
					v_index -= 1;
					vt_index -= 1;
					vn_index -= 1;

					// Get the values from the tmp arrays, and place them in the object arrays;
					if (v_index >= 0)
					{
						bobj_v3_new(&bob->objects[o].v[bob->objects[o].v_amount],
							tmp_v[v_index].x, tmp_v[v_index].y, tmp_v[v_index].z);
						++bob->objects[o].v_amount;
					}
					if (vt_index >= 0)
					{
						bobj_v2_new(&bob->objects[o].vt[bob->objects[o].vt_amount],
							tmp_vt[vt_index].x, tmp_vt[vt_index].y);
						++bob->objects[o].vt_amount;
					}
					if (vn_index >= 0)
					{
						bobj_v3_new(&bob->objects[o].vn[bob->objects[o].vn_amount],
							tmp_vn[vn_index].x, tmp_vn[vn_index].y, tmp_vn[vn_index].z);
						++bob->objects[o].vn_amount;
					}

					// TODO : get_combination_index();
					if (i == 0)
						bob->objects[o].meshes[m].f[bob->objects[o].meshes[m].f_amount].i0 = bob->objects[o].meshes[m].index_amount;
					if (i == 1)
						bob->objects[o].meshes[m].f[bob->objects[o].meshes[m].f_amount].i1 = bob->objects[o].meshes[m].index_amount;
					if (i == 2)
						bob->objects[o].meshes[m].f[bob->objects[o].meshes[m].f_amount].i2 = bob->objects[o].meshes[m].index_amount;
					++bob->objects[o].meshes[m].index_amount;
# if BOBBUG
					ft_printf("s : %d, %d %d %d\n", slash, has_v, has_vt, has_vn);
					ft_printf("%d %d %d\n", v_index, vt_index, vn_index);
# endif

					ft_arraydel(arr);
				}
# if BOBBUG
				ft_printf("%d %d %d\n",
					bob->objects[o].meshes[m].f[bob->objects[o].meshes[m].f_amount].i0,
					bob->objects[o].meshes[m].f[bob->objects[o].meshes[m].f_amount].i1,
					bob->objects[o].meshes[m].f[bob->objects[o].meshes[m].f_amount].i2);
# endif
				++bob->objects[o].meshes[m].f_amount;
			}
		}

# if BOBBUG
		ft_printf("Amounts :: v : %d, vt : %d, vn : %d\n",
			bob->objects[o].v_amount,
			bob->objects[o].vt_amount,
			bob->objects[o].vn_amount
			);
# endif

		// Free the extra stuff if they never were used;
		if (bob->objects[o].vt_amount <= 0)
		{
			free(bob->objects[o].vt);
			bob->objects[o].vt = NULL;
		}
		if (bob->objects[o].vn_amount <= 0)
		{
			free(bob->objects[o].vn);
			bob->objects[o].vn = NULL;
		}
	}

	free(tmp_v);
	free(tmp_vt);
	free(tmp_vn);
	free(file_content);
	LG_INFO("End");
}

/*
 * NOTE: remember to have set the 'mats' to NULL;
 *
 * Returns material amount; so 0 if unsuccesfull;
*/
int	bobj_load_material(t_bobj *bob, char *file_path)
{
	char	*file_content;
	int		mat_amount;
	char	tmp[256];
	// Temps, where we are storing the pointers to the variable in mat struct;
	float	*value;
	float	*value_arr;
	char	**str_value;

	file_content = get_file_content(file_path);
	if (!file_content)
	{
		LG_WARN("Couldn\'t open <%s>", file_path);
		return (0);
	}
	LG_INFO("Success opening <%s>", file_path);

	// Get amounts of material;
	int i = -1;
	mat_amount = 0;
	while (file_content[++i])
	{
		if (i - 1 < 0 || file_content[i - 1] != '\n')
			continue ;
		if (file_content[i] == 'n') // newmtl;
			++mat_amount;
	}

	bob->materials = malloc(sizeof(t_bobj_material) * mat_amount);

	// Get the values for the materials;
	i = -1;
	int	curr_mat = -1;
	while (file_content[++i])
	{
		if (i - 1 < 0 || file_content[i - 1] != '\n')
			continue ;
		if (file_content[i] == 'n') // newmtl
		{
			++curr_mat;
			memset(&bob->materials[curr_mat], 0, sizeof(t_bobj_material));
			i += 6;
			i += get_next_word(tmp, file_content + i);
			bob->materials[curr_mat].name = ft_strdup(tmp);
		}
		else if (file_content[i] == 'N') // Ns/Ni;
		{
			if (file_content[i + 1] == 's')
				value = &bob->materials[curr_mat].Ns;
			else if (file_content[i + 1] == 'i')
				value = &bob->materials[curr_mat].Ni;
			i += 2; // skip 'N' + the other char;
			i += get_next_word(tmp, file_content + i);
			*value = ft_atof(tmp);
		}
		else if (file_content[i] == 'K') // Ka / Kd / Ks / Ke
		{
			if (file_content[i + 1] == 'a')
				value_arr = ((float *)(bob->materials[curr_mat].Ka));
			else if (file_content[i + 1] == 'd')
				value_arr = ((float *)(bob->materials[curr_mat].Kd));
			else if (file_content[i + 1] == 's')
				value_arr = ((float *)(bob->materials[curr_mat].Ks));
			else if (file_content[i + 1] == 'e')
				value_arr = ((float *)(bob->materials[curr_mat].Ke));

			i += 2; // skip 'K' + the other char;
			i += get_next_word(tmp, file_content + i);
			value_arr[0] = ft_atof(tmp);
			i += get_next_word(tmp, file_content + i);
			value_arr[1] = ft_atof(tmp);
			i += get_next_word(tmp, file_content + i);
			value_arr[2] = ft_atof(tmp);
		}
		else if (file_content[i] == 'd')
		{
			i += 1; // skip 'd'
			i += get_next_word(tmp, file_content + i);
			bob->materials[curr_mat].d = ft_atof(tmp);
		}
		else if (file_content[i] == 'i') // illum
		{
			i += 5; // skip 'illum'
			i += get_next_word(tmp, file_content + i);
			bob->materials[curr_mat].illum = ft_atoi(tmp);
		}
		else if (file_content[i] == 'm') // map_Kd / map_Bump / map_Ns
		{
			i += 4; // skip 'map_'
			if (file_content[i] == 'K' && file_content[i + 1] == 'd')
			{
				str_value = &bob->materials[curr_mat].map_Kd;
				i += 2;
			}
			else if (file_content[i] == 'B' && file_content[i + 1] == 'u')
			{
				str_value = &bob->materials[curr_mat].map_Bump;
				i += 4;
			}
			else if (file_content[i] == 'N' && file_content[i + 1] == 's')
			{
				str_value = &bob->materials[curr_mat].map_Ns;
				i += 2;
			}

			i += get_next_word(tmp, file_content + i);
			*str_value = ft_strdup(tmp);
		}
	}

	free(file_content);
# if BOBBUG
	bobj_material_print(&bob->materials[0]);

	LG_INFO("Done reading mat file");
# endif
	return (mat_amount);
}

void	bobj_free(t_bobj *bob)
{
	free(bob->root_dir);

	// bobject free;
	t_bobj_object	*bobject;
	t_bobj_mesh		*bobmesh;
	for (int b = 0; b < bob->objects_amount; b++)
	{
		bobject = &bob->objects[b];

		if (bobject->name)
			free(bobject->name);
		if (bobject->v)
			free(bobject->v);
		if (bobject->vt)
			free(bobject->vt);
		if (bobject->vn)
			free(bobject->vn);

		bobject->name = NULL;
		bobject->v = NULL;
		bobject->vt = NULL;
		bobject->vn = NULL;

		// free Mesh
		for (int m = 0; m < bobject->meshes_amount; m++)
		{
			bobmesh = &bobject->meshes[m];
			free(bobmesh->f);
			bobmesh->f = NULL;
		}
		free(bobject->meshes);
		bobject->meshes = NULL;
	}

	// free material
	t_bobj_material	*boberial;
	for (int m = 0; m < bob->materials_amount; m++)
	{
		boberial = &bob->materials[m];
		if (boberial->name)
			free(boberial->name);
		if (boberial->map_Bump)
			free(boberial->map_Bump);
		if (boberial->map_Kd)
			free(boberial->map_Kd);
		if (boberial->map_Ns)
			free(boberial->map_Ns);
		if (boberial->refl)
			free(boberial->refl);
		
		boberial->name = NULL;
		boberial->map_Bump = NULL;
		boberial->map_Kd = NULL;
		boberial->map_Ns = NULL;
		boberial->refl = NULL;
	}
}

/*
 * Returns index of 't_bobj_material' with the name 'name';
*/
int	get_material_index(t_bobj *bob, char *name)
{
	int	i = 0;

	if (!bob || !bob->materials || bob->materials_amount <= 0)
		return (-1);
	while (i < bob->materials_amount)
	{
		if (ft_strequ(bob->materials[i].name, name))
			return (i);
		++i;
	}
	return (-1);
}

void	bobj_material_print(t_bobj_material *mat)
{
	ft_printf("name : %s\n", mat->name);
	ft_printf("Ns : %f\n", mat->Ns);
	ft_printf("Ka : %f %f %f\n", mat->Ka[0], mat->Ka[1], mat->Ka[2]);
	ft_printf("Kd : %f %f %f\n", mat->Kd[0], mat->Kd[1], mat->Kd[2]);
	ft_printf("Ks : %f %f %f\n", mat->Ks[0], mat->Ks[1], mat->Ks[2]);
	ft_printf("Ke : %f %f %f\n", mat->Ke[0], mat->Ke[1], mat->Ke[2]);
	ft_printf("Ni : %f\n", mat->Ni);
	ft_printf("d : %f\n", mat->d);
	ft_printf("illum : %d\n", mat->illum);
	ft_printf("map_Kd : %s\n", mat->map_Kd);
	ft_printf("map_Bump : %s\n", mat->map_Bump);
	ft_printf("map_Ns : %s\n", mat->map_Ns);
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

void	bobj_u3_new(t_bobj_u3 *u3, unsigned int x, unsigned int y, unsigned int z)
{
	u3->i0 = x;
	u3->i1 = y;
	u3->i2 = z;
}