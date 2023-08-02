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

/// @brief Draws all the entities that are active;
/// @param manager 
/// @param camera 
void entity_manager_draw(t_entity_manager *manager, t_camera *camera)
{
	LG_INFO("START");

	// Have an array for all the amounts for how many of the type needs to get rendered (for instance rendering);
	int type_to_render[ENTITY_AMOUNT];
	memset(type_to_render, 0, sizeof(int) * ENTITY_AMOUNT);
	// Decide which entities should get rendered;
	for (int i = 0; i < manager->entity_amount; i++)
	{
		// if the slot isnt occupied we can just skip it;
		if (!manager->slot_occupied[i])
			continue;

		t_entity *entity = &manager->entities[i];

		// If the entity is outside frustrum, we dont want to render it;
		/*
		if (v3_dist_sqrd(camera->pos, entity->pos) > camera->far_plane * camera->far_plane)
			continue;
			*/

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
	}

//	vNi_string("typeToRender: ", type_to_render, ENTITY_AMOUNT);

	int type_total[ENTITY_AMOUNT];
	memset(type_total, 0, sizeof(int) * ENTITY_AMOUNT);
	// Create model matrices for all entities;
	// NOTE : The entities dont come in the correct order; (sorted by the type i mean);
	for (int i = 0; i < manager->entity_amount; i++)
	{
		if (!manager->slot_occupied[i])
			continue;

		t_entity *entity = &manager->entities[i];

		// Get offset;
		int type_offset = 0;
		if ((int)entity->type > 0)
			type_offset = type_to_render[(int)entity->type] + type_to_render[(int)entity->type - 1];

		int final_offset = (type_total[(int)entity->type] + (type_offset * 16)) * 16;

//		printf("final_offset : %d (%d %d)\n", final_offset, type_total[(int)entity->type], type_offset);

		// Copy model matrix of the entity;
		memcpy(manager->model_mats + final_offset, entity->model_mat, sizeof(float) * 16);
		// Set the model matrix;
		/*
		model_matrix(manager->model_mats + final_offset,
			entity->scale_mat, entity->rot_mat, entity->trans_mat);
			*/
		type_total[(int)entity->type]++;

//		m4_string("manager->models_mats : ", manager->model_mats + final_offset);

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

	int total = 0;
	for (int i = 0; i < ENTITY_AMOUNT; i++)
	{
		// We dont have any entities of this type;
		if (type_to_render[i] == 0)
			continue ;

		// Get offset;
		int type_offset = 0;
		if (i > 0)
			type_offset = type_to_render[i] + type_to_render[i - 1];

/*
		LG_INFO("type_offset : %d", type_offset);
		LG_INFO("We want to render model mats from %d to %d", type_offset * 16, (type_offset * 16) + 16 * type_to_render[i]);
		LG_INFO("is this inside the size of 'model_mats' ? %d", manager->max_entities * 16);

		LG_INFO("%d entity model mesh amount", manager->entity_models[i].meshes_amount);
		LG_INFO("%d shader", manager->model_instance_shader);
		LG_INFO("type[%d] : %d to render", i, type_to_render[i]);
		*/
		
		// Instanciated rendering;
		model_instance_render(&manager->entity_models[i], manager->model_instance_shader,
			manager->model_mats + (type_offset * 16), type_to_render[i],
			camera->view, camera->projection);
		
		total += type_to_render[i];
	}

	LG_INFO("%d entities rendered", total);
	LG_INFO("END");
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

