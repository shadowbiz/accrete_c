#ifndef _RENDERER_COMPONENT_H_
#define _RENDERER_COMPONENT_H_

#include "globals.h"
#include "material.h"

seRenderer *seRendererAllocate();
seRenderer *newRenderer(seEntity *entity, Mesh *mesh, Material *material, seCamera *camera, seLight *light);

void freeRenderer(seRenderer *renderer);

void seRendererUpdate(seEntity *entity);

void seRendererRender(seEntity *entity);
void seRendererRenderAt(seEntity *entity, const vec3 *position);

bool getRendererAllocStatus();

#endif // !_RENDERER_COMPONENT_H_
