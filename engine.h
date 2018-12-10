#ifndef SE_ENGINE_H
#define SE_ENGINE_H

#include "globals.h"
#include "structs.h"
#include "input.h"
#include "math.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "timer.h"
#include "mesh.h"
#include "acmain.h"
#include "stage.h"
#include "list.h"
#include "array.h"
#include "acutils.h"
#include "entity.h"
#include "textrenderer.h"
#include "vector.h"
#include "game.h"

bool engineInitialized;
Input *engineInput;

void setEngineMainCamera(seCamera *camera);

bool EngineInit(void);

bool EnginePrepareRender(void);
void EngineRenderPre(void);
void EngineRenderPost(void);

bool EngineUpdate(void);
bool EngineRelease(void);

#endif // !ENGINE_H