#ifndef SE_STAGE_H
#define SE_STAGE_H

#include "structs.h"
#include "camera.h"
#include "light.h"
#include "shader.h"
#include "timer.h"
#include "mesh.h"
#include "list.h"

static Stage *StageAllocate();
void freeStage(Stage *stage);

Stage *newStage(char *name);

void StageInit(Stage *stage);

void addStageObject(Stage *stage, void *gameObject);
void setStageMainCamera(Stage *stage, seCamera *camera);

seCamera *getStageMainCamera(Stage *stage);

static bool StagePrepare(Stage *stage);
void StageUpdate(Stage *stage);
void StageRender(Stage *stage);

bool getStageAllocateStatus();

#endif // STAGE_H