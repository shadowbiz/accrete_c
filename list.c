#include "list.h"

seList *newList()
{
	seList *result = seNew(seList, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	printf("Creating list...\n");

	return result;
}

void freeList(seList *list)
{
	if (!list)
		return;
	seListClear(list);
	free(list);
	list = NULL;
}

void seListClear(seList *list)
{
	seNode *node, *tmpNode;
	node = list->firstNode;
	while (node != NULL)
	{
		tmpNode = node;
		node = node->next;
		free(tmpNode);
		list->size--;
	}
	list->firstNode = NULL;
}

bool seListAdd(seList *list, void *element)
{
	seNode *node = (seNode *)malloc(sizeof(seNode));

	if (!node)
	{
		printf("Not enough memory!\n");
		return false;
	}

	node->element = element;
	node->next = list->firstNode;
	list->firstNode = node;
	list->size++;
	return true;
}

void seListRemove(seList *list, void *element)
{
	seNode *node = list->firstNode, *tmpNode;

	while (node != NULL)
	{
		if (node->next != NULL && node->next->element == element)
		{
			printf("Element found in list. Removing...\n");
			tmpNode = node->next;
			node->next = node->next->next;
			free(tmpNode);
			list->size--;
		}
		node = node->next;
	}
}

bool seListConstains(seList *list, void *element)
{
	seNode *node = list->firstNode;

	while (node != NULL)
	{
		if (node->element == element)
		{
			return true;
		}
		node = node->next;
	}
	return false;
}

uint getListSize(seList *list)
{
	return list->size;
}
