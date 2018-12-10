#ifndef SE_LIGHT_H
#define SE_LIGHT_H

#include "structs.h"
#include "math.h"

static seLight *LightAllocate();
void freeLight(seLight *light);
seLight *newLight();

void setLightPosition(seLight *light, const vec3 *position);
void setLightColor(seLight *light, const vec3 *color);
bool getLightAllocateStatus();

#endif // !LIGHT_H