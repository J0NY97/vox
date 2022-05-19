#include "shaderpixel.h"
#include "ui_manager.h"

void	ui_manager_setup_opengl(t_ui_manager *ui)
{
	int error;

	new_shader(&ui->shader, SHADER_PATH"ui.vs", SHADER_PATH"ui.fs");

	ui->uniform_tex = glGetUniformLocation(ui->shader, "Texture");
	ui->uniform_proj = glGetUniformLocation(ui->shader, "ProjMtx");
	ui->attrib_pos = glGetAttribLocation(ui->shader, "Position");
	ui->attrib_uv = glGetAttribLocation(ui->shader, "TexCoord");
	ui->attrib_col = glGetAttribLocation(ui->shader, "Color");

	glGenVertexArrays(1, &ui->vao);
	glGenBuffers(1, &ui->vbo);
	glGenBuffers(1, &ui->ebo);

	glBindVertexArray(ui->vao);
	glBindBuffer(GL_ARRAY_BUFFER, ui->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui->ebo);

	glEnableVertexAttribArray(ui->attrib_pos);
	glEnableVertexAttribArray(ui->attrib_uv);
	glEnableVertexAttribArray(ui->attrib_col);

	glVertexAttribPointer(ui->attrib_pos, 2, GL_FLOAT, GL_FALSE, sizeof(t_ui_vertex), (void *)offsetof(t_ui_vertex, pos));
	glVertexAttribPointer(ui->attrib_uv, 2, GL_FLOAT, GL_FALSE, sizeof(t_ui_vertex), (void *)offsetof(t_ui_vertex, uv));
	glVertexAttribPointer(ui->attrib_col, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(t_ui_vertex), (void *)offsetof(t_ui_vertex, col));

	glGenTextures(1, &ui->texture);
	glBindTexture(GL_TEXTURE_2D, ui->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// TODO : move this to where we update the texture;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ui->bitmap.width, ui->bitmap.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, ui->bitmap.pixels);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
}

void	ui_manager_init(t_ui_manager *ui)
{
	bitmap_new(&ui->bitmap, 1, 1);
	bitmap_fill(&ui->bitmap, 0xffffffff);
	ui_manager_setup_opengl(ui);
	ui->vertices_allocated = 2048;
	ui->vertices = malloc(sizeof(t_ui_vertex) * ui->vertices_allocated);
	ui->indices_allocated = 1024;
	ui->indices = malloc(sizeof(Uint32) * ui->indices_allocated);
}

/*
 * This function should be called before we start adding ui elements;
*/
void	ui_manager_start(t_ui_manager *ui)
{
	ui->vertex_amount = 0;
	ui->index_amount = 0;
}

/*
 * This function should be called before we start drawing ui elements;
*/
void	ui_manager_end(t_ui_manager *ui)
{
	(void)ui;
}

void	ui_manager_render(t_ui_manager *ui, int width, int height /*window context*/)
{
	if (ui->index_amount <= 0 || ui->vertex_amount <= 0)
		return ;

	GLfloat ortho[16] = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	};
	ortho[0] /= (GLfloat)width;
	ortho[5] /= (GLfloat)height;

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	glUseProgram(ui->shader);
	glUniform1i(ui->uniform_tex, 0);
	glUniformMatrix4fv(ui->uniform_proj, 1, GL_FALSE, ortho);

	// Drawing
	glBindVertexArray(ui->vao);
	glBindBuffer(GL_ARRAY_BUFFER, ui->vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui->ebo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(t_ui_vertex) * ui->vertex_amount, ui->vertices, GL_STREAM_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Uint8) * ui->index_amount, ui->indices, GL_STREAM_DRAW);

	glBindTexture(GL_TEXTURE_2D, ui->texture);
// 	glScissor(); you should scissor the outer most vertex coordinates that we have created from all the ui elements;

	glDrawElements(GL_TRIANGLES, ui->index_amount / 3, GL_UNSIGNED_INT, NULL);

	// Set to default opengl state
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDisable(GL_BLEND);
	glDisable(GL_SCISSOR_TEST);
}

/*
 * Adds new t_ui_vertex to ui->vertices, reallocating the vertex array if we need more;
 * 
 * Returns index in the array;
*/
int	ui_manager_new_vertex(t_ui_manager *ui, float *v2, float *uv, Uint8 *col)
{
	t_ui_vertex	*v;

	if (ui->vertex_amount + 1 > ui->vertices_allocated)
	{
		ui->vertices_allocated += 256;
		ui->vertices = realloc(ui->vertices, sizeof(t_ui_vertex) * ui->vertices_allocated);
		LG_WARN("Reallocated ui->vertices (%d)", ui->vertices_allocated);
	}
	v = &ui->vertices[ui->vertex_amount];
	v->pos[0] = v2[0];
	v->pos[1] = v2[1];
	v->uv[0] = uv[0];
	v->uv[1] = uv[1];
	v->col[0] = col[0];
	v->col[1] = col[1];
	v->col[2] = col[2];
	v->col[3] = col[3];
	++ui->vertex_amount;
	return (ui->vertex_amount - 1);
}

/*
*/
int	ui_manager_new_index(t_ui_manager *ui, int index)
{
	if (ui->index_amount + 1 > ui->indices_allocated)
	{
		ui->indices_allocated += 256;
		ui->indices = realloc(ui->indices, sizeof(Uint32) * ui->indices_allocated);
		LG_WARN("Reallocated ui->indices (%d)", ui->indices_allocated);
	}

	ui->indices[ui->index_amount] = index;
	++ui->index_amount;
	return (ui->index_amount - 1);
}

/*
 * Adds outline of rectangle to the ui_manager vertices;
 *
 * 'pos'[4] : dimensions of rectangle;
 * 'color'[4] : rgba for all the points; (Uint8 []){r, g, b, a};
*/
void	ui_draw_rect(t_ui_manager *ui, float *pos, Uint8 *color)
{
	// TODO: check for some edge cases, like if w / h is <= 0 then we dont add rect, osv...;

	int v1 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1]}, (float []){0, 0}, color);
	int v2 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1] + pos[3]}, (float []){0, 0}, color);
	int v3 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1] + pos[3]}, (float []){0, 0}, color);
	int v4 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1]}, (float []){0, 0}, color);

	ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v2);
	ui_manager_new_index(ui, v3);
	ui_manager_new_index(ui, v4);
}