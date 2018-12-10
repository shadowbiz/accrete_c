#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "globals.h"
#include "array.h"
#include "renderercomponent.h"

static seEntity *seEntityAllocate();
seEntity *newEntity();

void seEntityInit(seEntity *entity);

void freeEntity(seEntity *entity);

void seEntityRender(seEntity *entity);
void seEntityUpdate(seEntity *entity);

void *getEntityComponentType(seEntity *entity, const seComponentType type);

void seEntityAddComponent(seEntity *entity, void *component, const seComponentType type);

void setEntityPositionVec(seEntity *entity, const vec3 *position);
void setEntityPosition(seEntity *entity, const float x, const float y, const float z);
void setEntityScale(seEntity *entity, const float s);

bool getEntityAllocStatus();

#endif // !_ENTITY_H_
