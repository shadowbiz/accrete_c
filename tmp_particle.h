#ifndef SE_PARTICLE_H
#define SE_PARTICLE_H

#include "globals.h"
#include "array.h"

static seParticle *seParticleAllocate();
seParticle *newParticle(const vec3 *position, const seParticleType type);

void setParticleMass(seParticle *particle, const float mass);
void setParticleVelocity(seParticle *particle, const float x, const float y, const float z);
void setParticleAcceleration(seParticle *particle, const float x, const float y, const float z);
void setParticleDamping(seParticle *particle, const float damp);

float getParticleMass(const seParticle *particle);

void freeParticle(void *particle);

void seParticleUpdate(seParticle *particle);
void seParticleResetForce(seParticle *particle);
void seParticleAddForce(seParticle *particle, const vec3 *force);

bool getParticleAllocStatus();

static seParticleForceGenerator *seParticleForceGeneratorAllocate();
seParticleForceGenerator *newParticleForceGenerator();
void freeParticleForceGenerator(seParticleForceGenerator *generator);
void setParticleForceGeneratorGravityVec(seParticleForceGenerator *generator, const vec3 *gravity);
void setParticleForceGeneratorGravity(seParticleForceGenerator *generator, const float x, const float y, const float z);
void setParticleForceGeneratorVelocityDrag(seParticleForceGenerator *generator, const float d);
void seParticleForceGeneratorUpdateForce(seParticleForceGenerator *generator, seParticle *particle);

static seParticleForceData *seParticleForceDataAllocate();
static seParticleForceData *newParticleForceData(seParticle *particle, seParticleForceGenerator *forceGen);
static void freeParticleForceData(void *registry);

#endif