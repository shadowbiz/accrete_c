#include "light.h"
#include "math.h"

static uint allocs = 0;

void freeLight(seLight *light)
{
	if (!light)
		return;

	printf("Destroying light %s...\n", light->name);

	freeVec3(light->position);
	freeVec3(light->ambient);
	freeVec3(light->diffuse);
	freeVec3(light->specular);

	free(light);
	light = NULL;
	allocs--;
}

seLight *newLight()
{
	static int num = 0;

	seLight *result = seNew(seLight, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	allocs++;

	printf("Creating light...\n");

	result->position = newVec3();
	result->ambient = newVec3();
	result->diffuse = newVec3();
	result->specular = newVec3();

	sprintf(result->name, "Light %i", num);
	num++;

	return result;
}

void setLightPosition(seLight *light, const vec3 *position)
{
	vec3Copy(light->position, position);
}

void setLightColor(seLight *light, const vec3 *color)
{
	vec3MultScalar(light->ambient, color, 0.14);
	vec3MultScalar(light->diffuse, color, 0.84);
	vec3MultScalar(light->specular, color, 0.64);
}

bool getLightAllocateStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Light allocations left %i\n", allocs);
		return false;
	}
	return true;
}