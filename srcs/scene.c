#include "vox.h"

void	create_scene(t_scene *scene)
{
	memset(scene, 0, sizeof(t_scene));

/*
	scene->entity_amount = 0;
	scene->entities_allocated = 128;
	scene->entities = malloc(scene->entities_allocated * sizeof(t_entity));
	*/
}

/*
 * NOTE: doesnt free it.
*/
void	remove_scene(t_scene *scene)
{
	(void)scene;
	// free entities
}

/*
 * Returns index where the entity was placed;
*/
size_t	add_entity_to_scene(t_scene *scene, t_vox_entity *entity)
{
	size_t	increase_amount = 5;

	if (!entity)
		LG_ERROR("no entity");
	scene->entity_amount += 1;
	// If not enough allocated, allocate more...
	if (scene->entity_amount >= scene->entities_allocated)
	{
		scene->entities_allocated += increase_amount;
		scene->entities = realloc(scene->entities,
			scene->entities_allocated * sizeof(t_vox_entity *));
		for (size_t i = 0; i < increase_amount; i++)
			scene->entities[scene->entities_allocated - increase_amount + i] = NULL;
		LG_INFO("Making entities array bigger. (%d to %d)",
			scene->entities_allocated - increase_amount, scene->entities_allocated);
	}
	// Save entity in the next open array slot;
	size_t	open = 0;
	for (; open < scene->entities_allocated; open++)
		if (!scene->entities[open])
			break ;
	if (open >= scene->entities_allocated)
		LG_ERROR("First empty entities slot > entities_allocated.");
	scene->entities[open] = entity;
	entity->id = open;
	LG_INFO("Entity added to scene (id : %d)", open);
	return (open);
}

/*
 * NOTE: Doesnt free the entity though;
*/

void	remove_entity_from_scene_with_index(t_scene *scene, size_t index)
{
	if (index >= scene->entities_allocated)
		LG_ERROR("Trying to remove index %d, which is >= entities_allocated.", index);
	if (!scene->entities[index])
		LG_ERROR("Entity index %d doesnt exist. (array slot empty)", index);

	scene->entities[index] = NULL;
	scene->entity_amount -= 1;
}

void	remove_entity_from_scene(t_scene *scene, t_vox_entity *entity)
{
	size_t	index;

	index = 0;
	for (; index < scene->entities_allocated; index++)
		if (scene->entities[index] == entity)
			break ;
	remove_entity_from_scene_with_index(scene, index);
}

t_vox_entity	*get_scene_entity(t_scene *scene, size_t index)
{
	if (index >= scene->entities_allocated)
	{
		LG_WARN("Index goes over amount allocated (%d - %d).", index, scene->entities_allocated);
		return (NULL);
	}
	if (!scene->entities[index])
		LG_WARN("Entity youre trying to get is NULL, but whatever, we are returning it.");
	return (scene->entities[index]);
}
