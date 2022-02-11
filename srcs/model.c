#include "shaderpixel.h"

void	memset_pattern(void *dest, size_t dest_size, void *src, size_t src_size)
{
	size_t	i;

	i = 0;
	while (i < dest_size)
	{
		((char *)dest)[i] = ((char *)src)[i % src_size];
		i++;
	}
}

void	fill_mesh_info(t_mesh_info *info, t_mesh *mesh)
{
	GLuint	vbo[4];

	info->mesh = *mesh;

	// Full blue color just to show something;
	info->color_amount = mesh->vertex_amount;
	info->color_value_amount = 3;
	info->colors_value_amount = info->color_amount * info->color_value_amount;
	info->colors_size = sizeof(float) * info->colors_value_amount;
	info->colors = malloc(info->colors_size);
	float	temp[3] = {0, 0, 255};
	memset_pattern(info->colors, info->colors_size, temp, sizeof(float) * 3);

	glGenVertexArrays(1, &info->vao);
	glBindVertexArray(info->vao);

	glGenBuffers(3, vbo);
	info->vbo_pos = vbo[0];
	info->vbo_color = vbo[1];
	info->vbo_tex = vbo[2];
	info->vbo_norm = vbo[3];

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, info->mesh.vertices_size, &info->mesh.vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, info->mesh.vertices_size, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, info->colors_size, &info->colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, info->colors_size, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, info->mesh.uvs_size, &info->mesh.uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, info->mesh.uvs_size, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, info->mesh.normals_size, &info->mesh.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, info->mesh.normals_size, NULL);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	new_model(t_model *model, t_obj *obj)
{
	model->obj = obj;

	if (obj->mesh_amount <= 0)
	{
		LG_WARN("Couldnt create model : mesh_amount = %d", obj->mesh_amount);
		return ;
	}

	model->mat_info_amount = obj->material_amount;
	model->mat_info = NULL;
	if (model->mat_info_amount > 0)
	{
		model->mat_info = malloc(sizeof(t_material_info) * model->mat_info_amount);
		for (int i = 0; i < model->mat_info_amount; i++)
		{
			model->mat_info[i].material = &obj->materials[i];
			glGenTextures(1, &model->mat_info[i].texture);
			model->mat_info[i].loaded = new_texture(&model->mat_info[i].texture, model->mat_info[i].material->map_Kd);
		}
	}

	model->info_amount = obj->mesh_amount;
	model->info = malloc(sizeof(t_mesh_info) * model->info_amount);
	for (int i = 0; i < model->info_amount; i++)
		fill_mesh_info(&model->info[i], &obj->meshes[i]);
}