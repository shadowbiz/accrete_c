#ifndef SE_TEXTUREFONT_H
#define SE_TEXTUREFONT_H

#include "globals.h"
#include "structs.h"
#include "textureatlas.h"

seTextureFont *newTextureFont(seTextureAtlas *atlas, const char *filename, const char size);
void freeTextureFont(seTextureFont *font);

seTextureGlypth *getGlyphTextureFont(const seTextureFont *font, const wchar_t charcode);
size_t seTextureFontLoadGlyphs(seTextureFont *font, const wchar_t *charcodes);
float getKerningTextureFont(const seTextureFont *font, const wchar_t charcode);

#endif // !SE_TEXTUREFONT_H
