#include "shaderpixel.h"

/*
 * Check player to entity collision;
 *
 * Checks if player is moving;
 * Checks if player aabb is inside entity aabb.
 * Checks if player.velocity intersects all triangles in entity collision mesh.
 * 
 * TODO: entity collision mesh, we currently check against the triangles the aabb
 *		is made of;
*/
void	player_entity_collision(t_player *player, t_entity *entity)
{
	float p1[VEC3_SIZE];
	float p2[VEC3_SIZE];
	float p3[VEC3_SIZE];
	unsigned int index = 0;
	
	float	intersect_point[3];
	float	normed[3];
	int		player_moving = (player->velocity[0] || player->velocity[1] || player->velocity[2]);
	int		aabb_collision = aabb_aabb_collision(&player->aabb, &entity->aabb);
	int		triangle_collision = 0;

	if (player_moving && aabb_collision)
	{
		for (int triangle = 0; triangle < 12; triangle++)
		{
			index = entity->bb_indices[triangle * 3 + 0] * 3;
			new_vec3(p1,
				entity->bb_vertices[index + 0],
				entity->bb_vertices[index + 1],
				entity->bb_vertices[index + 2]
			);
			index = entity->bb_indices[triangle * 3 + 1] * 3;
			new_vec3(p2,
				entity->bb_vertices[index + 0],
				entity->bb_vertices[index + 1],
				entity->bb_vertices[index + 2]
			);
			index = entity->bb_indices[triangle * 3 + 2] * 3;
			new_vec3(p3,
				entity->bb_vertices[index + 0],
				entity->bb_vertices[index + 1],
				entity->bb_vertices[index + 2]
			);

			if (ray_triangle_intersect(player->camera.pos,
					vec3_normalize(normed, player->velocity),
					p1, p2, p3, intersect_point))
			{
				triangle_collision = 1;
				break ;
			}
		}
		if (triangle_collision)
		{
			float	new_pos[3];
			vec3_add(new_pos, player->camera.pos, player->velocity);
			if (vec3_dist(player->camera.pos, new_pos) >
				vec3_dist(player->camera.pos, intersect_point))
			{
				entity->collision = 1;
				//vec3_sub(player.velocity, new_pos, intersect_point);
				new_vec3(player->velocity, 0, 0, 0);
			}
		}
	}
}

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
	node = new->content;
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
	ft_lstdel(&detector->nodes, &free_collision_node);
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
