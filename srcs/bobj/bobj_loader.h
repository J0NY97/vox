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

/*
 * face_count	= index_amount / (indices_per_face * index_value_amount);
 * 				= index_amount / (3 * 3) = index_amount / 9;
*/
typedef struct s_element
{
	unsigned int	*indices;
	int				indices_size;
	int				indices_value_amount;
	int				index_amount;
	int				index_value_amount;

	int				face_count;

	t_material		*material; // address of the material you should use;

	// Allocation amount;
	int				index_allocated;
}	t_element;

/*
 * Vertices / uvs / normals / indices, all follow these rules;
 *
 * vertices : array with the vertex coordinates; 3 values per vertex_amount;
 * vertices_size : the size of the vertices array in bytes;
 * vertices_value_amount : amount of values in the vertices array;
 * vertex_amount : amount of vertices in the array;
 * vertex_value_amount : amount of values per vertex;
 *
 * ex:
 * 	vertex_value_amount = 3 (x, y, z)
 * 	vertex_amount = 6 (2 triangles)
 *  vertices_value_amount = vertex_value_amount * vertex_amount (3 * 6 = 18)
 *  vertices_size = vertices_value_amount * sizeof(float) (18 * 4 = 72 bytes)
 *  vertices = { 18 floats (6 vertices (3 values per vertex)) };
 */
typedef struct		s_mesh
{
	char			*name;

	// Vertices
	float			*vertices;
	int				vertices_size;
	int				vertices_value_amount;
	int				vertex_amount;
	int				vertex_value_amount;
	// Texture Coordinates
	float			*uvs;
	int				uvs_size;
	int				uvs_value_amount;
	int				uv_amount;
	int				uv_value_amount;
	// Normals
	float			*normals;
	int				normals_size;
	int				normals_value_amount;
	int				normal_amount;
	int				normal_value_amount;

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
	int				obj_vert_amount;
	int				obj_vert_value_amount;

	float			*obj_norms;
	int				obj_norm_amount;
	int				obj_norm_value_amount;

	float			*obj_uvs;
	int				obj_uv_amount;
	int				obj_uv_value_amount;

	// Materials
	t_material		*materials; // array of t_materials;
	int				material_amount;

	t_mesh			*meshes; // array of meshi;
	int				mesh_amount;
};

int					obj_load(t_obj *obj, const char *obj_file_path);
void				obj_info(t_obj *obj);
t_material			*material_load(int *mat_amount, const char *root_path, const char *mat_file);
t_material			*get_material(t_material *materials, int mats, char *to_find);
t_mesh				*get_mesh(t_mesh *meshes, int mesh_amount, char *name);
int					get_index_of(t_obj *obj, int v, int vn, int vt);

// Print
void				bobj_element_print(t_element *elem);

#endif
