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