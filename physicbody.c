#include "physicbody.h"

static PhBody *PhBodyAllocate()
{
	PhBody *result = (PhBody *)malloc(sizeof(PhBody));
	PhBodyInit(result);
	return result;
}

static void PhBodyInit(PhBody *body)
{
	if (!body)
	{
		return;
	}

	memset(body, 0, sizeof(PhBody));
}

void PhBodyFree(PhBody *body)
{
	if (!body)
	{
		return;
	}

	GameObjectFree(body->entity);
	free(body);
}

PhBody *PhBodyCreate(GameObject *entity)
{
	PhBody *result = PhBodyAllocate();

	InputNew(&result->input);
	result->Update = PhBodyUpdate;
	result->Render = PhRender;
	result->entity = entity;

	PhStateCreate(&result->current);
	PhStateCreate(&result->previous);

	result->current.size = 1;
	result->current.mass = 1;
	result->current.inverseMass = 1.0f / result->current.mass;

	vec3SetZero(result->current.position);
	vec3SetZero(result->current.momentum);
	vec3SetZero(result->current.angularMomentum);

	quatSetIdentity(result->current.orientation);

	result->current.inertiaTensor = result->current.mass * result->current.size * result->current.size * 1.0f / 6.0f;
	result->current.inverseInertiaTensor = 1.0f / result->current.inertiaTensor;
	result->current.Recalculate(&result->current);
	result->previous = result->current;

	return result;
}

void setPhBodyMass(PhBody *body, seFloat mass)
{
	if (mass < EPSILON)
		mass = 1;
	body->current.mass = mass;
	body->current.inverseMass = 1.0 / mass;
	body->current.Recalculate(&body->current);
}

void setPhBodyMomentum(PhBody *body, seFloat momentum)
{
	if (momentum < 0)
		momentum = 1;
	body->current.inertiaTensor = momentum;
	body->current.inverseInertiaTensor = 1.0 / momentum;
	body->current.Recalculate(&body->current);
}

void PhBodyUpdate(PhBody *body, const seFloat time)
{
	body->previous = body->current;
	PhIntegrate(&body->current, &body->input, time);

	PhState state;
	PhStateNew(&state);
	PhInterpolate(&state, &body->previous, &body->current, 1.0f);

	mat4Identity(body->entity->positionMatrix);
	mat4Translate(body->entity->positionMatrix, state.position);
	quatGetMat4(body->entity->rotationMatrix, state.orientation);

	GameObjectUpdate(body->entity);
}

void PhRender(PhBody *body)
{
	body->entity->Render(body->entity);
}