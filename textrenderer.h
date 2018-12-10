#ifndef SE_TEXTRENDERER_H
#define SE_TEXTRENDERER_H

#include "globals.h"
#include "structs.h"
#include "shader.h"
#include "mesh.h"

Text2D *newText2D(char *text, const int size, const char *texturePath);
void Text2DRender(const Text2D *text, const int x, const int y);
void freeText2D(Text2D *text);

#endif // !SE_TEXTRENDERER_H