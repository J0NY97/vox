#include "bobj_loader.h"

void	element_builder(t_obj *obj, char **arr);

char	*remove_file_name(const char *file)
{
	int	len = ft_strlen(file);
	int	temp = len;
	while (--temp > 0)
		if (file[temp] == '/')
			break ;
	char *str = ft_strndup(file, temp + 1);	
	return (str);
}

int	obj_load(t_obj *obj, const char *obj_file_path)
{
	FILE	*fd;
	char	**arr;
	int	mesh_found = 0;
	int	usemtl_found = 0;
	t_mesh	*curr_mesh = NULL;
	t_element	*curr_elem = NULL;

	LG_INFO("Reading <%s>", obj_file_path);
	memset(obj, 0, sizeof(t_obj));

	obj->root_path = remove_file_name(obj_file_path);

	obj->name = ft_strdup(obj_file_path);
	obj->obj_vert_value_amount = 3;
	obj->obj_norm_value_amount = 3;
	obj->obj_uv_value_amount = 2;
	obj->mesh_amount = 0;

	// Creating default mesh;
	obj->meshes = malloc(sizeof(t_mesh));
	curr_mesh = &obj->meshes[obj->mesh_amount];
	memset(curr_mesh, 0, sizeof(t_mesh));
	curr_mesh->vertex_value_amount = 3;
	curr_mesh->normal_value_amount = 3;
	curr_mesh->uv_value_amount = 2;

	// Creating default mesh element;
	curr_mesh->element_amount = 0;
	curr_mesh->elements = malloc(sizeof(t_element));
	curr_elem = &curr_mesh->elements[curr_mesh->element_amount];
	memset(curr_elem, 0, sizeof(t_element));
	curr_elem->index_value_amount = 3;
	usemtl_found = 0;

	char	*file_content;
	char	line[256];
	LG_DEBUG("Reading whole file.");
	file_content = get_file_content(obj_file_path);
	LG_DEBUG("Whole file read.");
	//lines = ft_strsplit(file_content, '\n'); // TOOO SLOW!
	int amount = get_elem_amount(file_content, '\n') * 2;
	LG_DEBUG("moutn got.");
	int *elem_pos = malloc(sizeof(int) * amount); // remember to free;
	LG_DEBUG("loc.");
	fill_elem_pos(elem_pos, file_content, '\n');
	LG_DEBUG("Splat.");
	//ft_strdel(&file_content); // Add this to the end;
	LG_DEBUG("Dill DAll DOll.");
	int ding = 0;

	////////////
	// Lets Count all V / VN / VT in the file and malloc that much;
	////////////
	int uv_amount = 0;
	int norm_amount = 0;
	int vert_amount = 0;

	while (ding < amount)
	{
		ft_strncpy(line, &file_content[elem_pos[ding]], elem_pos[ding + 1]);
		line[elem_pos[ding + 1]] = 0;
		if (line[0] == 'v') // vertices, uvs, normals
		{
			if (line[1] == 't') // uvs
				uv_amount++;
			else if (line[1] == 'n') // normals
				norm_amount++;
			else // vertices
				vert_amount++;
		}

		ding += 2;
		if (ding % 100000 == 0)
			putchar('-');
	}
	if (uv_amount > 0)
		obj->obj_uvs = malloc(sizeof(float) * (uv_amount * obj->obj_uv_value_amount));
	if (norm_amount > 0)
		obj->obj_norms = malloc(sizeof(float) * (norm_amount * obj->obj_norm_value_amount));
	if (vert_amount > 0)
		obj->obj_verts = malloc(sizeof(float) * (vert_amount * obj->obj_vert_value_amount));

	LG_DEBUG("Obj value mallocs done.");

	ding = 0;

	while (ding < amount)
	{
		ft_strncpy(line, &file_content[elem_pos[ding]], elem_pos[ding + 1]);
		line[elem_pos[ding + 1]] = 0;

		arr = ft_strsplit(line, ' ');	
		if (line[0] == 'v') // vertices, uvs, normals
		{
			if (line[1] == 't') // uvs
			{
				obj->obj_uv_amount++;
				obj->obj_uvs[(obj->obj_uv_amount - 1) * obj->obj_uv_value_amount + 0] = atof(arr[1]);
				obj->obj_uvs[(obj->obj_uv_amount - 1) * obj->obj_uv_value_amount + 1] = atof(arr[2]);
			}
			else if (line[1] == 'n') // normals
			{
				obj->obj_norm_amount++;
				obj->obj_norms[(obj->obj_norm_amount - 1) * obj->obj_norm_value_amount + 0] = atof(arr[1]);
				obj->obj_norms[(obj->obj_norm_amount - 1) * obj->obj_norm_value_amount + 1] = atof(arr[2]);
				obj->obj_norms[(obj->obj_norm_amount - 1) * obj->obj_norm_value_amount + 2] = atof(arr[3]);
			}
			else // vertices
			{
				obj->obj_vert_amount++;
				obj->obj_verts[(obj->obj_vert_amount - 1) * obj->obj_vert_value_amount + 0] = atof(arr[1]);
				obj->obj_verts[(obj->obj_vert_amount - 1) * obj->obj_vert_value_amount + 1] = atof(arr[2]);
				obj->obj_verts[(obj->obj_vert_amount - 1) * obj->obj_vert_value_amount + 2] = atof(arr[3]);
			}
		}
		else if (line[0] == 'f') // indices
		{
			if (curr_elem->index_amount == 0)
			{
				// Preallocating an x amount of indices, adjusting size if need be;
				curr_elem->index_allocated = 8192;
				curr_elem->indices = malloc(sizeof(uint32_t) * curr_elem->index_allocated);
				/*
				LG_DEBUG("Starting element builder. Mallocing %d indices.",
					curr_elem->index_allocated);
					*/
			}
			else if (curr_elem->index_amount / (curr_elem->index_allocated * 0.75f) > 1.0f)
			{
				curr_elem->index_allocated += 8192;
				curr_elem->indices = realloc(curr_elem->indices,
					sizeof(uint32_t) * curr_elem->index_allocated);
				LG_DEBUG("Still building elements. (%d) Reallocing %d indices.",
					curr_elem->index_amount, curr_elem->index_allocated);
			}
			element_builder(obj, arr);
		}
		else if (line[0] == 'o') // New mesh
		{
			LG_DEBUG("[%s] New mesh found #%d <%s>\n",
				__FUNCTION__, obj->mesh_amount + 1, arr[1]);
			if (mesh_found)
			{
				obj->meshes = realloc(obj->meshes,
					sizeof(t_mesh) * (++obj->mesh_amount + 1));
				curr_mesh = &obj->meshes[obj->mesh_amount];
				memset(curr_mesh, 0, sizeof(t_mesh));
				curr_mesh->name = ft_strdup(arr[1]);
				curr_mesh->vertex_value_amount = 3;
				curr_mesh->normal_value_amount = 3;
				curr_mesh->uv_value_amount = 2;

				LG_DEBUG("[%s] Created mesh\n", __FUNCTION__);

				LG_DEBUG("[%s] Creating default element\n", __FUNCTION__);
				curr_mesh->element_amount = 0;
				curr_mesh->elements = realloc(curr_mesh->elements, sizeof(t_element) * 1);
				curr_elem = &curr_mesh->elements[curr_mesh->element_amount];
				memset(curr_elem, 0, sizeof(t_element));
				curr_elem->index_value_amount = 3;
				LG_DEBUG("[%s] Created new element #%d\n", __FUNCTION__, curr_mesh->element_amount);
				usemtl_found = 0; // for every mesh we need to check for usemtl found already;
			}
			else
			{
				curr_mesh->name = ft_strdup(arr[1]);
				LG_DEBUG("Dont need to create a new one tho.\n");
			}
			mesh_found = 1;
		}
		else if (ft_strequ(arr[0], "mtllib"))
		{
			obj->mtllib = ft_strdup(arr[1]);
			LG_DEBUG("mtllib found : <%s>", obj->mtllib);
			LG_DEBUG("root_path : %s", obj->root_path);
			obj->materials = material_load(&obj->material_amount,
				obj->root_path, obj->mtllib);
			LG_DEBUG("done reading mats");
		}
		else if (ft_strequ(arr[0], "usemtl"))
		{
			//LG_DEBUG("We have found usemtl : %s\n", arr[1]);
			if (usemtl_found)
			{
				curr_mesh->elements
					= realloc(curr_mesh->elements, sizeof(t_element)
						* (++curr_mesh->element_amount + 1));
				curr_elem = &curr_mesh->elements[curr_mesh->element_amount];
				memset(curr_elem, 0, sizeof(t_element));
				//LG_DEBUG("Created new element #%d\n", curr_mesh->element_amount);
			}
			usemtl_found = 1;
			curr_elem->material
				= get_material(obj->materials, obj->material_amount, arr[1]);
		}
		ft_arraydel(arr);

		if (ding % 100000 == 0)
			putchar('.');
		ding += 2;
	}

	free(file_content);
	free(elem_pos);

	LG_DEBUG("Done reading obj file, now we need to calculate some stuff.");

	// Calculating size of the index arrays;
	int	iter = -1;
	t_mesh *iter_mesh;
	t_element *iter_elem;

	obj->mesh_amount++;
	while (++iter < obj->mesh_amount)
	{
		iter_mesh = &obj->meshes[iter];

		iter_mesh->uvs_value_amount = iter_mesh->uv_amount * iter_mesh->uv_value_amount;
		iter_mesh->uvs_size = sizeof(float) * iter_mesh->uvs_value_amount;

		iter_mesh->normals_value_amount = iter_mesh->normal_amount * iter_mesh->normal_value_amount;
		iter_mesh->normals_size = sizeof(float) * iter_mesh->normals_value_amount;

		iter_mesh->vertices_value_amount = iter_mesh->vertex_amount * iter_mesh->vertex_value_amount;
		iter_mesh->vertices_size = sizeof(float) * iter_mesh->vertices_value_amount;

		// Readjusting the sizes of the buffers;
		iter_mesh->vert_allocated = iter_mesh->vertices_value_amount;
		iter_mesh->vertices = realloc(iter_mesh->vertices, iter_mesh->vertices_size);

		iter_mesh->uv_allocated = iter_mesh->uvs_value_amount;
		iter_mesh->uvs = realloc(iter_mesh->uvs, iter_mesh->uvs_size);

		iter_mesh->norm_allocated = iter_mesh->normals_value_amount;
		iter_mesh->normals = realloc(iter_mesh->normals, iter_mesh->normals_size);

		int elem_iter = -1;
		iter_mesh->element_amount++;
		while (++elem_iter < iter_mesh->element_amount)
		{
			iter_elem = &iter_mesh->elements[elem_iter];

			iter_elem->index_value_amount = 3;
			iter_elem->index_amount /= iter_elem->index_value_amount;
			iter_elem->indices_value_amount = iter_elem->index_amount * iter_elem->index_value_amount;
			iter_elem->indices_size = sizeof(unsigned int) * iter_elem->indices_value_amount;

			iter_elem->face_count = iter_elem->index_amount / 3;

			// Readjusting indices size;
			iter_elem->index_allocated = iter_elem->index_amount * iter_elem->index_value_amount;
			iter_elem->indices = realloc(iter_elem->indices, iter_elem->indices_size);
		}
		LG_DEBUG("Mesh #%d has %d elements.\n", iter, iter_mesh->element_amount);
	}
	LG_DEBUG("Freeing temp obj stuff.");
	free(obj->obj_verts);
	free(obj->obj_norms);
	free(obj->obj_uvs);

	LG_DEBUG("Done.");
	return (1);
}

void	obj_info(t_obj *obj)
{
	LG_DEBUG("meshi : %d\n", obj->mesh_amount);
	LG_DEBUG("vertices : %d\n", obj->obj_vert_amount);
	int ind_total = 0;
	int elem_total = 0;
	int	vert_total = 0;
	for (int i = 0; i < obj->mesh_amount; i++)
	{
		for (int j = 0; j < obj->meshes[i].element_amount; j++)
			ind_total += obj->meshes[i].elements[j].index_amount;
		elem_total += obj->meshes[i].element_amount;
		vert_total += obj->meshes[i].vertex_amount;
	}
	LG_DEBUG("indices : %d\n", ind_total);
	LG_DEBUG("elems : %d\n", elem_total);
	LG_DEBUG("verts : %d\n", vert_total);
}

/*
 * Returns mesh pointer from array of meshes with name;
*/
t_mesh	*get_mesh(t_mesh *meshes, size_t mesh_amount, char *name)
{
	for (size_t i = 0; i < mesh_amount; i++)
	{
		LG_DEBUG("Mesh : <%s> == <%s>?\n", meshes[i].name, name);
		if (ft_strequ(meshes[i].name, name))
			return (&meshes[i]);
	}
	LG_DEBUG("[%s] Couldn\'t find mesh with the name <%s>.\n", __FUNCTION__, name);
	return (NULL);
}

int	ft_strcharoccurence(char *str, int c)
{
	int	count;

	count = 0;
	while (*str)
	{
		str++;
		if (*str == c)
			count++;
	}
	return (count);
}

int	ft_strlentodelim(char *str, int c)
{
	int	count;

	count = 0;
	while (str[count])
	{
		if (str[count] == c)
			return (count);
		count++;
	}
	return (count);
}

char	**ft_strsplitdup(char *str, char delim)
{
	char	**arr;
	int		wc;	
	int		i;
	int		wl;

	wc = ft_strcharoccurence(str, delim) + 1;
	arr = ft_memalloc(sizeof(char *) * (wc + 1));
	i = 0;
	while (i < wc)
	{
		wl = ft_strlentodelim(str, delim);
		arr[i] = ft_strndup(&(*str), wl);
		str += wl + 1;
		i++;
	}
	arr[i] = 0;
	return (arr);
}

void	print_arr(char **arr)
{
	int	i;

	i = -1;
	while (arr[++i])
		printf("%s\n", arr[i]);
}

void	new_index(t_obj *obj, int v, int vt, int vn)
{
	int			ind;
	size_t		pp;
	t_mesh		*curr_mesh;
	t_element	*elem;

	curr_mesh = &obj->meshes[obj->mesh_amount];
	//ind = get_index_of(obj, v, vt, vn); // this is the line that doesnt allow duplicates;
	ind = -1; // uncomment if you dont care if duplicates get added;

	elem = &curr_mesh->elements[curr_mesh->element_amount];
	elem->index_amount++;
	if (ind != -1)
	{
		elem->indices[elem->index_amount - 1] = ind;
		return ;
	}
	// Add new vertices
	if (v != -1)
	{
		if (curr_mesh->vertex_amount == 0)
		{
			curr_mesh->vert_allocated = 100000;
			curr_mesh->vertices = malloc(sizeof(float) * curr_mesh->vert_allocated);
			if (!curr_mesh->vertices)
				LG_ERROR("Mesh vertices couldnt be malloced initially.");
			LG_DEBUG("Vertices initial %d malloced.", curr_mesh->vert_allocated);
		}
		else if ((curr_mesh->vertex_amount * curr_mesh->vertex_value_amount) / (curr_mesh->vert_allocated * 0.99f) > 1.0f)
		{
			LG_DEBUG("Vertices reallocing %d. (curr: %d, actual: %d, new : %d)",
				curr_mesh->vert_allocated + 8192,
				sizeof(float) * curr_mesh->vert_allocated,
				curr_mesh->vertex_amount * curr_mesh->vertex_value_amount * sizeof(float),
				sizeof(float) * (curr_mesh->vert_allocated + 8192)
				);
			curr_mesh->vert_allocated += 8192;
			curr_mesh->vertices = realloc(curr_mesh->vertices,
				sizeof(float) * curr_mesh->vert_allocated);
			if (!curr_mesh->vertices)
				LG_ERROR("Mesh Vertices couldnt be realloced.");
		}
		curr_mesh->vertex_amount++;

		pp = 0;
		while (pp < curr_mesh->vertex_value_amount)
		{
			curr_mesh->vertices[((curr_mesh->vertex_amount - 1)
				* curr_mesh->vertex_value_amount) + pp]
					= obj->obj_verts[v * curr_mesh->vertex_value_amount + pp];
			pp++;
		}
	}
	// Add new normals
	if (vn != -1)
	{
		if (curr_mesh->normal_amount == 0)
		{
			curr_mesh->norm_allocated = 100000;
			curr_mesh->normals = malloc(sizeof(float) * curr_mesh->norm_allocated);
			if (!curr_mesh->normals)
				LG_ERROR("Mesh normals couldnt be malloced initially.");
			LG_DEBUG("Normals initial %d malloced.", curr_mesh->norm_allocated);
		}
		else if ((curr_mesh->normal_amount * curr_mesh->normal_value_amount) / (curr_mesh->norm_allocated * 0.99f) > 1.0f)
		{
			LG_DEBUG("Normals reallocing %d. (curr: %d, actual: %d, new : %d)",
				curr_mesh->norm_allocated + 8192,
				sizeof(float) * curr_mesh->norm_allocated,
				curr_mesh->normal_amount * curr_mesh->normal_value_amount * sizeof(float),
				sizeof(float) * (curr_mesh->norm_allocated + 8192)
				);
			curr_mesh->norm_allocated += 8192;
			curr_mesh->normals = realloc(curr_mesh->normals,
				sizeof(float) * curr_mesh->norm_allocated);
			if (!curr_mesh->normals)
				LG_ERROR("Mesh normals couldnt be realloced.");
		}
		curr_mesh->normal_amount++;

		pp = 0;
		while (pp < curr_mesh->normal_value_amount)
		{
			curr_mesh->normals[((curr_mesh->normal_amount - 1)
				* curr_mesh->normal_value_amount) + pp]
					= obj->obj_norms[vn * curr_mesh->normal_value_amount + pp];
			pp++;
		}
	}
	// Add new uvs
	if (vt != -1)
	{
		if (curr_mesh->uv_amount == 0)
		{
			curr_mesh->uv_allocated = 100000;
			curr_mesh->uvs = malloc(sizeof(float) * curr_mesh->uv_allocated);
			if (!curr_mesh->uvs)
				LG_ERROR("Mesh uvs couldnt be malloced initially.");
			LG_DEBUG("UVs initial %d malloced.", curr_mesh->uv_allocated);
		}
		else if ((curr_mesh->uv_amount * curr_mesh->uv_value_amount) / (curr_mesh->uv_allocated * 0.99f) > 1.0f)
		{
			LG_DEBUG("UVs reallocing %d. (curr: %d, actual: %d, new : %d)",
				curr_mesh->uv_allocated + 8192,
				sizeof(float) * curr_mesh->uv_allocated,
				curr_mesh->uv_amount * curr_mesh->uv_value_amount * sizeof(float),
				sizeof(float) * (curr_mesh->uv_allocated + 8192)
				);
			curr_mesh->uv_allocated += 8192;
			curr_mesh->uvs = realloc(curr_mesh->uvs,
				sizeof(float) * curr_mesh->uv_allocated);
			if (!curr_mesh->uvs)
				LG_ERROR("Mesh uvs couldnt be realloced.");
		}
		curr_mesh->uv_amount++;

		pp = 0;
		while (pp < curr_mesh->uv_value_amount)
		{
			curr_mesh->uvs[((curr_mesh->uv_amount - 1)
				* curr_mesh->uv_value_amount) + pp]
					= obj->obj_uvs[vt * curr_mesh->uv_value_amount + pp];
			pp++;
		}
	}
	elem->indices[elem->index_amount - 1] = curr_mesh->vertex_amount - 1;
}

/*
 * NOTE:
 * 	This function is specifically made for the index getter.
 * 
 * We always malloc 3 places for the indices.
 * 	If there is no vt and/or no vn we just place an empty char * there;
*/
void	strtovertices(char *str, char **arr)
{
	int		i;
	int		wl;
	int		slashes;

	i = -1;
	slashes = 0;
	while (str[++i])
		if (str[i] == '/')
			slashes++;
	wl = ft_strlentodelim(str, '/');
	arr[0] = ft_strndup(&(*str), wl);
	str += wl + 1;
	if (slashes > 0)
	{
		wl = ft_strlentodelim(str, '/');
		if (wl > 0)
			arr[1] = ft_strndup(&(*str), wl);
		str += wl + 1;
	}
	if (slashes > 1)
	{
		wl = ft_strlentodelim(str, '/');
		if (wl > 0)
			arr[2] = ft_strndup(&(*str), wl);
		str += wl + 1;
	}
}

/*
 * Processes the f of obj file.
*/
void	element_builder(t_obj *obj, char **arr)
{
	char	*farr[3];
	int		kk = 1; // skip the 'f';
	int		v;
	int		vt;
	int		vn;
	int		first_index;
	t_mesh	*curr_mesh = &obj->meshes[obj->mesh_amount];
	t_element *elem = &curr_mesh->elements[curr_mesh->element_amount];

	while (arr[kk])
	{
		farr[0] = NULL;
		farr[1] = NULL;
		farr[2] = NULL;
		strtovertices(arr[kk], farr);
		v = -1;
		vt = -1;
		vn = -1;
		if (farr[0])
			v = atoi(farr[0]) - 1;
		if (farr[1])
			vt = atoi(farr[1]) - 1;
		if (farr[2])
			vn = atoi(farr[2]) - 1;
		if (kk > 3) // if index amount is more than 3, convert to triangle;
		{
			int previous_index = elem->indices[elem->index_amount - 1];

			elem->index_amount += 2;
			elem->indices[elem->index_amount - 2] = first_index;
			elem->indices[elem->index_amount - 1] = previous_index;
		}
		new_index(obj, v, vt, vn);
		if (kk == 1)
			first_index = elem->indices[elem->index_amount - 1];
		kk++;
		ft_strdel(&farr[0]);
		ft_strdel(&farr[1]);
		ft_strdel(&farr[2]);
	}
}

/*
 * Takes in the pointer of the material amount pointer you want it to save the
 * material count in, and the path of the materials file.
 * Returns array of materials loaded.
 * 
 * If the mat_file isnt found in direct path, we try with concatting with root path;
*/
t_material	*material_load(
			size_t *mat_amount, const char *root_path, const char *mat_file)
{
	char		*file_content;
	char		line[256];
	char		**arr;
	int			ding;
	t_material	*materials;
	t_material	*curr_mat = NULL;

	file_content = get_file_content(mat_file);
	if (!file_content)
	{
		int	root_len = ft_strlen(root_path);
		int mat_len = ft_strlen(mat_file);
		ft_strncpy(line, root_path, root_len);
		ft_strncpy(line + root_len, mat_file, mat_len);
		line[root_len + mat_len] = 0;
		file_content = get_file_content(line);
		if (!file_content)
			return (NULL);
	}

	materials = ft_memalloc(sizeof(t_material));

	int amount = get_elem_amount(file_content, '\n') * 2;
	int *elem_pos = malloc(sizeof(int) * amount); // remember to free;
	fill_elem_pos(elem_pos, file_content, '\n');

	ding = 0;
	*mat_amount = 0;
	while (ding < amount)
	{
		ft_strncpy(line, &file_content[elem_pos[ding]], elem_pos[ding + 1]);
		line[elem_pos[ding + 1]] = 0;
		ding += 2;
		arr = ft_strsplit(line, ' ');
		if (ft_strequ(arr[0], "newmtl"))
		{
			*mat_amount += 1;
			materials = realloc(materials, sizeof(t_material) * *mat_amount);
			curr_mat = &materials[*mat_amount - 1];
			curr_mat->name = ft_strdup(arr[1]);
		}
		else if (ft_strequ(arr[0], "Ns"))
			curr_mat->Ns = ft_atof(arr[1]);
		else if (ft_strequ(arr[0], "Ka"))
		{
			curr_mat->Ka[0] = ft_atof(arr[1]);
			curr_mat->Ka[1] = ft_atof(arr[2]);
			curr_mat->Ka[2] = ft_atof(arr[3]);
		}
		else if (ft_strequ(arr[0], "Kd"))
		{
			curr_mat->Kd[0] = ft_atof(arr[1]);
			curr_mat->Kd[1] = ft_atof(arr[2]);
			curr_mat->Kd[2] = ft_atof(arr[3]);
		}
		else if (ft_strequ(arr[0], "Ks"))
		{
			curr_mat->Ks[0] = ft_atof(arr[1]);
			curr_mat->Ks[1] = ft_atof(arr[2]);
			curr_mat->Ks[2] = ft_atof(arr[3]);
		}
		else if (ft_strequ(arr[0], "Ke"))
		{
			curr_mat->Ke[0] = ft_atof(arr[1]);
			curr_mat->Ke[1] = ft_atof(arr[2]);
			curr_mat->Ke[2] = ft_atof(arr[3]);
		}
		else if (ft_strequ(arr[0], "Ni"))
			curr_mat->Ni = ft_atof(arr[1]);
		else if (ft_strequ(arr[0], "d"))
			curr_mat->d = ft_atof(arr[1]);
		else if (ft_strequ(arr[0], "illum"))
			curr_mat->illum = ft_atoi(arr[1]);
		else if (ft_strequ(arr[0], "map_Kd"))
			curr_mat->map_Kd = ft_strdup(arr[1]);
		else if (ft_strequ(arr[0], "map_Ks"))
			curr_mat->map_Ks = ft_strdup(arr[1]);
		else if (ft_strequ(arr[0], "map_Ns"))
			curr_mat->map_Ns = ft_strdup(arr[1]);
		else if (ft_strequ(arr[0], "map_Bump"))
			curr_mat->map_Bump = ft_strdup(arr[1]);
	}
	free(file_content);
	free(elem_pos);
	LG_DEBUG("We have found %d materials.", *mat_amount);
	if (0) // DEbug
		for (int i = 0; i < *mat_amount; i++)
			LG_INFO("#%d : (kd <%s>)", i, materials[i].map_Kd);
	return (materials);
}

// Returns the address of the 'material' with 'material->name' == 'to_find';
t_material	*get_material(t_material *materials, size_t mats, char *to_find)
{
	size_t	i;

	i = 0;
	while (i < mats)
	{
		if (ft_strequ(materials[i].name, to_find))
			return (&materials[i]);
		i++;
	}
	return (NULL);
}

/*
 * Takes in 2 arrays and the starting point of where we are starting to compare in both arrays,
 * also how many elements one value in the array is.
 * Goes through array amount amount times, and compares the 2 arrays.
 * Return 1 if all amount of elements are the same, 0 if < amount is not.
*/
int	vert_same(float *arr, int begin1, float *arr2, int begin2, int amount)
{
	int		i;
	int		result;
	int		actual2;
	int		actual1;

	i = 0;
	result = 0;
	actual1 = begin1 * amount;
	actual2 = begin2 * amount;
	while (i < amount)
	{
		if (arr[actual1 + i] == arr2[actual2 + i])
			result++;
		i++;
	}
	return (result == amount);
}

/*
 * Takes in obj, vertex- (v), normal- (vn) and uv index (vt).
 * Goes through all the currently correctly indexed v, vn, vt
 * and returns it.
 * If we cant find an index with exactly those values,
 * it returns -1, which means you have to make a new index and
 * add all the values from v, vn, vt from the .obj file read values to
 * corresponding actual vbo arrays.
 * If v/vn/vt is -1, dont compare from those arrays.
*/
int	get_index_of(t_obj *obj, int v, int vt, int vn)
{
	size_t	i = 0;
	int		to_check_count = 0;
	int		result;
	t_mesh	*curr_mesh;

	curr_mesh = &obj->meshes[obj->mesh_amount];
//	printf("Checking : %d %d %d\n", v, vt, vn);
	if (v != -1)
		to_check_count++;
	if (vt != -1)
		to_check_count++;
	if (vn != -1)
		to_check_count++;
	while (i < curr_mesh->vertex_amount)
	{
		result = 0;
		if (v != -1 && vert_same(curr_mesh->vertices, i, obj->obj_verts, v, curr_mesh->vertex_value_amount))
			result++;
		if (vt != -1 && vert_same(curr_mesh->uvs, i, obj->obj_uvs, vt, curr_mesh->uv_value_amount))
			result++;
		if (vn != -1 && vert_same(curr_mesh->normals, i, obj->obj_norms, vn, curr_mesh->normal_value_amount))
			result++;
		if (result == to_check_count)
			return (i);
		i++;
	}
	return (-1);
}