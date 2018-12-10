#ifndef SE_LIST_H
#define SE_LIST_H

#include "globals.h"
#include "structs.h"

seList *newList();
void freeList(seList *list);

bool seListAdd(seList *list, void *element);
void seListClear(seList *list);
void seListRemove(seList *list, void *element);
bool seListConstains(seList *list, void *element);

uint getListSize(seList *list);

bool getListAllocateStatus();

#endif // !SE_LIST_H