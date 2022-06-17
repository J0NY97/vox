/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   model.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 13:41:57 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/17 12:08:06 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "shaderpixel.h"

void	fill_element_info(t_element_info *info, t_element *elem)
{
	info->element = *elem;

	info->material = NULL;

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

	if (info->mesh.vertices_size > 0)
		glEnableVertexAttribArray(0); // pos
	if (info->colors_size > 0)
		glEnableVertexAttribArray(1); // col
	if (info->mesh.uvs_size > 0)
		glEnableVertexAttribArray(2); // uvs
	if (info->mesh.normals_size > 0)
		glEnableVertexAttribArray(3); // nor

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
	char	*temp_mat_kd;

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
			if (access(model->mat_info[i].material->map_Kd, F_OK))
			{
				temp_mat_kd = ft_strjoin(obj->root_path, model->mat_info[i].material->map_Kd);
				LG_WARN("Cant find texture (%s) with direct path. Trying <%s>",
					model->mat_info[i].material->map_Kd, temp_mat_kd);
				free(model->mat_info[i].material->map_Kd);
				model->mat_info[i].material->map_Kd = ft_strdup(temp_mat_kd);
				free(temp_mat_kd);
			}
			glGenTextures(1, &model->mat_info[i].texture);
			model->mat_info[i].loaded = new_texture(&model->mat_info[i].texture, model->mat_info[i].material->map_Kd);
		}
	}

	LG_INFO("materials created. (%d)", glGetError());

	model->info_amount = obj->mesh_amount;
	model->info = malloc(sizeof(t_mesh_info) * model->info_amount);
	for (int i = 0; i < model->info_amount; i++)
	{
		fill_mesh_info(&model->info[i], &obj->meshes[i]);

		// Give the mesh elements their corresponding materials; (i know its convoluted);
		for (int e = 0; e < model->info[i].mesh.element_amount; e++)
		{
			for (int t = 0; t < model->mat_info_amount; t++)
			{
				if (model->info[i].elem_info[e].element.material == model->mat_info[t].material)
					model->info[i].elem_info[e].material = &model->mat_info[t];
			}
		}
	}

	LG_INFO("new model made from [%s] (%d)", obj->name, glGetError());
}

void	render_element(t_element_info *elem)
{
	int error = glGetError();

	if (elem->material && elem->material->loaded)
	{
		glActiveTexture(GL_TEXTURE0 + 0);
		glBindTexture(GL_TEXTURE_2D, elem->material->texture);
	}

	if (elem->element.indices_size == 0)
	{
		// glDrawArrays(GL_TRIANGLE_STRIP, 0, mesh.vertex_amount);
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elem->ebo);
		glDrawElements(GL_TRIANGLES, elem->element.indices_value_amount, GL_UNSIGNED_INT, NULL);
	}

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

void	render_mesh(t_mesh_info *mesh)
{
	int error = glGetError();

	glBindVertexArray(mesh->vao);

	for (int i = 0; i < mesh->mesh.element_amount; i++)
		render_element(&mesh->elem_info[i]);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

void	render_model(t_model *model)
{
	for (int i = 0; i < model->info_amount; i++)
		render_mesh(&model->info[i]);
}

////////////////////////////////
// MODEL v2
////////////////////////////////

/*
 * 'model' : has to be allocated already;
*/
void	model_init(t_model_v2 *model)
{
	GLuint	vbo[3];

	glGenVertexArrays(1, &model->vao);
	glBindVertexArray(model->vao);

	glEnableVertexAttribArray(0); // pos
	glEnableVertexAttribArray(1); // uvs

	glGenBuffers(2, vbo);
	model->vbo_pos = vbo[0];
	model->vbo_tex = vbo[1];

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo_tex);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(float) * 2, NULL);

	// NOTE: you have to load the mats into this separately;
	model->materials = NULL;
	model->material_amount = 0;

	model->vertices = NULL;
	model->uvs = NULL;
	model->normals = NULL;
	model->vertices_amount = 0;
	model->uvs_amount = 0;
	model->normals_amount = 0;

	// NOTE: you have to load the meshes into this separately;
	model->meshes = NULL;
	model->meshes_amount = 0;

	glBindVertexArray(0);
}

/*
 * 'mat' : has to be allocated already;
*/
void	material_init(t_material_v2 *mat)
{
	mat->texture = 0;
}

/*
 * 'mesh' : has to be allocated already;
*/
void	mesh_init(t_mesh_v2 *mesh)
{
	glGenBuffers(1, &mesh->ebo);
	mesh->indices = NULL;
	mesh->indices_amount = 0;
}

/*
 * Since 't_bobj' can have multiple objects in it, this function wants the
 *	't_bobj_object' that we want to create a model of;
 *
 * 'index' : is the nth index from 't_bobj->t_bobj_object's;
*/
void	model_from_bobj(t_model_v2 *model, t_bobj *bob, int index)
{
	t_bobj_object	*bobject;
	char			tmp[256];

	if (index < 0 || index > bob->objects_amount)
	{
		LG_WARN("Trying to create model from nonexistant bob (index : %d)", index);
		return ;
	}
	LG_INFO("Start");
	bobject = &bob->objects[index];

	model_init(model);
	// V- NOTE: we are typecasting from 't_bobj_v3' to 'float *'..; (how is this even allowed);
	model->vertices = (float *)bobject->v;
	model->vertices_amount = bobject->v_amount * 3;
	model->uvs = (float *)bobject->vt;
	model->uvs_amount = bobject->vt_amount * 3;
	model->normals = (float *)bobject->vn;
	model->normals_amount = bobject->vn_amount * 3;

	model->material_amount = bob->materials_amount;
	model->materials = malloc(sizeof(t_material_v2) * model->material_amount);
	for (int m = 0; m < model->material_amount; m++)
	{
		material_init(&model->materials[m]);
		if (!new_texture(&model->materials[m].texture, bob->materials[m].map_Kd))
		{
			ft_strcpy(tmp, bob->root_dir);
			ft_strcpy(tmp + ft_strlen(tmp), bob->materials[m].map_Kd);
			new_texture(&model->materials[m].texture, tmp);
		}
	}
	
	model->meshes_amount = bobject->meshes_amount;
	model->meshes = malloc(sizeof(t_mesh_v2) * model->meshes_amount);
	for (int m = 0; m < model->meshes_amount; m++)
	{
		mesh_init(&model->meshes[m]);
		// V- NOTE: we are typecasting from 't_bobj_u3' to 'uint *'..; (how is this even allowed);
		model->meshes[m].indices = (unsigned int *)bobject->meshes[m].f;
		model->meshes[m].indices_amount = (int)bobject->meshes[m].index_amount;
		model->meshes[m].texture = model->materials[bobject->meshes[m].material_index].texture;
		ft_printf("texture : %d\n", model->meshes[m].texture);
	}
	LG_INFO("End");
}

void	mesh_update(t_mesh_v2 *mesh)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) *
		mesh->indices_amount, mesh->indices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*
 * Update the model to the gpu
 *
 * TODO : at some point have the ability to only update one of the VBOs.
 */
void	model_update(t_model_v2 *model)
{
	glBindVertexArray(model->vao);

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->vertices_amount,
		model->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * model->uvs_amount,
		model->uvs, GL_STATIC_DRAW);

	for (int i = 0; i < model->meshes_amount; i++)
		mesh_update(&model->meshes[i]);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*
 * Render the mesh;
*/
void	mesh_render(t_mesh_v2 *mesh)
{
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glDrawElements(GL_TRIANGLES, mesh->indices_amount, GL_UNSIGNED_INT, NULL);
}

/*
 * Render the model;
*/
void	model_render(t_model_v2 *model, GLuint shader, float *model_mat, float *view_mat, float *projection_mat)
{
	int	error;

	glUseProgram(shader);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, model_mat);
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, view_mat);
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, projection_mat);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);

	glBindVertexArray(model->vao);
	for (int i = 0; i < model->meshes_amount; i++)
		mesh_render(&model->meshes[i]);
	
	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}