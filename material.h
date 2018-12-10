#ifndef SE_MATERIAL_H
#define SE_MATERIAL_H

#include "structs.h"
#include "shader.h"
#include "stb_image.h"
#include "math.h"

Image *newImage(const char *filename);
uint getTextureID(const char *filename, const TextureType type);
void freeImage(Image *image);

Material *newMaterial(void);
void freeMaterial(Material *material);

Material *newMaterialPlanet(void);
Material *newMaterialSun(void);
Material *newMaterialPostFx(void);
Material *newMaterialToon(void);
Material *newMaterialGrid();
Material *newMaterialSelection();
Material *newMaterialTerrain();

void setMaterialTexture(Material *material, const char *diffusefilename, const char *bumpfilename);

bool MaterialPrepare(Material *material);

void MaterialDuplicate(Material *result, const Material *material);
bool getMaterialAllocateStatus();

#endif // !MATERIAL_H