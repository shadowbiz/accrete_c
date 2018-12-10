#include "textrenderer.h"
#include "globals.h"
#include "structs.h"

#include "shader.h"

#include "array.h"

size_t stringLength(char *text)
{
	size_t result = 0;
	while (*text != '\0')
	{
		text++;
		result++;
	}
	return result;
}

Text2D *newText2D(char *text, const int size, const char *texturePath)
{
	Text2D *result = seNew(Text2D, 1);

	if (!result)
	{
		exit(EXIT_FAILURE);
	}

	result->text = text;

	result->texture = getTextureID(texturePath, TEX_DIFFUSE);
	result->shader = getShaderProgramID("Data/Shaders/font_vertex.glsl", NULL, NULL, NULL, "Data/Shaders/font_fragment.glsl");
	result->uniform = glGetUniformLocation(result->shader, "myTextureSampler");

	/*

	const uint length = strlen(text);
	const uint totalIndices = (length + 1) * 2;
	const uint totalVertices = (length + 1) * 2;

	float *vertices = (float*)malloc(2 * totalVertices * sizeof(float));
	float *uvs = (float*)malloc(2 * totalVertices * sizeof(float));
	uint *indices = (uint*)malloc(totalIndices * sizeof(uint));

	uint indexCounter = 0;
	uint vertexCounter = 0;

	seArray *verticesArray = newArray(totalVertices);
	seArray *uvsArray = newArray(totalVertices);

	const float oneSixteenth = 1.0 / 16.0f;
	char character = text[0];

	seVec2 vertUpLeft		= { 0, size };
	seVec2 vertDownLeft		= { 0, 0 };

	float uvX = (character%16) * oneSixteenth;
	float uvY = (character/16) * oneSixteenth;

	seVec2 uvUpLeft			= { uvX, 1.0 - uvY };
	seVec2 uvDownLeft		= { uvX, 1.0 - (uvY + oneSixteenth) };

	seArrayPush(verticesArray, vertUpLeft);
	seArrayPush(verticesArray, vertDownLeft);

	seArrayPush(uvsArray, uvUpLeft);
	seArrayPush(uvsArray, uvDownLeft);

	indices[indexCounter] = indexCounter++;
	indices[indexCounter] = indexCounter++;

	for (uint i = 0; i < length; i++)
	{
		seVec2 vertUpRight		= { i*size+size, size };
		seVec2 vertDownRight	= { i*size+size, 0 };
		
		character = text[i];

		uvX = (character%16) * oneSixteenth;
		uvY = (character/16) * oneSixteenth;

		seVec2 uvUpRight		= { uvX + oneSixteenth, 1.0 - uvY };
		seVec2 uvDownRight		= { uvX + oneSixteenth, 1.0 - (uvY + oneSixteenth) };
		
		seArrayPush(verticesArray, vertUpRight);
		seArrayPush(verticesArray, vertDownRight);

		seArrayPush(uvsArray, uvUpRight);
		seArrayPush(uvsArray, uvDownRight);

		indices[indexCounter] = indexCounter++;
		indices[indexCounter] = indexCounter++;
	}

	for (uint i = 0; i < verticesArray->length; i++)
	{
		float *vertData = (float*)verticesArray->data[i];
		float *uvData = (float*)uvsArray->data[i];

		vertices[vertexCounter + 0] = vertData[0];
		vertices[vertexCounter + 1] = vertData[1];
		
		uvs[vertexCounter + 0] = uvData[0];
		uvs[vertexCounter + 1] = uvData[1];
	}

	freeArray(verticesArray);
	freeArray(uvsArray);

	*/

	const uint totalIndices = 6;
	const uint totalVertices = 4;

	float vertices[8] = {-1.0, 1.0, -1.0, -1.0, 1.0, 1.0, 1.0, -1.0};
	float uvs[8] = {0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0};

	uint indices[6] = {0, 1, 2, 2, 1, 3};

	const size_t sizeOfVec2 = 2 * sizeof(float);

	result->indexCount = totalIndices;
	result->vertexBuffer = MeshCreateGLBuffer(GL_ARRAY_BUFFER, vertices, sizeOfVec2 * totalVertices);
	result->uvBuffer = MeshCreateGLBuffer(GL_ARRAY_BUFFER, uvs, sizeOfVec2 * totalVertices);
	result->EBO = MeshCreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, indices, sizeof(GLuint) * totalIndices);

	glGenVertexArrays(1, &result->VAO);
	glBindVertexArray(result->VAO);

	glBindBuffer(GL_ARRAY_BUFFER, result->vertexBuffer);
	glVertexAttribPointer(LOCATION_VERTEX, 2, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, result->uvBuffer);
	glVertexAttribPointer(LOCATION_UV, 2, GL_FLOAT, false, 0, 0);

	glEnableVertexAttribArray(LOCATION_VERTEX);
	glEnableVertexAttribArray(LOCATION_UV);

	glBindVertexArray(0);
	return result;
}

void freeText2D(Text2D *text)
{
	if (!text)
		return;
	free(text);
}

void Text2DRender(const Text2D *text, const int x, const int y)
{
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDepthFunc(GL_LESS);

	glUseProgram(text->shader);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, text->texture);
	//glUniform1i(text->uniform, 0);

	glBindVertexArray(text->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, text->EBO);

	glDrawElements(GL_TRIANGLES, text->indexCount, GL_UNSIGNED_INT, 0);

	//glDisable(GL_BLEND);
}
