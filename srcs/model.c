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

void	fill_element_info(t_element_info *info, t_element *elem)
{
	info->element = *elem;

	glGenBuffers(1, &info->ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, info->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, info->element.indices_size, &info->element.indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	float	temp[3] = {0, 255, 0};
	memset_pattern(info->colors, info->colors_size, temp, sizeof(float) * 3);

	glGenVertexArrays(1, &info->vao);
	glBindVertexArray(info->vao);

	glGenBuffers(4, vbo);
	info->vbo_pos = vbo[0];
	info->vbo_color = vbo[1];
	info->vbo_tex = vbo[2];
	info->vbo_norm = vbo[3];

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, info->mesh.vertices_size, &info->mesh.vertices[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * info->mesh.vertex_value_amount, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, info->colors_size, &info->colors[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(float) * info->color_value_amount, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, info->mesh.uvs_size, &info->mesh.uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, sizeof(float) * info->mesh.uv_value_amount, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, info->vbo_norm);
	glBufferData(GL_ARRAY_BUFFER, info->mesh.normals_size, &info->mesh.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_TRUE, sizeof(float) * info->mesh.normal_value_amount, NULL);

	info->elem_info = malloc(sizeof(t_element_info) * info->mesh.element_amount);
	for (int i = 0; i < info->mesh.element_amount; i++)
		fill_element_info(&info->elem_info[i], &info->mesh.elements[i]);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	new_model(t_model *model, t_obj *obj)
{
	glGetError();

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
			if (access(model->mat_info[i].material->map_Kd, F_OK))
			{
				free(model->mat_info[i].material->map_Kd);
				model->mat_info[i].material->map_Kd
					= ft_strjoin(obj->root_path, obj->materials[i].map_Kd);
			}
			model->mat_info[i].loaded = new_texture(&model->mat_info[i].texture, model->mat_info[i].material->map_Kd);
		}
	}

	model->info_amount = obj->mesh_amount;
	model->info = malloc(sizeof(t_mesh_info) * model->info_amount);
	for (int i = 0; i < model->info_amount; i++)
		fill_mesh_info(&model->info[i], &obj->meshes[i]);

	LG_INFO("new model made. (%d)", glGetError());
}

void	render_element(t_element_info *elem)
{
	// 1. activate texture
	// 2. draw arrays / triangles
	if (elem->element.indices_size == 0)
	{
		// glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertex_amount);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem->ebo);
		glDrawElements(GL_TRIANGLES, elem->element.indices_value_amount, GL_UNSIGNED_INT, 0);
	}
}

void	render_mesh(t_mesh_info *mesh)
{
	glBindVertexArray(mesh->vao);
	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // col
	glEnableVertexAttribArray(2); // uvs
	glEnableVertexAttribArray(3); // nor

	for (int i = 0; i < mesh->mesh.element_amount; i++)
		render_element(&mesh->elem_info[i]);
}

void	render_model(t_model *model)
{
	for (int i = 0; i < model->info_amount; i++)
		render_mesh(&model->info[i]);
}