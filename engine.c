#include "engine.h"

seCamera *mainCamera;

void setEngineMainCamera(seCamera *camera)
{
	printf("Setting main camera...\n");
	mainCamera = camera;
}

void EngineRenderPre()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.95, 0.95, 0.91, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	GameRender();
}

void EngineRenderPost()
{
}

void EngineMousePointer()
{
	static vec3 mouseWorldNear, mouseWorldFar, mouseScr;
	vec3Set(&mouseScr, engineInput->mouseX, WINDOW_HEIGHT - engineInput->mouseY, 0.0);
	seUnproject(&mouseWorldNear, &mouseScr, mainCamera->viewMatrix, mainCamera->projectionMatrix);
	mouseScr.z = 1.0f;
	seUnproject(&mouseWorldFar, &mouseScr, mainCamera->viewMatrix, mainCamera->projectionMatrix);
	GetIntersectionXY(engineInput->mouseWorldPosition, &mouseWorldFar, &mouseWorldNear);
}

void EngineCameraUpdate()
{
	static const byte translationAmount = 15;
	static vec3 translation;
	if (engineInput->isActive)
	{
		vec3Set(&translation, -engineInput->mouseDeltaX * DELTA_TIME * translationAmount,
				-engineInput->mouseDeltaY * DELTA_TIME * translationAmount,
				engineInput->mouseDeltaZ * DELTA_TIME * translationAmount);

		CameraTranslate(mainCamera, &translation);
		CameraTargetTranslate(mainCamera, &translation);
	}

	CameraUpdate(mainCamera);
}

bool EngineUpdate()
{
	bool success = true;
	success = InputUpdate(engineInput);

	if (!success)
		return false;

	EngineCameraUpdate();
	EngineMousePointer();

	return true;
}

bool EngineInit()
{
	printf("Engine init...\n");
	engineInput = newInput();
	return true;
}

bool EngineRelease()
{
	printf("Releasing engine...");
	bool success = true;

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(LOCATION_VERTEX);
	glDisableVertexAttribArray(LOCATION_NORMAL);
	glDisableVertexAttribArray(LOCATION_TANGENT);
	glDisableVertexAttribArray(LOCATION_UV);

	printf("destroying stuff...");

	freeInput(engineInput);

	acFreeData(NULL);

	printf(" OK\n");
	return success;
}
