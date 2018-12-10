#include "shader.h"

static uint allocs = 0;

static ShaderProgram *ShaderAllocate()
{
	ShaderProgram *result = seNew(ShaderProgram, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	printf("Creating shader...\n");

	allocs++;
	return result;
}

void freeShader(ShaderProgram *program)
{
	if (!program)
		return;

	glDeleteProgram(program->program);

	free(program);
	program = NULL;
	allocs--;
}

ShaderProgram *newShaderBasic(const char *vsFilename, const char *tcFilename, const char *teFilename, const char *geoFilename, const char *fsFilename)
{
	printf("-->>Basic...\n");
	ShaderProgram *program = ShaderAllocate();
	program->program = getShaderProgramID(vsFilename, tcFilename, teFilename, geoFilename, fsFilename);
	ShaderBasicInitUniforms(program);
	ShaderGetActiveUniforms(program);
	return program;
}

static void ShaderGetActiveUniforms(const ShaderProgram *handle)
{
#if DEBUG_LOG

	char name[100];
	int total, i, length;
	GLuint location;

	ShaderUse(handle->program);

	glGetProgramiv(handle->program, GL_ACTIVE_UNIFORMS, &total);
	glGetProgramiv(handle->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);

	printf("Index\t|\tName\n");
	printf("------------------------------------------------\n");
	for (i = 0; i < total; i++)
	{
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;

		glGetActiveUniform(handle->program, (GLuint)i, sizeof(name) - 1, &name_len, &num, &type, name);
		name[name_len] = 0;
		location = glGetUniformLocation(handle->program, name);

		printf("%-5d\t|\t%s\n", location, name);
	}

	printf("\n");

#endif // DEBUG

	ShaderReset();
}

bool ShaderUse(const uint program)
{
	static uint activeProgram = 0;

	if (activeProgram != program)
	{
		activeProgram = program;
		glUseProgram(activeProgram);
		return true;
	}

	return false;
}

void ShaderReset()
{
	ShaderUse(0);
}

static void ShaderFontInitUniforms(ShaderProgram *program)
{
	program->vars[SHADER_TEXTURE_DIFFUSE] = glGetUniformLocation(program->program, "diffuseMap");
	program->vars[SHADER_FONT_COORDS] = glGetUniformLocation(program->program, "FontCoord");
	program->vars[SHADER_FONT_COLOR] = glGetUniformLocation(program->program, "FontColor");
}

static void ShaderBasicInitUniforms(ShaderProgram *program)
{
	program->vars[SHADER_MODEL_MATRIX] = glGetUniformLocation(program->program, "ModelMatrix");
	program->vars[SHADER_VIEW_MATRIX] = glGetUniformLocation(program->program, "ViewMatrix");
	program->vars[SHADER_MODELVIEW_MATRIX] = glGetUniformLocation(program->program, "ModelViewMatrix");
	program->vars[SHADER_MVP] = glGetUniformLocation(program->program, "ModelViewProjectionMatrix");
	program->vars[SHADER_PROJ_MATRIX] = glGetUniformLocation(program->program, "ProjMatrix");
	program->vars[SHADER_NORMAL_MATRIX] = glGetUniformLocation(program->program, "NormalMatrix");

	program->vars[SHADER_TEXTURE_DIFFUSE] = glGetUniformLocation(program->program, "diffuseMap");
	program->vars[SHADER_TEXTURE_BUMP] = glGetUniformLocation(program->program, "normalMap");
	program->vars[SHADER_TEXTURE_HEIGHT] = glGetUniformLocation(program->program, "heightMap");

	program->vars[SHADER_LIGHT_POSITION] = glGetUniformLocation(program->program, "Light.Position");
	program->vars[SHADER_LIGHT_LA] = glGetUniformLocation(program->program, "Light.La");
	program->vars[SHADER_LIGHT_LD] = glGetUniformLocation(program->program, "Light.Ld");
	program->vars[SHADER_LIGHT_LS] = glGetUniformLocation(program->program, "Light.Ls");

	program->vars[SHADER_DELTA] = glGetUniformLocation(program->program, "Delta");
}

GLuint getShaderProgramID(const char *vsFilename, const char *tcFilename, const char *teFilename, const char *geoFilename, const char *fsFilename)
{
	bool hasGeo = (geoFilename != NULL) ? true : false;
	bool hasTC = (tcFilename != NULL) ? true : false;
	bool hasTE = (teFilename != NULL) ? true : false;

	GLuint vShader, gShader, tcShader, teShader, fShader;

	vShader = getShaderID(GL_VERTEX_SHADER, vsFilename);
	fShader = getShaderID(GL_FRAGMENT_SHADER, fsFilename);

	if (hasGeo)
		gShader = getShaderID(GL_GEOMETRY_SHADER, geoFilename);
	if (hasTC)
		tcShader = getShaderID(GL_TESS_CONTROL_SHADER, tcFilename);
	if (hasTE)
		teShader = getShaderID(GL_TESS_EVALUATION_SHADER, teFilename);

	GLuint program = glCreateProgram();

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	if (hasGeo == true)
	{
		printf("Has geo...\n");
		glAttachShader(program, gShader);
	}
	if (hasTC)
	{
		printf("Has TC...\n");
		glAttachShader(program, tcShader);
	}
	if (hasTE)
	{
		printf("Has TE...\n");
		glAttachShader(program, teShader);
	}

	glLinkProgram(program);

	glDeleteShader(vShader);
	glDeleteShader(fShader);

	if (hasGeo)
		glDeleteShader(gShader);
	if (hasTC)
		glDeleteShader(tcShader);
	if (hasTE)
		glDeleteShader(teShader);

#if DEBUG_LOG

	printf("Shader %u (%s + %s) created.\n\n", program, vsFilename, fsFilename);

#endif

	return program;
}

GLuint getShaderID(const GLenum type, const char *filename)
{
	const GLchar *data = LoadTextFile(filename);
	GLuint shader;
	GLint status;

	shader = glCreateShader(type);

	glShaderSource(shader, 1, &data, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE)
	{
		char buffer[512];
		glGetShaderInfoLog(shader, 512, NULL, buffer);
		printf("%s", buffer);
		glDeleteProgram(shader);
	}

	if (data)
	{
		free((void *)data);
	}

	return shader;
}

void ShaderDuplicate(ShaderProgram *result, const ShaderProgram *shader)
{
	*result = *shader;
}

bool getShaderAllocStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Shader allocations left %i\n", allocs);
		return false;
	}
	return true;
}