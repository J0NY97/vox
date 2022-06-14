#include "vox_entity.h"

/*
 * 'mesh' : where we store the info;
 * 'texture' : of the entity;
 * 'shader' : that is going to be used for rendering this mesh;
*/
void	init_vox_entity_mesh(t_vox_entity_mesh *mesh, GLuint texture, GLuint shader)
{
	mesh->vertices_allocated = 0;
	mesh->vertices = NULL; //malloc(sizeof(float) * mesh->vertices_allocated);
	mesh->vertices_amount = 0;

	mesh->uvs_allocated = 0;
	mesh->uvs = NULL; //malloc(sizeof(float) * mesh->vertices_allocated);
	mesh->uvs_amount = 0;

	mesh->indices_allocated = 0;
	mesh->indices = NULL; //malloc(sizeof(unsigned int) * mesh->indices_allocated);
	mesh->indices_amount = 0;

	mesh->texture = texture;
	mesh->shader = shader;

	glGenVertexArrays(1, &mesh->vao);
	glBindVertexArray(mesh->vao);

	glGenBuffers(1, &mesh->vbo_pos);
	glGenBuffers(1, &mesh->vbo_tex);

	glGenBuffers(1, &mesh->ebo);

	// pos
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(float) * 3, NULL);

	// tex
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_tex);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, sizeof(float) * 2, NULL);

	mesh->uniform_view = glGetUniformLocation(mesh->shader, "view");
	mesh->uniform_proj = glGetUniformLocation(mesh->shader, "projection");

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

/*
 * Updates the mesh to the gpu;
*/
void	update_vox_entity_mesh(t_vox_entity_mesh *mesh)
{
	glBindVertexArray(mesh->vao);

	// pos
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_pos);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->vertices_amount,
		mesh->vertices, GL_STATIC_DRAW);

	// tex
	glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo_tex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->uvs_amount,
		mesh->uvs, GL_STATIC_DRAW);

	// ebo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int) * mesh->indices_amount,
		mesh->indices, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

/*
 * @modular : you could take in view and projection instead of camera, then we
 *		we dont have to have the camera header included; and it would make
 *		this work with any type of ...?;
*/
void	render_vox_entity_mesh(t_vox_entity_mesh *mesh, t_camera *camera)
{
	glUseProgram(mesh->shader);
	glUniformMatrix4fv(mesh->uniform_view, 1, GL_FALSE, camera->view);
	glUniformMatrix4fv(mesh->uniform_proj, 1, GL_FALSE, camera->projection);

	glBindVertexArray(mesh->vao);
	
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
	glDrawElements(GL_TRIANGLES, mesh->indices_amount, GL_UNSIGNED_INT, NULL);

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	new_vox_entity(t_vox_entity *entity)
{
	(void)entity;
}