#ifndef SHADER_H
#define SHADER_H

#include "structs.h"
#include "engine.h"
#include "loader.h"

static ShaderProgram *ShaderAllocate(void);
void freeShader(ShaderProgram *program);

uint getShaderID(const GLenum type, const char *data);
GLuint getShaderProgramID(const char *vsFilename, const char *tcFilename, const char *teFilename, const char *geoFilename, const char *fsFilename);

ShaderProgram *newShaderBasic(const char *vsFilename, const char *tcFilename, const char *teFilename, const char *geoFilename, const char *fsFilename);
void newShaderFont(ShaderProgram *program);

bool ShaderUse(const uint program);
void ShaderReset(void);

static void ShaderGetActiveUniforms(const ShaderProgram *program);

static void ShaderFontInitUniforms(ShaderProgram *program);
static void ShaderBasicInitUniforms(ShaderProgram *program);

void ShaderDuplicate(ShaderProgram *result, const ShaderProgram *shader);

bool getShaderAllocStatus();

#endif
