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
	int		aabb_collision = aabb_aabb_collision(&player->aabb, &entity->aabb);
	int		triangle_collision = 0;

	entity->collision = 0;
	if (player->moving && aabb_collision)
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
				player->colliding = 1;
				//vec3_sub(player.velocity, new_pos, intersect_point);
				new_vec3(player->velocity, 0, 0, 0);
			}
		}
	}
}

void	player_entity_collision_precise(t_player *player, t_entity *entity)
{
	float p1[VEC3_SIZE];
	float p2[VEC3_SIZE];
	float p3[VEC3_SIZE];
	unsigned int index = 0;

	float	player_pos[VEC3_SIZE];
	float	player_front[VEC3_SIZE];
	float	velocity[VEC3_SIZE];

	float	intersect_point[3];
	float	normed[3];
	int		triangle_collision = 0;
	int		aabb_collision;

	entity->collision = 0;
	if (!player->moving)
		return ;
	aabb_collision = aabb_aabb_collision(&player->aabb, &entity->aabb);
	if (!aabb_collision)
	{
		aabb_collision = point_aabb_collision(player->camera.pos, &entity->aabb);
		if (!aabb_collision)
		{
			LG_WARN("player not inside entity aabb (%d)", aabb_collision);
			return ;
		}
	}

	// Convert player pos and velocity to local coords of the entity;
	float	v3[VEC3_SIZE];
	float	trans_mat[MAT3_SIZE];
	float	inverse_trans_mat[MAT3_SIZE];

	mat4_to_mat3(trans_mat, entity->model_mat);

	mat3_identity(inverse_trans_mat);
	mat3_inverse(inverse_trans_mat, trans_mat);

	vec3_multiply_mat3(player_pos, player->camera.pos, inverse_trans_mat);
	vec3_multiply_mat3(velocity, player->velocity, inverse_trans_mat);
	vec3_normalize(velocity, velocity);

	vec3_multiply_mat3(player_front, player_front, inverse_trans_mat);
	vec3_normalize(player_front, player_front);
				/*
	for (int mesh_index = 0; mesh_index < entity->model.info_amount; mesh_index++)
	{
		for (int elem_index = 0; elem_index < entity->model.info[mesh_index].mesh.element_amount; elem_index++)
		{
			for (size_t face_index = 0; face_index < entity->model.info[mesh_index].mesh.elements[elem_index].index_amount; face_index++)
			{
				float			*vertices = entity->model.info[mesh_index].mesh.vertices;
				unsigned int	*indices = entity->model.info[mesh_index].mesh.elements[elem_index].indices;
				*/
				int face_index = 0; // remove
				float			*vertices = entity->model.info[0].mesh.vertices;
				unsigned int	*indices = entity->model.info[0].mesh.elements[0].indices;

				index = indices[face_index * 3 + 0] * 3;
				new_vec3(p1,
					vertices[index + 0], vertices[index + 1], vertices[index + 2]);
				index = indices[face_index * 3 + 1] * 3;
				new_vec3(p2,
					vertices[index + 0], vertices[index + 1], vertices[index + 2]);
				index = indices[face_index * 3 + 2] * 3;
				new_vec3(p3,
					vertices[index + 0], vertices[index + 1], vertices[index + 2]);

/*
				if (ray_triangle_intersect(player_pos, velocity,
						p1, p2, p3, intersect_point))
					triangle_collision = 1;
					*/
				if (ray_triangle_intersect(player_pos, player_front,
						p1, p2, p3, intersect_point))
					triangle_collision = 1;

/*
				*/
				vec3_multiply_mat3(p1, p1, trans_mat);
				vec3_multiply_mat3(p2, p2, trans_mat);
				vec3_multiply_mat3(p3, p3, trans_mat);
				glDisable(GL_DEPTH_TEST);
				render_3d_line(p1, p2, (float []){0, 1, 0}, player->camera.view, player->camera.projection);
				render_3d_line(p1, p3, (float []){0, 1, 0}, player->camera.view, player->camera.projection);
				render_3d_line(p2, p3, (float []){0, 1, 0}, player->camera.view, player->camera.projection);
				/*
			}
		}
	}
	*/
	if (triangle_collision)
	{
		float	new_pos[3];
		vec3_add(new_pos, player->camera.pos, player->velocity);
		vec3_multiply_mat3(new_pos, new_pos, inverse_trans_mat);

		entity->collision = 1;
		if (vec3_dist(player_pos, new_pos) >
			vec3_dist(player_pos, intersect_point))
		{
			entity->collision = 1;
			player->colliding = 1;
			//vec3_sub(player.velocity, new_pos, intersect_point);
			new_vec3(player->velocity, 0, 0, 0);
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
		aabb_create(&node->aabb, node->entity->model.info->mesh.vertices,
			node->entity->model.info->mesh.vertex_amount);
		aabb_transform(&node->aabb, node->entity->model_mat);
		node->collided = 0;
		curr = curr->next;
	}
}
