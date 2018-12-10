#ifndef TIMER_H
#define TIMER_H

#include "structs.h"

static uint totalTris;

float getTime();
float getTimerFPS(const float deltaTime);
float getTimerDelta(void);

#endif // !TIMER_H
