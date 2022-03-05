#include "shaderpixel.h"

void	new_collision_detector(t_collision_detector *detector)
{
	(void)detector;
}

/*
 * NOTE:
 *	the aabbs are created in the event handling loop;
*/
void	add_collision_entity(t_collision_detector *detector, t_entity *entity)
{
	t_collision_node	*node;
	t_list				*new;

	if (!entity)
	{
		LG_WARN("Youre giving NULL entity.");
		return ;
	}
	new = ft_lstnew(0, 0);
	node = &(new->content);
	node->entity = entity;
}

void	remove_collision_entity(t_collision_detector *detector, t_entity *entity)
{
	t_list	*curr;

	curr = detector->nodes;
	while (curr)
	{
		if (entity == curr->content)
		{
			ft_lstdelone(&detector->nodes, &free_collision_node);
			return ;
		}
		curr = curr->next;
	}
}

void	free_collision_detector(t_collision_detector *detector)
{
	ft_lstdel(detector->nodes, &free_collision_node);
}

void	free_collision_node(void *pointer, size_t size)
{
	t_collision_node	*node;

	node = pointer;
	if (!node)
		return ;
	(void)size;
	node->entity = NULL;
//	free(node);
}

void	update_collision_detector(t_collision_detector *detector)
{
	t_list				*curr;
	t_collision_node	*node;

	curr = detector->nodes;
	while (curr)
	{
		node = curr->content;
		aabb_create(&node->aabb, node->entity->model->info->mesh.vertices,
			node->entity->model->info->mesh.vertex_amount);
		aabb_transform(&node->aabb, node->entity->model_mat);
		node->collided = 0;
		curr = curr->next;
	}
}

void	player_entity_collision(t_player *player, t_entity *entity)
{
	(void)player;
	(void)entity;
}
