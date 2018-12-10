#include "game.h"

static seEntity *mousePointerEntity;
static seEntity *selectionRectangleEntity;
static seEntity *gridEntity;
static seEntity *cubeEntity;
static seEntity *groundEntity;

void GameInit()
{
	MeshCreateDefault();

	printf("Creating game...\n");

	mainStage = newStage("Main");
	setEngineMainCamera(mainStage->camera);

	mousePointerEntity = newEntity();
	seRenderer *cubeRenderer = newRenderer(mousePointerEntity, defaultSphereMesh, defaultMaterial, mainStage->camera, mainStage->light);
	seEntityAddComponent(mousePointerEntity, cubeRenderer, CMP_RENDERER);
	mousePointerEntity->active = false;

	gridEntity = newEntity();
	seRenderer *gridRenderer = newRenderer(gridEntity, defaultGridMesh, defaultGridMaterial, mainStage->camera, mainStage->light);
	seEntityAddComponent(gridEntity, gridRenderer, CMP_RENDERER);

	selectionRectangleEntity = newEntity();
	seRenderer *rectRenderer = newRenderer(selectionRectangleEntity, defaultRectMesh, defaultSelectionMaterial, mainStage->camera, mainStage->light);
	seEntityAddComponent(selectionRectangleEntity, rectRenderer, CMP_RENDERER);
	selectionRectangleEntity->active = false;

	cubeEntity = newEntity();
	seRenderer *renderer = newRenderer(cubeEntity, defaultCubeMesh, defaultMaterial, mainStage->camera, mainStage->light);
	seEntityAddComponent(cubeEntity, renderer, CMP_RENDERER);
	vec3Set(cubeEntity->position, 0, 0, 7.5);
	cubeEntity->scale = 15;

	groundEntity = newEntity();
	seRenderer *terrainRenderer = newRenderer(groundEntity, defaultTerrainMesh, defaultMaterial, mainStage->camera, mainStage->light);
	seEntityAddComponent(groundEntity, terrainRenderer, CMP_RENDERER);

	StageInit(mainStage);
}

void GameRender()
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	groundEntity->Render(groundEntity);
	cubeEntity->Render(cubeEntity);
	StageRender(mainStage);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gridEntity->Render(gridEntity);
	selectionRectangleEntity->Render(selectionRectangleEntity);
	mousePointerEntity->Render(mousePointerEntity);
	glDisable(GL_BLEND);
}

void GameUpdate()
{
	static vec3 s, e;
	static seRenderer *renderer = NULL;

	static bool pressed = false;

	if (renderer == NULL)
	{
		renderer = selectionRectangleEntity->renderer;
	}

	if (pressed == false)
	{
		if (engineInput->leftMouse == true && engineInput->special2Pressed == false)
		{
			pressed = true;
			selectionRectangleEntity->active = true;
			vec3Set(&s, engineInput->mouseWorldPosition->x, engineInput->mouseWorldPosition->y, 0.5f);
			vec3Set(&e, engineInput->mouseWorldPosition->x, engineInput->mouseWorldPosition->y, 0.5f);
			meshRectangleReshape(renderer->mesh, &s, &e);
		}
	}
	else
	{
		if (engineInput->leftMouse == false)
		{
			selectionRectangleEntity->active = false;
			pressed = false;
		}
		else
		{
			vec3 tmp;
			vec3Set(&tmp, engineInput->mouseWorldPosition->x, engineInput->mouseWorldPosition->y, 0.5f);

			if (vec3Compare(&e, &tmp) == false)
			{
				vec3Copy(&e, &tmp);
				meshRectangleReshape(renderer->mesh, &s, &e);
			}
		}
	}

	setEntityPositionVec(mousePointerEntity, engineInput->mouseWorldPosition);

	cubeEntity->Update(cubeEntity);
	groundEntity->Update(groundEntity);
	gridEntity->Update(gridEntity);
	selectionRectangleEntity->Update(selectionRectangleEntity);
	mousePointerEntity->Update(mousePointerEntity);

	StageUpdate(mainStage);
}

bool GameRelease()
{
	printf("Releasing game...\n");
	MeshClearDefault();

	freeStage(mainStage);
	freeEntity(cubeEntity);
	freeEntity(gridEntity);
	freeEntity(selectionRectangleEntity);
	freeEntity(mousePointerEntity);
	freeEntity(groundEntity);

	return true;
}