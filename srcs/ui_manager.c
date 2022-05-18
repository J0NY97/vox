#include "shaderpixel.h"
#include "ui_manager.h"

void	ui_manager_setup_opengl(t_ui_manager *ui)
{
	glGenVertexArrays(1, &ui->vao);
	glBindVertexArray(ui->vao);

	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ui->vbo);

	glBindBuffer(GL_ARRAY_BUFFER, ui->vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat) * 3, NULL);

	glGenBuffers(1, &ui->ebo);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void	ui_manager_copy_to_canvas(t_ui_manager *ui, t_bitmap *src, int x, int y)
{
	(void)ui;
	(void)src;
	(void)x;
	(void)y;
}

void	ui_manager_init(t_ui_manager *ui)
{
	new_bitmap(&ui->canvas);
	ui_manager_setup_opengl(ui);
}

void	ui_manager_render(t_ui_manager *ui)
{
	(void)ui;	
}