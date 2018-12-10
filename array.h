#ifndef SE_ARRAY_H
#define SE_ARRAY_H

#include "globals.h"

seArray *newArray(int size);
void freeArray(seArray *arr);

void seArrayPush(seArray *arr, void *obj);
void *seArrayPop(seArray *arr);

void seArrayRemove(seArray *arr, void *obj);
bool seArrayContains(seArray *arr, void *obj);

void seArrayFreeEach(seArray *arr, void(freeFnc)(void *));

bool getArrayAllocateStatus();

#endif // !SE_ARRAY_H
