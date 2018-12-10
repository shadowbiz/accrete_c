#include "textureatlas.h"

seTextureAtlas *newTextureAtlas(const size_t width, const size_t height, const size_t depth)
{
	assert((depth == 1) || (depth == 3) || (depth == 4));
	seTextureAtlas *result = seNew(seTextureAtlas, 1);

	if (!result)
	{
		exit(EXIT_FAILURE);
	}

	result->nodes = newVector(sizeof(vec3));
	result->used = 0;
	result->width = width;
	result->height = height;
	result->depth = depth;
	result->id = 0;

	vec3 node = {1, 1, width - 2};
	seVectorPushBack(result->nodes, &node);

	result->data = seNew(ubyte, width * height * depth);
	if (!result->data)
	{
		exit(EXIT_FAILURE);
	}

	return result;
}

void freeTextureAtlas(seTextureAtlas *atlas)
{
}

void seTextureAtlasUpload(seTextureAtlas *atlas)
{
}

void getRegionTextureAtlas(seVec4 region, const seTextureAtlas *atlas, const size_t width, const size_t height)
{
}

void setRegionTextureAtlas(seTextureAtlas *atlas, const size_t x, const size_t y, const size_t width, const size_t height, const ubyte *data, const size_t stride)
{
}

void seTextureAtlasClear(seTextureAtlas *atlas)
{
}
