#include "vox_entity.h"

# include "bmath.h"

void	vox_entity_new(t_vox_entity *entity)
{
	v3_new(entity->pos, 0, 0, 0);
	v3_new(entity->rot, 90, 0, 0);
	entity->scale = 1.0f;
	entity->type = 0;
}