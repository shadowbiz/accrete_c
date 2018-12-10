#ifndef _GAME_H_
#define _GAME_H_

#include "engine.h"

Stage *mainStage;

void GameInit();

void GameRender();
void GameUpdate();
bool GameRelease();

#endif // !_GAME_H_