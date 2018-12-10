#include "array.h"

seArray *newArray(int size)
{
    seArray *result = seNew(seArray, 1);

    if (!result)
    {
        fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    result->length = 0;
    result->capacity = (size ? size : 4);
    result->data = (void **)calloc(result->capacity, sizeof(void **));

    return result;
}

void freeArray(seArray *arr)
{
    if (!arr)
        return;

    free(arr->data);
    free(arr);
    arr = NULL;
}

void seArrayPush(seArray *arr, void *obj)
{
    if (arr->length == arr->capacity)
    {
        arr->capacity *= 2;
        arr->data = (void **)realloc(arr->data, arr->capacity * sizeof(void **));
    }

    arr->data[arr->length] = obj;
    arr->length++;
}

void *seArrayPop(seArray *arr)
{
    arr->length--;
    void *result = arr->data[arr->length];
    arr->data[arr->length] = NULL;

    return result;
}

void seArrayRemove(seArray *arr, void *obj)
{
    for (int i = 0; i < arr->length; i++)
    {
        if (arr->data[i] == obj)
        {
            arr->length--;
            arr->data[i] = arr->data[arr->length];
            arr->data[arr->length] = NULL;

            return;
        }
    }
}

void seArrayFreeEach(seArray *arr, void(freeFnc)(void *))
{
    for (int i = 0; i < arr->length; i++)
    {
        freeFnc(arr->data[i]);
    }
}

bool seArrayContains(seArray *arr, void *obj)
{
    for (int i = 0; i < arr->length; i++)
    {
        if (arr->data[i] == obj)
            return true;
    }

    return false;
}
