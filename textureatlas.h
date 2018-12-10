#ifndef SE_TEXTUREATLAS_H
#define SE_TEXTUREATLAS_H

#include "globals.h"
#include "structs.h"
#include "vector.h"

seTextureAtlas *newTextureAtlas(const size_t width, const size_t height, const size_t depth);
void freeTextureAtlas(seTextureAtlas *atlas);

void seTextureAtlasUpload(seTextureAtlas *atlas);

void getRegionTextureAtlas(seVec4 region, const seTextureAtlas *atlas, const size_t width, const size_t height);
void setRegionTextureAtlas(seTextureAtlas *atlas, const size_t x, const size_t y, const size_t width, const size_t height, const ubyte *data, const size_t stride);
void seTextureAtlasClear(seTextureAtlas *atlas);

#endif // !SE_TEXTUREATLAS_H
