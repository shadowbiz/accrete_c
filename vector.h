#ifndef SE_VECTOR_H
#define SE_VECTOR_H

#include "globals.h"

seVector *newVector(const size_t itemSize);

void freeVector(seVector *vector);

const void *seVectorGet(const seVector *vector, const size_t index);
const void *seVectorFront(const seVector *vector);
const void *seVectorBack(const seVector *vector);
bool seVectorContains(const seVector *vector, const void *item, bool (*compare)(const void *a, const void *b));
bool seVectorIsEmpty(const seVector *vector);
size_t seVectorSize(const seVector *vector);
void seVectorReserve(seVector *vector, const size_t size);
size_t seVectorCapacity(const seVector *vector);
void seVectorShrink(seVector *vector);
void seVectorClear(seVector *vector);
void seVectorSet(seVector *vector, const size_t index, const void *item);
void seVectorInsert(seVector *vector, const size_t index, const void *item);
void seVectorEraseRange(seVector *vector, const size_t first, const size_t last);
void seVectorErase(seVector *vector, const size_t index);
void seVectorPushBack(seVector *vector, const void *item);
void seVectorPopBack(seVector *vector);
void seVectorResize(seVector *vector, const size_t size);
void seVectorPushBackData(seVector *self, const void *data, const size_t count);
void seVectorInsertData(seVector *self, const size_t index, const void *data, const size_t count);
void seVectorSort(seVector *self, int (*cmp)(const void *, const void *));

#endif // !_VECTOR_H_
