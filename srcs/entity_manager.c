#include "entity_manager.h"
#include "shader.h"

void entity_manager_init(t_entity_manager *manager)
{
	manager->next_id = 0;
	manager->max_entities = 65535;
	manager->model_mats = malloc(sizeof(float) * manager->max_entities * 16);
	LG_INFO("model mats size : %d (* sizeof(float) ofc...)", manager->max_entities * 16);

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
		manager->slot_occupied = malloc(sizeof(char) * manager->entity_amount);
		memset(manager->slot_occupied, 0, sizeof(char) * manager->entity_amount);
	}

	// Get the first non slot taken;
	for (int i = 0; i < manager->entity_amount; i++)
	{
		// if the slot isnt taken, we have found a slot that doesnt have an entity in it yet;
		if (!manager->slot_occupied[i])
		{
			// Set slot taken;
			manager->slot_occupied[i] = 1;
			// Init it;
			entity_init(&manager->entities[i]);
			// Set id to it;
			manager->entities[i].id = manager->next_id;
			// Increment manager next id;
			manager->next_id++;
			// return it;
//			LG_INFO("Entity (%d) created", manager->entities[i].id);
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
		if (manager->slot_occupied[i] && manager->entities[i].id == id)
			return (&manager->entities[i]);
	}
	return (NULL);
}

/// @brief Draws all the entities;
/// @param manager 
/// @param camera 
void entity_manager_draw(t_entity_manager *manager, t_camera *camera)
{
	// Have an array for all the amounts for how many of the type needs to get rendered (for instance rendering);
	int type_to_render[ENTITY_AMOUNT];
	memset(type_to_render, 0, sizeof(int) * ENTITY_AMOUNT);

	// Array of all indices from 'manager->entities' that should get rendered;
	int indices_to_render[manager->max_entities];
	int total_indices = 0;

	// Decide which entities should get rendered;
	for (int i = 0; i < manager->entity_amount; i++)
	{
		// if the slot isnt occupied we can just skip it;
		if (!manager->slot_occupied[i])
			continue;

		t_entity *entity = &manager->entities[i];

		// If the entity is outside frustrum, we dont want to render it; or calculate matrices;
		if (v3_dist_sqrd(camera->pos, entity->pos) > camera->far_plane * camera->far_plane)
			continue;

		/* TODO : this needs to happen in the ai entity manager; or if the entity has a event function pointer in it;
		if (entity->ai)
			entity_event(entity, &world_info, &fps);
		*/

		// NOTE : The matrices of the entity gets updated in 'entity_update()', so one
		//	has to have this bool set for it to ever be called;
		if (entity->needs_update)
		{
			entity_update(entity);
			entity->needs_update = 0;
		}

		// increment the type;
		type_to_render[(int)entity->type]++;

		indices_to_render[total_indices] = i;
		total_indices++;
	}

	// How many of this type have we handled;
	int type_total[ENTITY_AMOUNT];
	memset(type_total, 0, sizeof(int) * ENTITY_AMOUNT);

	// Copy all the model matrices for all entities that are going to get rendered;
	for (int i = 0; i < total_indices; i++)
	{
		t_entity *entity = &manager->entities[indices_to_render[i]];
		int type = (int)entity->type;

		// Get offset;
		int type_offset = 0;
		if (type > 0)
			type_offset = type_to_render[type] + type_to_render[type - 1];

		int final_offset = (type_total[type] + (type_offset * 16)) * 16;

		// Copy model matrix of the entity;
		memcpy(manager->model_mats + final_offset, entity->model_mat, sizeof(float) * 16);
		type_total[type]++;

		// Debug
		float ent_pos2[3];
		float tmp[3];
		float tmp2[3];
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
		int type = i;
		
		// We dont have any entities of this type;
		if (type_to_render[type] == 0)
			continue ;

		// Get offset;
		int type_offset = 0;
		if (type > 0)
			type_offset = type_to_render[type] + type_to_render[type - 1];

		// Instanciated rendering;
		model_instance_render(&manager->entity_models[type], manager->model_instance_shader,
			manager->model_mats + (type_offset * 16), type_to_render[type],
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

void entity_manager_load_entity_objects(t_entity_manager *manager)
{
	manager->entity_objects = malloc(sizeof(t_bobj) * ENTITY_AMOUNT);
	manager->entity_models = malloc(sizeof(t_model_v2) * ENTITY_AMOUNT);
	new_shader(&manager->model_instance_shader, SHADER_PATH"model_instance.vs", SHADER_PATH"model_instance.fs");

	for (int i = 0; i < ENTITY_AMOUNT; i++)
	{
		bobj_load(&manager->entity_objects[i], g_entity_data[i].model_path);
		model_instance_from_bobj(&manager->entity_models[i],
			&manager->entity_objects[i], 0);
		model_update(&manager->entity_models[i]);
	}
	LG_INFO("All entity models loaded (%d)", ENTITY_AMOUNT);
}

