#include "material.h"

static uint allocs = 0;

Image *newImage(const char *filename)
{
	Image *result = seNew(Image, 1);

	if (!result)
	{
		exit(EXIT_FAILURE);
	}

	int tmp;
	result->data = stbi_load(filename, &result->width, &result->height, &tmp, 4);
	result->format = GL_RGBA;

	return result;
}

uint getTextureID(const char *filename, const TextureType type)
{
	uint result;
	Image *image = newImage(filename);

	if (!image)
	{
		exit(EXIT_FAILURE);
	}

	glGenTextures(1, &result);

	switch (type)
	{
	case TEX_DIFFUSE:
		glActiveTexture(GL_TEXTURE0);
		break;

	case TEX_NORMAL:
		glActiveTexture(GL_TEXTURE1);
		break;

	case TEX_HEIGHT:
		glActiveTexture(GL_TEXTURE2);
		break;
	}

	glBindTexture(GL_TEXTURE_2D, result);
	glTexImage2D(GL_TEXTURE_2D, 0, image->format, image->width, image->height, 0, image->format, GL_UNSIGNED_BYTE, image->data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);

	freeImage(image);
	return result;
}

void freeImage(Image *image)
{
	if (!image)
	{
		return;
	}

	if (image->data)
	{
		free(image->data);
		image->data = NULL;
	}

	free(image);
	image = NULL;
}

Material *newMaterial()
{
	Material *result = seNew(Material, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	allocs++;

	printf("Creating material...\n");

	result->diffuseMap = 0;
	result->normalMap = 0;
	result->heightMap = 0;

	result->ambientColor = newVec3At(0.1, 0.1, 0.1);
	result->diffuseColor = newVec3At(0.8, 0.8, 0.8);
	result->specularColor = newVec3At(0.6, 0.6, 0.6);

	result->renderMode = GL_TRIANGLES;

	return result;
}

void freeMaterial(Material *material)
{
	if (!material)
		return;

	freeShader(material->shader);
	freeVec3(material->ambientColor);
	freeVec3(material->diffuseColor);
	freeVec3(material->specularColor);

	free(material);
	material = NULL;

	allocs--;
}

void setMaterialShader(Material *material, ShaderProgram *shader)
{
	material->shader = shader;
}

void setMaterialTexture(Material *material, const char *diffusefilename, const char *bumpfilename)
{
	material->diffuseMap = getTextureID(diffusefilename, TEX_DIFFUSE);
	material->normalMap = getTextureID(diffusefilename, TEX_NORMAL);
}

Material *newMaterialPlanet()
{
	Material *result = newMaterial();
	result->shader = newShaderBasic("Data/Shaders/planet_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/planet_fragment.glsl");
	return result;
}

Material *newMaterialSun()
{
	Material *result = newMaterial();
	result->shader = newShaderBasic("Data/Shaders/star_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/star_fragment.glsl");
	return result;
}

Material *newMaterialToon()
{
	Material *result = newMaterial();
	result->shader = newShaderBasic("Data/Shaders/toon_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/toon_fragment.glsl");
	return result;
}

Material *newMaterialTerrain()
{
	Material *result = newMaterial();
	result->renderMode = GL_PATCHES;
	result->shader = newShaderBasic("Data/Shaders/terrain_vertex.glsl", "Data/Shaders/terrain_tesscontrol.glsl", "Data/Shaders/terrain_tesseval.glsl", NULL, "Data/Shaders/terrain_fragment.glsl");
	result->diffuseMap = getTextureID("Data/Textures/heightmap.png", TEX_DIFFUSE);
	result->heightMap = getTextureID("Data/Textures/heightmap.png", TEX_HEIGHT);
	return result;
}

Material *newMaterialGrid()
{
	Material *result = newMaterial();
	result->renderMode = GL_LINES;
	result->shader = newShaderBasic("Data/Shaders/grid_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/grid_fragment.glsl");
	return result;
}

Material *newMaterialSelection()
{
	Material *result = newMaterial();
	result->renderMode = GL_TRIANGLES;
	result->shader = newShaderBasic("Data/Shaders/grid_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/grid_fragment.glsl");
	return result;
}

Material *newMaterialPostFx()
{
	Material *result = newMaterial();
	result->shader = newShaderBasic("Data/Shaders/post_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/post_fragment.glsl");
	return result;
}

bool MaterialPrepare(Material *material)
{
	bool firstUse = ShaderUse(material->shader->program);

	if (material->diffuseMap)
	{
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(material->shader->vars[SHADER_TEXTURE_DIFFUSE], 0);
		glBindTexture(GL_TEXTURE_2D, material->diffuseMap);
	}

	if (material->normalMap)
	{
		glActiveTexture(GL_TEXTURE1);
		glUniform1i(material->shader->vars[SHADER_TEXTURE_BUMP], 1);
		glBindTexture(GL_TEXTURE_2D, material->normalMap);
	}

	if (material->heightMap)
	{
		glActiveTexture(GL_TEXTURE2);
		glUniform1i(material->shader->vars[SHADER_TEXTURE_HEIGHT], 2);
		glBindTexture(GL_TEXTURE_2D, material->heightMap);
	}

	return firstUse;
}

bool getMaterialAllocateStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Material allocations left %i\n", allocs);
		return false;
	}
	return true;
}