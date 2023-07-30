#include "entity_manager.h"

void entity_manager_init(t_entity_manager *manager)
{
	manager->next_id = 0;
	manager->max_entities = 500;
	manager->model_mats = malloc(sizeof(float *) * manager->max_entities * 16);

	manager->entity_amount = 0;
	manager->entities = NULL;
}

/// @brief Creates entity and returns it;
/// @param manager 
/// @return 
t_entity *entity_manager_new_entity(t_entity_manager *manager)
{
	// TODO : at some point change this to batching;
	// If we dont have anything in the entities array yet, we create max amount of entities there;
	// we also set the slot taken, and memset it to 0 since we have just created a new one;
	if (manager->entities == NULL)
	{
		manager->entities = malloc(sizeof(t_entity) * manager->max_entities);
		manager->entity_amount = manager->max_entities;
		manager->slot_taken = malloc(sizeof(char) * manager->entity_amount);
		memset(manager->slot_taken, 0, sizeof(char) * manager->entity_amount);
	}

	// Get the first non slot taken;
	for (int i = 0; i < manager->entity_amount; i++)
	{
		// if the slot isnt taken, we have found a slot that doesnt have an entity in it yet;
		if (!manager->slot_taken[i])
		{
			// Init it;
			entity_init(&manager->entities[i]);
			// Set id to it;
			manager->entities[i].id = manager->next_id;
			// Increment manager next id;
			manager->next_id++;
			// return it;
			return (&manager->entities[i]);
		}
	}	

	LG_WARN("Should really never come here");
	return (NULL);
}

/// @brief Returns an entity with the id of 'id';
/// @param manager 
/// @param id 
/// @return 
t_entity *entity_manager_get_entity(t_entity_manager *manager, int id)
{
	for (int i = 0; i < manager->entity_amount; i++)
	{
		// check first if the entity slot has something in it, then check that its the correct id;
		if (manager->slot_taken[i] && manager->entities[i].id == id)
			return (&manager->entities[i]);
	}
	return (NULL);
}

void entity_manager_draw(t_entity_manager *manager, t_camera *camera)
{
	// Have an array for all the amounts for how many of the type needs to get rendered (for instance rendering);
	int type_to_render[ENTITY_AMOUNT];
	memset(type_to_render, 0, sizeof(int) * ENTITY_AMOUNT);
	// Decide which entities should get rendered;
	for (int i = 0; i < manager->entity_amount; i++)
	{
		// if the slot isnt taken we can just skip it;
		if (!manager->slot_taken[i])
			continue;

		t_entity *entity = &manager->entities[i];

		// Decide if the entity should even be rendered;
		// TODO : if its too far away, it should probably despawn;
		if (v3_dist_sqrd(camera->pos, entity->pos) > camera->far_plane * camera->far_plane)
			continue;

		/*
		if (entity->ai)
			entity_event(entity, &world_info, &fps);
		*/

		if (entity->needs_update)
		{
			entity_update(entity);
			entity->needs_update = 0;
		}

		// increment the type;
		type_to_render[(int)entity->type]++;
	}

	int type_offset[ENTITY_AMOUNT];
	memset(type_offset, 0, sizeof(int) * ENTITY_AMOUNT);
	// Create offset array;
	for (int i = 0; i < ENTITY_AMOUNT; i++)
	{
		// just skip the first, doesnt need an offset;
		if (i == 0)
			continue;

		type_offset[i] = type_to_render[i] + type_to_render[i - 1];
	}

	float ent_pos2[3];
	float tmp[3];
	float tmp2[3];

	int type_total[ENTITY_AMOUNT];
	memset(type_total, 0, sizeof(int) * ENTITY_AMOUNT);
	// Create model matrices for all entities;
	for (int i = 0; i < manager->entity_amount; i++)
	{
		if (!manager->slot_taken[i])
			continue;

		t_entity *entity = &manager->entities[i];
		// Set the model matrix;
		model_matrix(manager->model_mats + (type_total[(int)entity->type] + type_offset[(int)entity->type]) * 16,
			entity->scale_m4, entity->rot_m4, entity->trans_m4);
		type_total[(int)entity->type]++;

		// Debug
		if (entity->draw_dir)
		{
			// Front
			v3_multiply_f(ent_pos2, entity->front, 1.0f);
			v3_add(ent_pos2, ent_pos2, entity->pos);
			render_3d_line(entity->pos, ent_pos2, (float []){0, 0, 255}, camera->view, camera->projection);
			// Up
			v3_multiply_f(ent_pos2, (float []){0, 1, 0}, 1.0f);
			v3_add(ent_pos2, ent_pos2, entity->pos);
			render_3d_line(entity->pos, ent_pos2, (float []){255, 0, 0}, camera->view, camera->projection);
		}

		if (entity->draw_aabb)
		{
			render_3d_rectangle(v3_add(tmp, entity->pos,
				manager->entity_models[(int)entity->type].bound.min),
				v3_add(tmp2, entity->pos, manager->entity_models[(int)entity->type].bound.max),
				(float []){255, 0, 0}, camera->view, camera->projection);
		}
	}
	for (int i = 0; i < ENTITY_AMOUNT; i++)
	{
		// We dont have any entities of this type;
		if (type_to_render[i] == 0)
			continue ;

		model_instance_render(&manager->entity_models[i], manager->model_instance_shader,
			manager->model_mats + type_offset[i], type_to_render[i],
			camera->view, camera->projection);
	}
}

void	create_entity_at_world_pos(t_entity_manager *manager, float *world_pos, int entity_type)
{
	t_entity *entity = entity_manager_new_entity(manager); 
	entity->type = entity_type;
	v3_assign(entity->pos, world_pos);

	entity_update(entity);

	LG_INFO("Entity (%d) added at %f %f %f",
		entity->id, entity->pos[0], entity->pos[1], entity->pos[2]);
}

