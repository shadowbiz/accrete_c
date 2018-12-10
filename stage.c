#include "stage.h"
#include "globals.h"
#include "structs.h"
#include "engine.h"
#include "game.h"

static uint allocs = 0;

void freeStage(Stage *stage)
{
	if (!stage)
		return;

	printf("Releasing stage...");

	freeLight(stage->light);
	freeCamera(stage->camera);

	seArray *arr = stage->stageObjects;
	for (int i = 0; i < arr->length; i++)
	{
		seEntity *entity = (seEntity *)arr->data[i];
		freeEntity(entity);
	}

	printf(" stage array...");
	freeArray(arr);

	printf(" stage script...");
	lua_close(stage->script);

	free(stage);
	stage = NULL;

	allocs--;

	printf(" OK\n");
}

int addSphere(lua_State *lua)
{
	seEntity *entity = newEntity();
	entity->useScript = true;
	seEntityInit(entity);
	seRenderer *renderer = newRenderer(entity, defaultSphereMesh, defaultMaterial, mainStage->camera, mainStage->light);
	seEntityAddComponent(entity, renderer, CMP_RENDERER);
	entity->scale = 10;

	addStageObject(mainStage, entity);

	return 0;
}

Stage *newStage(char *name)
{
	Stage *stage = seNew(Stage, 1);

	if (!stage)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	allocs++;

	vec3 cameraPosition, lightPosition, zero;

	vec3Set(&cameraPosition, 0, -120, 120);
	vec3Set(&lightPosition, 100, -100, 100);
	vec3SetZero(&zero);

	stage->Render = StageRender;
	stage->Update = StageUpdate;

	stage->camera = newCamera(35.0, &cameraPosition, &zero, stage);
	stage->light = newLight();

	setLightPosition(stage->light, &lightPosition);

	stage->stageObjects = newArray(1);

	setName(stage->name, "qwe");

	stage->script = luaL_newstate();
	luaL_openlibs(stage->script);
	lua_register(stage->script, "addSphere", addSphere);

	return stage;
}

void StageInit(Stage *stage)
{
	printf("Reading stage scripts...\n");

	int err = 0.0;
	lua_State *lua = stage->script;

	if (err = luaL_dofile(lua, "Data/Scripts/stage.lua") == 0.0)
	{
		lua_getglobal(lua, "init");
		if (lua_isfunction(lua, -1))
		{
			lua_call(lua, 0, 3);
		}
		/*
		lua_getglobal(lua, "update");
		if (lua_isfunction(lua, -1))
		{
			lua_pushnumber(lua, DELTA_TIME);
			lua_call(lua, 1, 0);
		}
		*/
	}
	else
	{
		printf("Error, unable to run lua script...\n");
	}
}

void addStageObject(Stage *stage, void *entity)
{
	seArrayPush(stage->stageObjects, entity);
}

void setStageMainCamera(Stage *stage, seCamera *camera)
{
	stage->camera = camera;
}

seCamera *getStageMainCamera(Stage *stage)
{
	return stage->camera;
}

int seEntityCompare(const void *a, const void *b)
{
	seEntity *e1 = (seEntity *)a;
	seEntity *e2 = (seEntity *)b;
	seCamera *camera = e1->renderer->camera;
	vec3 *v1 = e1->position;
	vec3 *v2 = e2->position;
	vec3 *v3 = camera->position;

	float d1 = vec3Distance(v1, v3);
	float d2 = vec3Distance(v2, v3);

	if (d1 < d2)
		return -1;
	if (d1 > d2)
		return 1;

	return 0;
}

void StageUpdate(Stage *stage)
{
	seArray *arr = stage->stageObjects;
	//qsort(arr->data, arr->length, sizeof(seEntity*), seEntityCompare);

	for (int i = 0; i < arr->length; i++)
	{
		seEntity *entity = (seEntity *)arr->data[i];
		entity->Update(entity);
	}
}

void StageRender(Stage *stage)
{
	seArray *arr = stage->stageObjects;
	for (int i = 0; i < arr->length; i++)
	{
		seEntity *entity = (seEntity *)arr->data[i];
		entity->Render(entity);
	}
}

bool getStageAllocateStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Stage allocations left %i\n", allocs);
		return false;
	}
	return true;
}