/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vox.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 14:27:43 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 13:21:36 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef VOX_H
# define VOX_H

//# include <ft2build.h>
//# include FT_FREETYPE_H

# include "limits.h"
# include "pthread.h"
# include "libft.h"
# include "libpf.h"
# include "liblg.h"
# include "glad.h"
# include "glfw3.h"
# include "bmath.h"
# include "bimgf.h"
# include "bobj_loader.h"
# include "collision.h"
# include "thread.h"
# include "hashtable.h"
# include "camera.h"
# include "model.h"
# include "enum.h"
# include "fps.h"
# include "entity.h"

///////////////////
//	TEXTURE
///////////////////

int			new_texture(GLuint *texture, const char *image_file);

///////////////////
//	SKYBOX
///////////////////

typedef struct s_skybox
{
	GLuint		vao;
	GLuint		vbo_position;
	GLuint		texture;
	GLuint		shader;
	float		vertices[6 * 3 * 6];
	size_t		vertices_size;
}	t_skybox;

static const char	*g_realistic_skybox[6] = {
	ROOT_PATH"skybox/realistic/land-3.bmp",
	ROOT_PATH"skybox/realistic/land-1.bmp",
	ROOT_PATH"skybox/realistic/land-5.bmp",
	ROOT_PATH"skybox/realistic/land-0.bmp",
	ROOT_PATH"skybox/realistic/land-2.bmp",
	ROOT_PATH"skybox/realistic/land-4.bmp"
};

static const char	*g_mc_skybox[6] = {
	ROOT_PATH"skybox/mc/side.bmp",
	ROOT_PATH"skybox/mc/side.bmp",
	ROOT_PATH"skybox/mc/bot.bmp",
	ROOT_PATH"skybox/mc/top.bmp",
	ROOT_PATH"skybox/mc/side.bmp",
	ROOT_PATH"skybox/mc/side.bmp"
};

unsigned int	load_cube_map(char **paths, int paths_size);
t_skybox		*new_skybox(t_skybox *skybox, const char **faces);
float			*skybox_vertices(float *res, size_t *size);
void			render_skybox(t_skybox *skybox, t_camera *camera);

///////////////////
//	FRACTAL
///////////////////

typedef struct s_fractal2d
{
	GLuint	vao;
	GLuint	vbo;
	GLuint	ebo;
	GLfloat	vertices[12];
	GLuint	indices[6];
}	t_fractal2d;

void		new_fractal2d(t_fractal2d *fractal);
void		render_fractal2d(t_fractal2d *fractal, GLuint shader);

//////////////////
//	SCENE
///////////////////

/*
 * entity_allocated : size of the entities array;
 * entity_amount : amount of entities in entities array:
 * note: we can have empty spots in the entities array thats why they are separate;
*/
typedef struct	s_scene
{
	t_vox_entity	**entities;
	size_t		entities_allocated;
	size_t		entity_amount;
}		t_scene;

void		create_scene(t_scene *scene);
void		remove_scene(t_scene *scene);
void		remove_entity_from_scene_with_index(t_scene *scene, size_t index);
void		remove_entity_from_scene(t_scene *scene, t_vox_entity *entity);
size_t		add_entity_to_scene(t_scene *scene, t_vox_entity *entity);
t_vox_entity	*get_scene_entity(t_scene *scene, size_t index);

///////////////////
//	VOX
///////////////////

typedef struct	s_vox
{
	GLFWwindow	*win;
	int			win_w;
	int			win_h;
	int			polygon_mode;
	/// @brief 0 : GL_FILL, 1 : GL_LINE, 2 : GL_POINT;
	int			renderMode;
}	t_vox;

///////////////////
//	HELP
///////////////////

void		memset_pattern(void *dest, size_t dest_size, void *src, size_t src_size);
void		normalized_device_coords(float *v3, float *mouse, int w, int h);

///////////////////
//	Crosshair
///////////////////

void		new_crosshair_shader(GLuint *shader);
void		render_crosshair(void);

///////////////////
// Line Draw
///////////////////

void		render_2d_line(float *p1, float *p2, float *col);
void		render_3d_line(float *p1, float *p2, float *col, float *view_mat, float *project_mat);
void		render_3d_point(float *p1, float *col, float *view_mat, float *project_mat);
void		render_3d_rectangle(float *p1, float *p2, float *col, float *view_mat, float *proj_mat);

///////////////////
// Box Draw
///////////////////

float			*create_bb_vertices(float *res, float *min, float *max);
unsigned int	*create_bb_indices(unsigned int *res);
void			render_box(float *vertices, unsigned int *indices, float *col, float *view, float *proj);

#endif
