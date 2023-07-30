#ifndef ENTITY_MANAGER_H
# define ENTITY_MANAGER_H

# include "vox.h"
# include "bmath.h"
# include "camera.h"
# include "fps.h"
# include "entity.h"

typedef struct s_entity_manager
{
	// YOINKED FROM WORLD;
	int			*entity_palette;		// size : ENTITY_AMOUNT;
	t_bobj		*entity_objects;		// size : ENTITY_AMOUNT;
	t_model_v2	*entity_models;			// size : ENTITY_AMOUNT;
	GLuint		model_instance_shader;


	/// @brief The next id that is going to get given to an entity;
	int		next_id;
	int		max_entities;

	float	*model_mats;

	t_entity *entities;
	/// @brief Amount of t_entity allocated to 'this.entities';
	int entity_amount;
	/// @brief 0 / 1 if the slot in 'this.entities' has an entity in it; (size is the same as 'this.entity_amount');
	char *slot_taken;
}	t_entity_manager;

void entity_manager_init(t_entity_manager *manager);
t_entity *entity_manager_new_entity(t_entity_manager *manager);
t_entity *entity_manager_get_entity(t_entity_manager *manager, int id);
void entity_manager_draw(t_entity_manager *manager, t_camera *camera);

void	create_entity_at_world_pos(t_entity_manager *manager, float *world_pos, int entity_type);
void	world_update_entity_palette(t_entity_manager *manager);

#endif