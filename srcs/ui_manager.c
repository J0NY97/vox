#include "shaderpixel.h"
#include "ui_manager.h"

void	ui_manager_setup_opengl(t_ui_manager *ui)
{
	int error;

	new_shader(&ui->shader, SHADER_PATH"ui.vs", SHADER_PATH"ui.fs");
	if (glIsProgram(ui->shader) != GL_TRUE)
		ft_printf("UI shader program isnt...\n");

	ui->uniform_tex = glGetUniformLocation(ui->shader, "Texture");
	ui->uniform_proj = glGetUniformLocation(ui->shader, "ProjMtx");

	ui->attrib_pos = glGetAttribLocation(ui->shader, "Position");
	ui->attrib_uv = glGetAttribLocation(ui->shader, "TexCoord");
	ui->attrib_col = glGetAttribLocation(ui->shader, "Color");

	if (ui->attrib_pos == -1)
		ft_printf("pos : %d\n", ui->attrib_pos);
	if (ui->attrib_uv == -1)
		ft_printf("uv : %d\n", ui->attrib_uv);
	if (ui->attrib_col == -1)
		ft_printf("col : %d\n", ui->attrib_col);

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

	ui->texture = ui_new_texture(ui, &ui->bitmap);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	error = glGetError();
	if (error)
		LG_ERROR("(%d)", error);
	
	LG_INFO("Done.");
}

void	ui_manager_init(t_ui_manager *ui)
{
	/*
	font_manager_init(&ui->font_manager);
	// Lets open default font;
	int arial_font_index = font_manager_get_font(&ui->font_manager, "C:/Windows/Fonts/arial.ttf", 12);
	print_face(ui->font_manager.font_faces[arial_font_index]);
	*/

	bitmap_new(&ui->bitmap, 1, 1);
	bitmap_fill(&ui->bitmap, 0xffffffff);
	ui->vertices_allocated = 2048;
	ui->vertices = malloc(sizeof(t_ui_vertex) * ui->vertices_allocated);
	ui->indices_allocated = 1024;
	ui->indices = malloc(sizeof(Uint32) * ui->indices_allocated);

	// Elements
	ui->elements = NULL;
	ui->element_amount = 0;
	ui->elements_allocated = 0;

	// Texture
	ui->all_textures = NULL;
	ui->textures_generated = 0;
	ui->textures_in_use = 0;

	ui_manager_setup_opengl(ui);
}

/*
 * This function should be called before we start adding ui elements;
*/
void	ui_manager_start(t_ui_manager *ui)
{
	ui->vertex_amount = 0;
	ui->index_amount = 0;
	ui->element_amount = 0;
	ui->textures_in_use = 1; // 0th texture is the default;
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Uint32) * ui->index_amount, ui->indices, GL_STREAM_DRAW);

	for (int i = 0; i < ui->element_amount; i++)
	{
		glBindTexture(GL_TEXTURE_2D, ui->elements[i].texture);
		glDrawElements(ui->elements[i].draw_type, ui->elements[i].index_amount, GL_UNSIGNED_INT, (void *)(ui->elements[i].index_start * sizeof(Uint32)));
	}

	/*
	glBindTexture(GL_TEXTURE_2D, ui->texture);
// 	glScissor(); you should scissor the outer most vertex coordinates that we have created from all the ui elements;
	glDrawElements(GL_TRIANGLES, ui->index_amount, GL_UNSIGNED_INT, NULL);
	*/

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
 * Returns the new element's index;
*/
int	ui_new_element(t_ui_manager *ui)
{
	if (ui->element_amount + 1 > ui->elements_allocated)
	{
		ui->elements_allocated += 256;
		ui->elements = realloc(ui->elements, sizeof(t_ui_element) * ui->elements_allocated);
		LG_WARN("Reallocated ui->elements (%d)", ui->elements_allocated);
	}

	ui->elements[ui->element_amount].index_amount = 0;
	ui->elements[ui->element_amount].index_start = 0;
	ui->elements[ui->element_amount].texture = ui->texture;
	ui->elements[ui->element_amount].draw_type = GL_TRIANGLES; 
	++ui->element_amount;
	return (ui->element_amount - 1);
}

/*
 * Returns the OpenGL texture that was created.
*/
GLuint	ui_new_texture(t_ui_manager *ui, t_bitmap *bmp)
{
	int new_texture_gened = 0;

	if (ui->textures_in_use + 1 > ui->textures_generated)
	{
		new_texture_gened = 1;
		ui->textures_generated += 1;
		ui->all_textures = realloc(ui->all_textures, sizeof(GLuint) * ui->textures_generated);
		glGenTextures(1, &ui->all_textures[ui->textures_in_use]);
		LG_WARN("New texture generated (%d)", ui->textures_generated);
	}

	glBindTexture(GL_TEXTURE_2D, ui->all_textures[ui->textures_in_use]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glGenerateMipmap(GL_TEXTURE_2D);
//	if (new_texture_gened)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bmp->width, bmp->height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, bmp->pixels);
		/*
	else
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, bmp->width, bmp->height,
			GL_RGBA, GL_UNSIGNED_BYTE, bmp->pixels);
		*/
	++ui->textures_in_use;
	glBindTexture(GL_TEXTURE_2D, 0);
	return (ui->all_textures[ui->textures_in_use - 1]);
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

	int start = ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v2);
	ui_manager_new_index(ui, v3);
	int end = ui_manager_new_index(ui, v4);

	int elem_index = ui_new_element(ui);
	ui->elements[elem_index].index_start = start;
	ui->elements[elem_index].index_amount = end - start + 1; 
	ui->elements[elem_index].draw_type = GL_LINE_LOOP; 
}

void	ui_draw_filled_rect(t_ui_manager *ui, float *pos, Uint8 *color)
{
	// TODO: check for some edge cases, like if w / h is <= 0 then we dont add rect, osv...;

	int v1 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1]}, (float []){0, 0}, color);
	int v2 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1] + pos[3]}, (float []){0, 0}, color);
	int v3 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1] + pos[3]}, (float []){0, 0}, color);
	int v4 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1]}, (float []){0, 0}, color);

	int start = ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v2);
	ui_manager_new_index(ui, v3);

	ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v3);
	int end = ui_manager_new_index(ui, v4);

	int elem_index = ui_new_element(ui);
	ui->elements[elem_index].index_start = start;
	ui->elements[elem_index].index_amount = end - start + 1; 
	ui->elements[elem_index].draw_type = GL_TRIANGLES; 
}

void	ui_draw_filled_rect_multi_color(t_ui_manager *ui, float *pos, Uint32 *color)
{
	// TODO: check for some edge cases, like if w / h is <= 0 then we dont add rect, osv...;

	int v1 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1]}, (float []){0, 0}, (Uint8 []){255, 255, 0, 255});
	int v2 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1] + pos[3]}, (float []){0, 0}, (Uint8 []){255, 0, 0, 255});
	int v3 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1] + pos[3]}, (float []){0, 0}, (Uint8 []){0, 0, 255, 255});
	int v4 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1]}, (float []){0, 0}, (Uint8 []){0, 255, 0, 255});

	int start = ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v2);
	ui_manager_new_index(ui, v3);

	ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v3);
	int end = ui_manager_new_index(ui, v4);

	int elem_index = ui_new_element(ui);
	ui->elements[elem_index].index_start = start;
	ui->elements[elem_index].index_amount = end - start + 1; 
	ui->elements[elem_index].draw_type = GL_TRIANGLES; 
}

/*
 *
*/
void	ui_draw_bitmap(t_ui_manager *ui, float *pos, t_bitmap *bmp)
{
	Uint8 col[] = {255, 255, 255, 255};
	int v1 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1]}, (float []){0, 0}, col);
	int v2 = ui_manager_new_vertex(ui, (float []){pos[0], pos[1] + pos[3]}, (float []){0, 1}, col);
	int v3 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1] + pos[3]}, (float []){1, 1}, col);
	int v4 = ui_manager_new_vertex(ui, (float []){pos[0] + pos[2], pos[1]}, (float []){1, 0}, col);

	int start = ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v2);
	ui_manager_new_index(ui, v3);

	ui_manager_new_index(ui, v1);
	ui_manager_new_index(ui, v3);
	ui_manager_new_index(ui, v4);

	int elem_index = ui_new_element(ui);
	ui->elements[elem_index].index_start = start;
	ui->elements[elem_index].index_amount = 6;
	ui->elements[elem_index].draw_type = GL_TRIANGLES; 
	ui->elements[elem_index].texture = ui_new_texture(ui, bmp);
}

// Collision ?

int	point_in_rectangle(float *v2, float *v4)
{
	return (v2[0] >= v4[0] && v2[0] < v4[0] + v4[2] &&
		v2[1] >= v4[1] && v2[1] < v4[1] + v4[3]);
}