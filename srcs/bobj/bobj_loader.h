#ifndef BOBJ_LOADER_H
# define BOBJ_LOADER_H
# include "stdio.h"
# include "string.h"
# include "stdlib.h"
# include "libft.h"
# include "bimgf.h"

typedef struct s_obj		t_obj;

typedef struct s_material
{
	char			*name;
	float			Ka[3];	// ambient	color		(light scattered around the entire scene)
	float			Kd[3];	// diffuse	color		(usually the texture)
	float			Ks[3];	// specular	color		(shininess)
	float			Ke[3];	// emissive	coefficient (how much light is emitted from the object)
	float			Ns;		// specular	highlight	(the higher the value the tighter concentrated highlight)
	float			Ni;		// optical	density		(aka index of refraction)
	float			d;		// dissolve				(aka transparency)
	int				illum;	// illumination	model	(0 : constant color illumination, 1 : a diffuse illumination using Lambertian's shading..., 2 : a diffuse and specular illumination model using Lambertian's shading and Blinn's interpretation of Phong's specular illumination model...)
	char			*map_Bump;	// bump map
	char			*map_Kd;	// texture map
	char			*map_Ks;	// specular color texture map
	char			*map_Ns;	// specular highlight component
}	t_material;

typedef struct s_element
{
	unsigned int	*indices;
	size_t			indices_size;
	size_t			indices_value_amount;
	size_t			index_amount;
	size_t			index_value_amount;

	t_material		*material; // address of the material you should use;
	
	// Allocation amount;
	int				index_allocated;
}	t_element;


typedef struct		s_mesh
{
	char			*name;

	// Vertices
	float			*vertices;
	size_t			vertices_size;
	size_t			vertices_value_amount;
	size_t			vertex_amount;
	size_t			vertex_value_amount;
	// Texture Coordinates
	float			*uvs;
	size_t			uvs_size;
	size_t			uvs_value_amount;
	size_t			uv_amount;
	size_t			uv_value_amount;
	// Normals
	float			*normals;
	size_t			normals_size;
	size_t			normals_value_amount;
	size_t			normal_amount;
	size_t			normal_value_amount;

	t_element		*elements; // array of t_element, delimetered with usemtl in .obj;
	int				element_amount;

	// Allocation amounts;
	int				vert_allocated;
	int				norm_allocated;
	int				uv_allocated;
}					t_mesh;

struct s_obj
{
	char			*root_path; // dir of the .obj file location;
	char			*name;
	char			*mtllib; // mat file

	// obj_verts, obj_norms and obj_uvs, are only used in the obj_load():er;
	//	so those should probably not be stored in the t_obj. (useless memory usage);
	float			*obj_verts;
	size_t			obj_vert_amount;
	size_t			obj_vert_value_amount;

	float			*obj_norms;
	size_t			obj_norm_amount;
	size_t			obj_norm_value_amount;

	float			*obj_uvs;
	size_t			obj_uv_amount;
	size_t			obj_uv_value_amount;

	// Materials
	t_material		*materials; // array of t_materials;
	size_t			material_amount;

	t_mesh			*meshes; // array of meshi;
	int				mesh_amount;
};

int					obj_load(t_obj *obj, const char *obj_file_path);
void				obj_info(t_obj *obj);
t_material			*material_load(size_t *mat_amount, const char *root_path, const char *mat_file);
t_material			*get_material(t_material *materials, size_t mats, char *to_find);
t_mesh				*get_mesh(t_mesh *meshes, size_t mesh_amount, char *name);
int					get_index_of(t_obj *obj, int v, int vn, int vt);

#endif
