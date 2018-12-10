#ifndef SE_SHOT_H
#define SE_SHOT_H

#include "globals.h"
#include "tmp_particle.h"
//#include "shape.h"

static seParticleEmitter *seEmitterAllocate();
seParticleEmitter *newEmitter(const vec3 *position, const seParticleType type);
void freeEmitter(seParticleEmitter *shot);

bool getEmitterAllocStatus();

void seEmitterRender(seParticleEmitter *emitter, seShape *shape);
void seEmitterUpdate(seParticleEmitter *emitter);

void seEmitterAddParticle(seParticleEmitter *emitter, const vec3 *position, const seParticleType type);
void seEmitterRemoveParticle(seParticleEmitter *emitter, seParticle *particle);

void seEmitterClearDefaultData();

#endif // !SE_SHOT_H
