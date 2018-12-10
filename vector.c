#include "vector.h"

seVector *newVector(const size_t itemSize)
{
	seVector *result = seNew(seVector, 1);
	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	result->itemSize = itemSize;
	result->size = 0;
	result->capacity = 1;
	result->items = malloc(result->itemSize * result->capacity);

	return result;
}

void freeVector(seVector *vector)
{
	if (!vector)
		return;

	free(vector->items);
	free(vector);
}

const void *seVectorGet(const seVector *vector, const size_t index)
{
	return (char *)(vector->items) + index * vector->itemSize;
}

const void *seVectorFront(const seVector *vector)
{
	return seVectorGet(vector, 0);
}

const void *seVectorBack(const seVector *vector)
{
	return seVectorGet(vector, vector->size - 1);
}

bool seVectorContains(const seVector *vector, const void *item, bool (*compare)(const void *a, const void *b))
{
	for (size_t i = 0; i < vector->size; i++)
	{
		if ((*compare)(item, seVectorGet(vector, i)))
		{
			return true;
		}
	}
	return false;
}

bool seVectorIsEmpty(const seVector *vector)
{
	return vector->size == 0;
}

size_t seVectorSize(const seVector *vector)
{
	return vector->size;
}

void seVectorReserve(seVector *vector, const size_t size)
{
	if (vector->capacity < size)
	{
		vector->items = realloc(vector->items, size * vector->itemSize);
		vector->capacity = size;
	}
}

size_t seVectorCapacity(const seVector *vector)
{
	return vector->capacity;
}

void seVectorShrink(seVector *vector)
{
	if (vector->capacity > vector->size)
	{
		vector->items = realloc(vector->items, vector->size * vector->itemSize);
	}
	vector->capacity = vector->size;
}

void seVectorClear(seVector *vector)
{
	vector->size = 0;
}

void seVectorSet(seVector *vector, const size_t index, const void *item)
{
	memcpy((char *)(vector->items) + index * vector->itemSize, item, vector->itemSize);
}

void seVectorInsert(seVector *vector, const size_t index, const void *item)
{
	if (vector->capacity <= vector->size)
	{
		seVectorReserve(vector, vector->capacity * 2);
	}

	if (index < vector->size)
	{
		memmove((char *)(vector->items) + (index + 1) * vector->itemSize, (char *)(vector->items) + (index + 0) * vector->itemSize, (vector->size - index) * vector->itemSize);
	}
	vector->size++;
	seVectorSet(vector, index, item);
}

void seVectorEraseRange(seVector *vector, const size_t first, const size_t last)
{
	memmove((char *)(vector->items) + first * vector->itemSize, (char *)(vector->items) + last * vector->itemSize, (vector->size - last) * vector->itemSize);
	vector->size -= (last - first);
}

void seVectorErase(seVector *vector, const size_t index)
{
	seVectorEraseRange(vector, index, index + 1);
}

void seVectorPushBack(seVector *vector, const void *item)
{
	seVectorInsert(vector, vector->size, item);
}

void seVectorPopBack(seVector *vector)
{
	vector->size--;
}

void seVectorResize(seVector *vector, const size_t size)
{
	if (size > vector->capacity)
	{
		seVectorReserve(vector, size);
		vector->size = vector->capacity;
	}
	else
	{
		vector->size = size;
	}
}

void seVectorPushBackData(seVector *self, const void *data, const size_t count)
{
	if (self->capacity < (self->size + count))
	{
		seVectorReserve(self, self->size + count);
	}
	memmove((char *)(self->items) + self->size * self->itemSize, data, count * self->itemSize);
	self->size += count;
}

void seVectorInsertData(seVector *self, const size_t index, const void *data, const size_t count)
{
	if (self->capacity < (self->size + count))
	{
		seVectorReserve(self, self->size + count);
	}
	memmove((char *)(self->items) + (index + count) * self->itemSize, (char *)(self->items) + (index)*self->itemSize, count * self->itemSize);
	memmove((char *)(self->items) + index * self->itemSize, data, count * self->itemSize);
	self->size += count;
}

void seVectorSort(seVector *self, int (*cmp)(const void *, const void *))
{
	qsort(self->items, self->size, self->itemSize, cmp);
}