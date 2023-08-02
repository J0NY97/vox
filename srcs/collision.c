/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   collision.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jsalmi <jsalmi@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/20 12:04:28 by jsalmi            #+#    #+#             */
/*   Updated: 2022/06/20 12:04:29 by jsalmi           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vox.h"
#include "player.h"

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
	float p1[V3_SIZE];
	float p2[V3_SIZE];
	float p3[V3_SIZE];
	unsigned int index = 0;

	float	intersect_point[3];
	float	normed[3];
	int		aabb_collision = aabb_aabb_collision(&player->aabb, &entity->aabb);
	int		triangle_collision = 0;
	float	dist;

	entity->collision = 0;
	if (player->moving && aabb_collision)
	{
		for (int triangle = 0; triangle < 12; triangle++)
		{
			index = entity->aabb_indices[triangle * 3 + 0] * 3;
			v3_new(p1,
				entity->aabb_vertices[index + 0],
				entity->aabb_vertices[index + 1],
				entity->aabb_vertices[index + 2]
			);
			index = entity->aabb_indices[triangle * 3 + 1] * 3;
			v3_new(p2,
				entity->aabb_vertices[index + 0],
				entity->aabb_vertices[index + 1],
				entity->aabb_vertices[index + 2]
			);
			index = entity->aabb_indices[triangle * 3 + 2] * 3;
			v3_new(p3,
				entity->aabb_vertices[index + 0],
				entity->aabb_vertices[index + 1],
				entity->aabb_vertices[index + 2]
			);

			if (ray_triangle_intersect(player->camera->pos,
					v3_normalize(normed, player->velocity),
					p1, p2, p3, intersect_point, &dist))
			{
				triangle_collision = 1;
				break ;
			}
		}
		if (triangle_collision)
		{
			float	new_pos[3];
			v3_add(new_pos, player->camera->pos, player->velocity);
			if (v3_dist(player->camera->pos, new_pos) >
				v3_dist(player->camera->pos, intersect_point))
			{
				entity->collision = 1;
				player->colliding = 1;
				//v3_sub(player.velocity, new_pos, intersect_point);
				v3_new(player->velocity, 0, 0, 0);
			}
		}
	}
}

/*
 * Checks collision with player and entity mesh;
*/
int	player_entity_mesh_collision(t_player *player, t_entity *entity)
{
	// Convert player world position to entity local position;
	//		Get inverse transformation matrix of entity model;
	float	inverse_trans[M4_SIZE];

	m4_identity(inverse_trans);
	m4_inverse(inverse_trans, entity->model_mat);

	//		Apply inverse transformation matrix on player position;
	float	local_player_pos[V4_SIZE];

	v3_to_v4(local_player_pos, player->camera->pos);
	v4_multiply_m4(local_player_pos, local_player_pos, inverse_trans);

	// Compare player to all the meshes in entity model;
	float	intersection_p[V3_SIZE];
	float	*vertices;
	unsigned int	*indices;
	float	p1[4];
	float	p2[4];
	float	p3[4];
	float	dist;

	entity->collision = 0;
	for (int i = 0; i < entity->model->info_amount; i++)
	{
		vertices = entity->model->info[i].mesh.vertices;
		for (int j = 0; j < entity->model->info[i].mesh.element_amount; j++)
		{
			indices = entity->model->info[i].elem_info[j].element.indices;
			for (int k = 0; k < entity->model->info[i].elem_info[j].element.index_amount / 3; k++)
			{
				int ind = k * 3;
				v3_new(p1,
					vertices[indices[ind + 0] * 3 + 0],
					vertices[indices[ind + 0] * 3 + 1],
					vertices[indices[ind + 0] * 3 + 2]);
				v3_new(p2,
					vertices[indices[ind + 1] * 3 + 0],
					vertices[indices[ind + 1] * 3 + 1],
					vertices[indices[ind + 1] * 3 + 2]);
				v3_new(p3,
					vertices[indices[ind + 2] * 3 + 0],
					vertices[indices[ind + 2] * 3 + 1],
					vertices[indices[ind + 2] * 3 + 2]);
				if (ray_triangle_intersect(local_player_pos, player->camera->front,
					p3, p2, p1, intersection_p, &dist))
				{
					entity->collision = 1;

				}
					glDisable(GL_DEPTH_TEST);
					v4_multiply_m4(p1, p1, entity->model_mat);
					v4_multiply_m4(p2, p2, entity->model_mat);
					v4_multiply_m4(p3, p3, entity->model_mat);
					render_3d_line(p1, p2, (float []){0, 0, 1}, player->camera->view, player->camera->projection);
					render_3d_line(p1, p3, (float []){0, 0, 1}, player->camera->view, player->camera->projection);
					render_3d_line(p2, p3, (float []){0, 0, 1}, player->camera->view, player->camera->projection);
					glEnable(GL_DEPTH_TEST);
			}
		}
	}

	// Return intersection;
	return (entity->collision);
}