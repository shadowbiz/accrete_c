#include "tmp_emitter.h"

static uint allocs = 0;
static seParticleForceGenerator *defaultForceGenerator;
static bool firstRun = true;

static seParticleEmitter *seEmitterAllocate()
{
	allocs++;
	return (seParticleEmitter *)calloc(1, sizeof(seParticleEmitter));
}

seParticleEmitter *newEmitter(const vec3 *position, const seParticleType type)
{
	if (firstRun)
	{
		defaultForceGenerator = newParticleForceGenerator();
		defaultForceGenerator->SetGravity(defaultForceGenerator, 0, 0, -9.8);
		firstRun = false;
	}

	seParticleEmitter *result = seEmitterAllocate();

	result->Render = seEmitterRender;
	result->Update = seEmitterUpdate;
	result->AddParticle = seEmitterAddParticle;
	result->RemoveParticle = seEmitterRemoveParticle;

	result->particles = newArray(0);
	result->generator = defaultForceGenerator;

	result->boundX = 150.0;
	result->boundY = 150.0;
	result->boundZ = 0.0;

	return result;
}

void seEmitterUpdate(seParticleEmitter *emitter)
{
	for (int i = 0; i < emitter->particles->length; i++)
	{
		seParticle *particle = (seParticle *)emitter->particles->data[i];

		bool isPositionOK = particle->position.x > -emitter->boundX && particle->position.x < emitter->boundX && particle->position.y > -emitter->boundY && particle->position.y < emitter->boundY;
		bool isInAir = particle->type != EXPLOSION ? particle->position.z >= 0.0 : particle->position.z >= emitter->boundZ;
		bool isTimeOK = glfwGetTime() < particle->bornTime + 10.0;

		if (!isPositionOK || !isTimeOK)
		{
			emitter->RemoveParticle(emitter, particle);
			continue;
		}
		else if (!isInAir)
		{
			bool initExplosion = (particle->type != EXPLOSION);
			if (initExplosion)
			{
				vec3 position, rnd;
				vec3Copy(&position, &particle->position);
				vec3Set(&rnd, seRandomNumber(-1, 1), seRandomNumber(-1, 1), 0.0);
				vec3Add(&position, &position, &rnd);
				uint expCount = (int)seRandomNumber(60, 90);
				for (int e = 0; e < expCount; e++)
				{
					emitter->AddParticle(emitter, &position, EXPLOSION);
				}
			}

			bool bounce = !initExplosion && seRandomNumber(0.0, 100.0) > 30.0;

			if (bounce)
			{
				vec3MultScalar(&particle->velocity, &particle->velocity, 0.5);
				particle->velocity.z *= -1;
			}
			else
			{
				emitter->RemoveParticle(emitter, particle);
				continue;
			}
		}

		particle->ResetForce(particle);
		emitter->generator->UpdateForce(emitter->generator, particle);
		particle->Update(particle);
	}
}

void seEmitterAddParticle(seParticleEmitter *emitter, const vec3 *position, const seParticleType type)
{
	seArrayPush(emitter->particles, newParticle(position, type));
}

void seEmitterRemoveParticle(seParticleEmitter *emitter, seParticle *particle)
{
	seArrayRemove(emitter->particles, particle);
	freeParticle(particle);
	particle = NULL;
}

void seEmitterRender(seParticleEmitter *emitter, seShape *shape)
{
	for (int i = 0; i < emitter->particles->length; i++)
	{
		seParticle *particle = (seParticle *)emitter->particles->data[i];
		float scale = getParticleMass(particle) / 200.0;
		//setPrimitiveScale(shape, scale);
		//sePrimitiveRenderAtVec(shape, &particle->position);
	}
}

void freeEmitter(seParticleEmitter *emitter)
{
	if (!emitter)
		return;
	seArrayFreeEach(emitter->particles, freeParticle);
	freeArray(emitter->particles);
	free(emitter);
	allocs--;
}

void seEmitterClearDefaultData()
{
	freeParticleForceGenerator(defaultForceGenerator);
}

bool getEmitterAllocStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Emitter allocations left %i\n", allocs);
		return false;
	}
	return true;
}
