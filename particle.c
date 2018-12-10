#include "tmp_particle.h"

static uint allocs = 0;

seParticle *newParticle(const vec3 *position, const seParticleType type)
{
	seParticle *particle = seNew(seParticle, 1);

	if (!particle)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	allocs++;

	vec3SetZero(&particle->acceleration);
	vec3SetZero(&particle->velocity);
	vec3SetZero(&particle->position);
	vec3SetZero(&particle->forceAccum);

	particle->type = type;

	particle->damping = 1.0;
	particle->inverseMass = 1.0;

	particle->bornTime = glfwGetTime();

	particle->AddForce = seParticleAddForce;
	particle->ResetForce = seParticleResetForce;
	particle->Update = seParticleUpdate;

	vec3Copy(&particle->position, position);
	particle->position.z = 2.0;

	switch (type)
	{
	case PISTOL:
		vec3Set(&particle->velocity, 0.0, 35.0, 0.0);
		particle->damping = 0.99;
		setParticleMass(particle, 2.0);
		break;

	case ARTILLERY:
		vec3Set(&particle->velocity, 0.0, 25.0, 12.0);
		particle->damping = 0.99;
		setParticleMass(particle, 200.0);
		break;

	case FIREBALL:
		vec3Set(&particle->velocity, 10.0, 0.0, 0.0);
		particle->damping = 0.9;
		setParticleMass(particle, 1.0);
		break;

	case LASER:
		vec3Set(&particle->velocity, 100.0, 0.0, 0.0);
		particle->damping = 0.99;
		setParticleMass(particle, 0.1);
		break;

	case EXPLOSION:
		vec3Set(&particle->velocity, seRandomNumber(-1.0, 1.0), seRandomNumber(-1.0, 1.0), seRandomNumber(0.0, 1.0));
		vec3Normalize(&particle->velocity);
		vec3MultScalar(&particle->velocity, &particle->velocity, seRandomNumber(1.0, 15.0));
		particle->damping = 0.69;
		particle->position.z = 0.1;
		setParticleMass(particle, seRandomNumber(5.0, 70.0));
		break;
	}

	return particle;
}

void setParticleMass(seParticle *particle, const float mass)
{
	if (mass <= 0)
		return;
	particle->inverseMass = 1.0 / mass;
}

void setParticleVelocity(seParticle *particle, const float x, const float y, const float z)
{
	vec3Set(&particle->velocity, x, y, z);
}

void setParticleAcceleration(seParticle *particle, const float x, const float y, const float z)
{
	vec3Set(&particle->acceleration, x, y, z);
}

void setParticleDamping(seParticle *particle, const float damp)
{
	particle->damping = damp;
}

float getParticleMass(const seParticle *particle)
{
	return 1.0 / particle->inverseMass;
}

void freeParticle(void *particle)
{
	if (!particle)
		return;
	free(particle);
	particle = NULL;
	allocs--;
}

void seParticleUpdate(seParticle *particle)
{
	vec3 tmp, tmp1;

	vec3MultScalar(&tmp, &particle->velocity, DELTA_TIME);
	vec3Add(&particle->position, &particle->position, &tmp);

	vec3MultScalar(&tmp, &particle->forceAccum, particle->inverseMass);
	vec3Add(&tmp1, &particle->acceleration, &tmp);

	vec3MultScalar(&tmp, &tmp1, DELTA_TIME);
	vec3Add(&particle->velocity, &particle->velocity, &tmp);

	vec3MultScalar(&particle->velocity, &particle->velocity, powf(particle->damping, DELTA_TIME));
}

void seParticleResetForce(seParticle *particle)
{
	vec3SetZero(&particle->forceAccum);
}

void seParticleAddForce(seParticle *particle, const vec3 *force)
{
	vec3Add(&particle->forceAccum, &particle->forceAccum, force);
}

bool getParticleAllocStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Particle allocations left %i\n", allocs);
		return false;
	}
	return true;
}

static seParticleForceGenerator *seParticleForceGeneratorAllocate()
{
	return (seParticleForceGenerator *)calloc(1, sizeof(seParticleForceGenerator));
}

seParticleForceGenerator *newParticleForceGenerator()
{
	seParticleForceGenerator *result = seParticleForceGeneratorAllocate();

	result->UpdateForce = seParticleForceGeneratorUpdateForce;
	result->SetDrag = setParticleForceGeneratorVelocityDrag;
	result->SetGravity = setParticleForceGeneratorGravity;
	result->SetGravityVec = setParticleForceGeneratorGravityVec;

	result->hasGravity = false;
	result->hasDrag = false;

	return result;
}

void freeParticleForceGenerator(seParticleForceGenerator *generator)
{
	if (!generator)
		return;
	free(generator);
}

void setParticleForceGeneratorGravityVec(seParticleForceGenerator *generator, const vec3 *gravity)
{
	vec3Copy(&generator->gravity, gravity);
	generator->hasGravity = !(gravity->x == 0.0 && gravity->y == 0.0 && gravity->z == 0.0);
}

void setParticleForceGeneratorGravity(seParticleForceGenerator *generator, const float x, const float y, const float z)
{
	vec3Set(&generator->gravity, x, y, z);
	generator->hasGravity = !(x == 0.0 && y == 0.0 && z == 0.0);
}

void setParticleForceGeneratorVelocityDrag(seParticleForceGenerator *generator, const float d)
{
	generator->velDrag = d;
	generator->vecDragSquared = d * d;
	generator->hasDrag = (d != 1.0);
}

void seParticleForceGeneratorUpdateForce(seParticleForceGenerator *generator, seParticle *particle)
{
	if (generator->hasGravity)
	{
		vec3 gravityForce;
		vec3MultScalar(&gravityForce, &generator->gravity, getParticleMass(particle));
		particle->AddForce(particle, &gravityForce);
	}

	if (generator->hasDrag)
	{
		vec3 dragForce;
		vec3Copy(&dragForce, &particle->velocity);
		float dragCoef = vec3Length(&dragForce);
		dragCoef = generator->velDrag * dragCoef + generator->vecDragSquared * dragCoef * dragCoef;
		vec3Normalize(&dragForce);
		vec3MultScalar(&dragForce, &dragForce, -dragCoef);
		particle->AddForce(particle, &dragForce);
	}
}

static seParticleForceData *seParticleForceDataAllocate()
{
	return (seParticleForceData *)calloc(1, sizeof(seParticleForceData));
}

static seParticleForceData *newParticleForceData(seParticle *particle, seParticleForceGenerator *forceGenerator)
{
	seParticleForceData *result = seParticleForceDataAllocate();

	result->particle = particle;
	result->forceGenerator = forceGenerator;

	return result;
}

static void freeParticleForceData(void *registry)
{
	if (!registry)
		return;
	free(registry);
}

static seParticleForceGenerator *defaultForceGenerator;
static bool firstRun = true;

static void seParticleForceDataManagerFirstRun()
{
	defaultForceGenerator = newParticleForceGenerator();
	defaultForceGenerator->SetGravity(defaultForceGenerator, 0, 0, -9.8);
	firstRun = false;
}
