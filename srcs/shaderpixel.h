/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shaderpixel.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/11 14:27:43 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/17 10:59:27 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SHADERPIXEL_H
# define SHADERPIXEL_H

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

# define DEBUG 0

# define Uint32	unsigned int

# define ROOT_PATH "./"
# define MODEL_PATH ROOT_PATH"models/"
# define SHADER_PATH ROOT_PATH"shaders/"
# define TEXTURE_PATH ROOT_PATH"textures/"
# define SAVE_PATH ROOT_PATH"saves/"

# define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])

typedef struct	s_player	t_player;
typedef struct	s_fps		t_fps;
typedef struct	s_key		t_key;

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

///////////////////
//	ENTITY
///////////////////

typedef struct	s_entity
{
	size_t			id;

	float			pos[VEC3_SIZE];
	float			rot_x_angle;
	float			rot_y_angle;
	float			rot_z_angle;
	float			scale_value;

	float			scale_mat[MAT4_SIZE];
	float			rot_mat[MAT4_SIZE];
	float			trans_mat[MAT4_SIZE];
	float			model_mat[MAT4_SIZE];

	int				render_aabb;
	int				collision_detection_enabled;
	int				collision_use_precise;
	t_aabb			aabb;
	float			bb_vertices[24];
	unsigned int	bb_indices[36];
	int				collision;

	int				show_normal_map;

	t_model			model;
}	t_entity;

void				new_entity(t_entity *entity);
void				entity_print(t_entity *entity);
void				update_entity(t_entity *entity);
void				render_entity(t_entity *entity, t_camera *camera, t_model *model, GLuint shader);

void				new_model_matrix(float *m4_res, float scale, float *v3_rot, float *v3_pos);;

///////////////////
//	PLAYER
///////////////////

struct s_player
{
	t_camera	camera;

	float		velocity[VEC3_SIZE];
	float		gravity;

	int			moving;
	int			colliding;
	int			enabled_mouse;
	double		mouse_pos[2];
	double		last_mouse_pos[2];

	t_aabb		aabb;

};

void		new_player(t_player *player);
void		player_events(t_player *player, t_key *keys, GLFWwindow *win);
void		player_movement(t_player *player, GLFWwindow *win, t_fps fps);
void		player_apply_velocity(t_player *player);
void		player_apply_gravity(t_player *player);
void		player_looking(t_player *player, GLFWwindow *win, t_fps fps);
void		player_entity_collision(t_player *player, t_entity *entity);
int			player_entity_mesh_collision(t_player *player, t_entity *entity);
void		player_print(t_player *player);

///////////////////
//	FPS
///////////////////

struct	s_fps
{
	float			last_time;
	float			curr_time;
	float			delta_time;
	int				fps;
	int				count;
};

void		new_fps(t_fps *fps);
void		update_fps(t_fps *fps);

///////////////////
//	KEYS
///////////////////

enum e_mouse_button_state
{
	BUTTON_PRESS, // the framei t was pressed;
	BUTTON_HOLD, // if held for more than a frame;
	BUTTON_RELEASE, // the frame it was released;
	BUTTON_NONE // if not one of the above (aka not pressed?);
};

struct	s_key
{
	int		state;
};

void		update_all_keys(t_key *keys, t_key *mouse, GLFWwindow *win);

///////////////////
//	SCENE
///////////////////

/*
 * entity_allocated : size of the entities array;
 * entity_amount : amount of entities in entities array:
 * note: we can have empty spots in the entities array thats why they are separate;
*/
typedef struct	s_scene
{
	t_entity	**entities;
	size_t		entities_allocated;
	size_t		entity_amount;
}		t_scene;

void		create_scene(t_scene *scene);
void		remove_scene(t_scene *scene);
void		remove_entity_from_scene_with_index(t_scene *scene, size_t index);
void		remove_entity_from_scene(t_scene *scene, t_entity *entity);
size_t		add_entity_to_scene(t_scene *scene, t_entity *entity);
t_entity	*get_scene_entity(t_scene *scene, size_t index);

///////////////////
//	SHADERPIXEL
///////////////////

typedef struct	s_shaderpixel
{
	GLFWwindow	*win;
	int			win_w;
	int			win_h;
	int			polygon_mode;
	int			pilpalpol;
}	t_shaderpixel;

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

///////////////////
// Box Draw
///////////////////

float			*create_bb_vertices(float *res, float *min, float *max);
unsigned int	*create_bb_indices(unsigned int *res);
void			render_box(float *vertices, unsigned int *indices, float *col, float *view, float *proj);

#endif
