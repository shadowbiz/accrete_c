#include "mesh.h"

static uint dataAllocs = 0;
static uint meshAllocs = 0;

void MeshCreateDefault(void)
{
	static bool firstRun = true;

	if (firstRun == false)
	{
		return;
	}
	else if (firstRun == true)
	{
		firstRun = false;
	}

	printf("Creating default meshes...\n");

	//defaultSphereMesh = newMeshSphere(1,4);//MeshCreateGeoSphere
	//defaultSphereMesh = MeshCreateGeoSphere(1);
	defaultSphereMesh = newMeshObjFile("Data/Objects/monkey.obj");
	//defaultSphereMesh = newMeshPlane128();

	defaultGridMesh = newMeshGrid(10.0, 200);

	vec3 *s = newVec3At(-10, -10, 0);
	vec3 *e = newVec3At(10, 10, 0);

	defaultRectMesh = newMeshRectangle(s, e);

	vec3Set(s, -100, -100, 0);
	vec3Set(e, 100, 100, 0);

	defaultTerrainMesh = newMeshRectangle(s, e);

	//defaultCubeMesh = newArrow();
	defaultCubeMesh = newMeshObjFile("Data/Objects/polycube3.obj");
	//defaultCubeMesh = newMeshObjFile("Data/Objects/monkey.obj");
	//defaultGridMesh = newMeshPlane(150.0, 150.0);

	defaultMaterial = newMaterialToon();
	defaultGridMaterial = newMaterialGrid();
	defaultSelectionMaterial = newMaterialSelection();
	defaultTerrainMaterial = newMaterialTerrain();

	freeVec3(s);
	freeVec3(e);
}

void MeshClearDefault(void)
{
	printf("Releasing default mesh data...");
	if (defaultMaterial)
	{
		freeMaterial(defaultMaterial);
		defaultMaterial = NULL;
	}

	if (defaultGridMaterial)
	{
		freeMaterial(defaultGridMaterial);
		defaultGridMaterial = NULL;
	}

	if (defaultTerrainMaterial)
	{
		freeMaterial(defaultTerrainMaterial);
		defaultTerrainMaterial = NULL;
	}

	if (defaultSelectionMaterial)
	{
		freeMaterial(defaultSelectionMaterial);
		defaultSelectionMaterial = NULL;
	}

	if (defaultSphereMesh)
	{
		freeMesh(defaultSphereMesh);
		defaultSphereMesh = NULL;
	}

	if (defaultGridMesh)
	{
		freeMesh(defaultGridMesh);
		defaultSphereMesh = NULL;
	}

	if (defaultCubeMesh)
	{
		freeMesh(defaultCubeMesh);
		defaultCubeMesh = NULL;
	}

	if (defaultRectMesh)
	{
		freeMesh(defaultRectMesh);
		defaultRectMesh = NULL;
	}

	if (defaultTerrainMesh)
	{
		freeMesh(defaultTerrainMesh);
		defaultTerrainMesh = NULL;
	}

	printf(" OK\n");
}

static MeshData *MeshDataAllocate()
{
	MeshData *result = seNew(MeshData, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	dataAllocs++;
	return result;
}

void freeMeshData(MeshData *meshData)
{
	if (!meshData)
		return;

	if (meshData->indices)
	{
		free(meshData->indices);
		meshData->indices = NULL;
	}
	if (meshData->normals)
	{
		free(meshData->normals);
		meshData->normals = NULL;
	}
	if (meshData->uvs)
	{
		free(meshData->uvs);
		meshData->uvs = NULL;
	}
	if (meshData->vertices)
	{
		free(meshData->vertices);
		meshData->vertices = NULL;
	}
	if (meshData->tangents)
	{
		free(meshData->tangents);
		meshData->tangents = NULL;
	}
	free(meshData);

	dataAllocs--;
}

static Mesh *MeshAllocate()
{
	Mesh *result = seNew(Mesh, 1);

	if (!result)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}

	printf("Creating mesh...\n");

	meshAllocs++;
	return result;
}

void freeMesh(Mesh *mesh)
{
	if (!mesh)
		return;

	glDisableVertexAttribArray(LOCATION_VERTEX);
	glDisableVertexAttribArray(LOCATION_NORMAL);
	glDisableVertexAttribArray(LOCATION_TANGENT);
	glDisableVertexAttribArray(LOCATION_UV);

	glDeleteVertexArrays(1, &mesh->vars[BUFFER_VAO]);

	glDeleteBuffers(1, &mesh->vars[BUFFER_VERTEX]);
	glDeleteBuffers(1, &mesh->vars[BUFFER_NORMAL]);
	glDeleteBuffers(1, &mesh->vars[BUFFER_TANGENT]);
	glDeleteBuffers(1, &mesh->vars[BUFFER_UV]);
	glDeleteBuffers(1, &mesh->vars[BUFFER_EBO]);

	free(mesh);
	mesh = NULL;
	meshAllocs--;
}

static void MeshFromData(Mesh *result, const MeshData *meshData)
{
	const size_t sizeOfVec3 = 3 * sizeof(float);
	const size_t sizeOfVec2 = 2 * sizeof(float);

	result->vars[BUFFER_INDEX_COUNT] = meshData->numberIndices;
	result->vars[BUFFER_VERTEX] = MeshCreateGLBuffer(GL_ARRAY_BUFFER, meshData->vertices, sizeOfVec3 * meshData->numberVertices);
	result->vars[BUFFER_NORMAL] = MeshCreateGLBuffer(GL_ARRAY_BUFFER, meshData->normals, sizeOfVec3 * meshData->numberVertices);
	result->vars[BUFFER_TANGENT] = MeshCreateGLBuffer(GL_ARRAY_BUFFER, meshData->tangents, sizeOfVec3 * meshData->numberVertices);
	result->vars[BUFFER_UV] = MeshCreateGLBuffer(GL_ARRAY_BUFFER, meshData->uvs, sizeOfVec2 * meshData->numberVertices);
	result->vars[BUFFER_EBO] = MeshCreateGLBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData->indices, sizeof(GLuint) * result->vars[BUFFER_INDEX_COUNT]);

	glGenVertexArrays(1, &result->vars[BUFFER_VAO]);
	glBindVertexArray(result->vars[BUFFER_VAO]);

	glBindBuffer(GL_ARRAY_BUFFER, result->vars[BUFFER_VERTEX]);
	glVertexAttribPointer(LOCATION_VERTEX, 3, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, result->vars[BUFFER_NORMAL]);
	glVertexAttribPointer(LOCATION_NORMAL, 3, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, result->vars[BUFFER_TANGENT]);
	glVertexAttribPointer(LOCATION_TANGENT, 3, GL_FLOAT, false, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, result->vars[BUFFER_UV]);
	glVertexAttribPointer(LOCATION_UV, 2, GL_FLOAT, false, 0, 0);

	glEnableVertexAttribArray(LOCATION_VERTEX);
	glEnableVertexAttribArray(LOCATION_NORMAL);
	glEnableVertexAttribArray(LOCATION_TANGENT);
	glEnableVertexAttribArray(LOCATION_UV);

	glBindVertexArray(0);
}

Mesh *newMeshObjFile(const char *filename)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();
	MeshDataLoadObjFile(meshData, filename);
	MeshDataCalculateTangents(meshData);
	MeshFromData(result, meshData);
	freeMeshData(meshData);
	return result;
}

Mesh *newArrow()
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	uint numberVertices = 4;
	uint numberIndices = 6;

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	meshData->vertices = seNew(float, numberVertices * 3);
	meshData->normals = seNew(float, numberVertices * 3);
	meshData->uvs = seNew(float, numberVertices * 2);
	meshData->indices = seNew(uint, numberIndices);

	mat4 *rotMatrix = newMat4Identity();
	vec3 *rotVector = newVec3At(0.0, 0, 0.5);

	const float scale = 3.0f;

	mat4Rotate(rotMatrix->data, 25, rotVector);

	vec3 *v1 = newVec3At(0, 0, 0.6f);
	vec3 *v2 = newVec3At(-1 * scale, -2 * scale, 0.6f);
	vec3 *v3 = newVec3At(0, -1.5 * scale, 0.6f);
	vec3 *v4 = newVec3At(1 * scale, -2 * scale, 0.6f);

	vec3MultiplyMat4(v1, rotMatrix, v1);
	vec3MultiplyMat4(v2, rotMatrix, v2);
	vec3MultiplyMat4(v3, rotMatrix, v3);
	vec3MultiplyMat4(v4, rotMatrix, v4);

	float vertices[] =
		{
			v1->x,
			v1->y,
			v1->z,
			v2->x,
			v2->y,
			v2->z,
			v3->x,
			v3->y,
			v3->z,
			v4->x,
			v4->y,
			v4->z,
		};

	freeMat4(rotMatrix);
	freeVec3(rotVector);
	freeVec3(v1);
	freeVec3(v2);
	freeVec3(v3);
	freeVec3(v4);

	float normals[] =
		{
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f};

	float uvs[] =
		{
			0.5f, 1.0f,
			0.0f, 0.0f,
			0.5f, 0.75f,
			1.0f, 1.0f};

	uint indices[] = {0, 1, 2, 0, 2, 3};

	memcpy(meshData->vertices, vertices, sizeof(vertices));
	memcpy(meshData->normals, normals, sizeof(normals));
	memcpy(meshData->uvs, uvs, sizeof(uvs));
	memcpy(meshData->indices, indices, sizeof(indices));

	MeshFromData(result, meshData);
	freeMeshData(meshData);
	return result;
}

Mesh *newMeshRectangle(const vec3 *recStart, const vec3 *recEnd)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	uint numberVertices = 4;
	uint numberIndices = 6;
	uint vertexCounter = 3 * numberVertices;

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	meshData->vertices = seNew(float, vertexCounter);
	meshData->tangents = seNew(float, vertexCounter);
	meshData->normals = seNew(float, vertexCounter);
	meshData->uvs = seNew(float, numberVertices * 2);
	meshData->indices = seNew(uint, numberIndices);

	float vertices[] =
		{
			recStart->x, recStart->y, recStart->z,
			recStart->x, recEnd->y, recStart->z,
			recEnd->x, recStart->y, recStart->z,
			recEnd->x, recEnd->y, recStart->z};

	float normals[] =
		{
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f};

	float tangents[] =
		{
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f};

	float uvs[] =
		{
			0.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 1.0f,
			1.0f, 0.0f};

	uint indices[] = {0, 1, 2, 2, 1, 3};

	memcpy(meshData->vertices, vertices, sizeof(vertices));
	memcpy(meshData->normals, normals, sizeof(normals));
	memcpy(meshData->tangents, tangents, sizeof(tangents));
	memcpy(meshData->uvs, uvs, sizeof(uvs));
	memcpy(meshData->indices, indices, sizeof(indices));

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

void meshRectangleReshape(Mesh *mesh, const vec3 *recStart, const vec3 *recEnd)
{
	float vertices[] =
		{
			recStart->x, recStart->y, recStart->z,
			recStart->x, recEnd->y, recStart->z,
			recEnd->x, recStart->y, recStart->z,
			recEnd->x, recEnd->y, recStart->z};

	glBindBuffer(GL_ARRAY_BUFFER, mesh->vars[BUFFER_VERTEX]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Mesh *newMeshGrid(const float gridStep, const float gridDimensions)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	uint index = 0, vertexCounter;

	float x = 0, y = 0, z = 0;

	while (x < gridDimensions)
	{
		x += gridStep;
		index++;
	}

	const float gridSize = index * gridStep;

	const uint numberOfLines = index + 1;
	const uint numberVertices = numberOfLines * 2 * 2;
	const uint numberIndices = numberVertices;

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	vertexCounter = 3 * numberVertices;

	meshData->vertices = seNew(float, vertexCounter);
	meshData->tangents = seNew(float, vertexCounter);
	meshData->normals = seNew(float, vertexCounter);
	meshData->indices = seNew(uint, numberIndices);

	vertexCounter = 0;

	for (index = 0; index < numberOfLines; index++)
	{
		x = -gridSize / 2.0 + index * gridStep;
		y = -gridSize / 2.0 - gridStep;

		meshData->vertices[vertexCounter * 3 + 0] = x;
		meshData->vertices[vertexCounter * 3 + 1] = y;
		meshData->vertices[vertexCounter * 3 + 2] = 0.0;

		meshData->normals[vertexCounter * 3 + 0] = 0.0;
		meshData->normals[vertexCounter * 3 + 1] = 0.0;
		meshData->normals[vertexCounter * 3 + 2] = 1.0;

		meshData->tangents[vertexCounter * 3 + 0] = 1.0;
		meshData->tangents[vertexCounter * 3 + 1] = 0.0;
		meshData->tangents[vertexCounter * 3 + 2] = 0.0;

		y = gridSize / 2.0 + gridStep;

		vertexCounter++;

		meshData->vertices[vertexCounter * 3 + 0] = x;
		meshData->vertices[vertexCounter * 3 + 2] = 0.0;
		meshData->vertices[vertexCounter * 3 + 1] = y;

		meshData->normals[vertexCounter * 3 + 0] = 0.0;
		meshData->normals[vertexCounter * 3 + 2] = 1.0;
		meshData->normals[vertexCounter * 3 + 1] = 0.0;

		meshData->tangents[vertexCounter * 3 + 0] = 1.0;
		meshData->tangents[vertexCounter * 3 + 2] = 0.0;
		meshData->tangents[vertexCounter * 3 + 1] = 0.0;

		vertexCounter++;

		x = -gridSize / 2.0 - gridStep;
		y = -gridSize / 2.0 + index * gridStep;

		meshData->vertices[vertexCounter * 3 + 0] = x;
		meshData->vertices[vertexCounter * 3 + 2] = 0.0;
		meshData->vertices[vertexCounter * 3 + 1] = y;

		meshData->normals[vertexCounter * 3 + 0] = 0.0;
		meshData->normals[vertexCounter * 3 + 2] = 1.0;
		meshData->normals[vertexCounter * 3 + 1] = 0.0;

		meshData->tangents[vertexCounter * 3 + 0] = 1.0;
		meshData->tangents[vertexCounter * 3 + 2] = 0.0;
		meshData->tangents[vertexCounter * 3 + 1] = 0.0;

		x = gridSize / 2.0 + gridStep;

		vertexCounter++;

		meshData->vertices[vertexCounter * 3 + 0] = x;
		meshData->vertices[vertexCounter * 3 + 2] = 0.0;
		meshData->vertices[vertexCounter * 3 + 1] = y;

		meshData->normals[vertexCounter * 3 + 0] = 0.0;
		meshData->normals[vertexCounter * 3 + 2] = 1.0;
		meshData->normals[vertexCounter * 3 + 1] = 0.0;

		meshData->tangents[vertexCounter * 3 + 0] = 1.0;
		meshData->tangents[vertexCounter * 3 + 2] = 0.0;
		meshData->tangents[vertexCounter * 3 + 1] = 0.0;

		vertexCounter++;
	}

	for (index = 0; index < numberIndices; index++)
	{
		meshData->indices[index] = index;
	}

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

Mesh *newMeshCircle(const uint numberSectors)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	const float radius = 1.0f;

	const uint numberVertices = numberSectors + 1U;
	const uint numberIndices = numberVertices;
	const float angleStep = (2.0f * PI) / ((float)numberSectors);

	uint indexIndices, indexCounter, i, vertexCounter = 0;

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->normals = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	for (i = 0; i < numberSectors + 1; i++)
	{
		float currentAngle = angleStep * (float)i;

		meshData->vertices[vertexCounter * 3 + 0] = seCos(currentAngle) * radius;
		meshData->vertices[vertexCounter * 3 + 1] = 0.0f;
		meshData->vertices[vertexCounter * 3 + 2] = seSin(currentAngle) * radius;

		meshData->normals[vertexCounter * 3 + 0] = 0.0f;
		meshData->normals[vertexCounter * 3 + 1] = 1.0f;
		meshData->normals[vertexCounter * 3 + 2] = 0.0f;

		meshData->uvs[vertexCounter * 2 + 0] = 0.5f * seCos(currentAngle) * 0.5f;
		meshData->uvs[vertexCounter * 2 + 1] = 0.5f * seSin(currentAngle) * 0.5f;

		vertexCounter++;
	}

	for (i = 0; i < numberIndices; i++)
	{
		meshData->indices[i] = i;
	}

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

Mesh *newMeshDisc(const uint numberSectors)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	const float radius = 1.0f;

	uint indexIndices, indexCounter, i, vertexCounter = 0;

	const uint numberVertices = numberSectors + 2;
	const uint numberIndices = numberSectors * 3;
	const float angleStep = (2.0f * PI) / ((float)numberSectors);

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->normals = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->tangents = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	meshData->vertices[vertexCounter * 3 + 0] = 0.0f;
	meshData->vertices[vertexCounter * 3 + 1] = 0.0f;
	meshData->vertices[vertexCounter * 3 + 2] = 0.0f;

	meshData->normals[vertexCounter * 3 + 0] = 0.0f;
	meshData->normals[vertexCounter * 3 + 1] = 0.0f;
	meshData->normals[vertexCounter * 3 + 2] = 1.0f;

	meshData->tangents[vertexCounter * 3 + 0] = 1.0f;
	meshData->tangents[vertexCounter * 3 + 1] = 0.0f;
	meshData->tangents[vertexCounter * 3 + 2] = 0.0f;

	meshData->uvs[vertexCounter * 2 + 0] = 0.5f;
	meshData->uvs[vertexCounter * 2 + 1] = 0.5f;

	vertexCounter++;

	for (i = 0; i < numberSectors + 1; i++)
	{
		float currentAngle = angleStep * (float)i;

		meshData->vertices[vertexCounter * 3 + 0] = seCos(currentAngle) * radius;
		meshData->vertices[vertexCounter * 3 + 2] = seSin(currentAngle) * radius;
		meshData->vertices[vertexCounter * 3 + 1] = 0.0f;

		meshData->normals[vertexCounter * 3 + 0] = 0.0f;
		meshData->normals[vertexCounter * 3 + 2] = 0.0f;
		meshData->normals[vertexCounter * 3 + 1] = 1.0f;

		meshData->tangents[vertexCounter * 3 + 0] = 1.0f;
		meshData->tangents[vertexCounter * 3 + 1] = 0.0f;
		meshData->tangents[vertexCounter * 3 + 2] = 0.0f;

		meshData->uvs[vertexCounter * 2 + 0] = 0.5f * seCos(currentAngle) * 0.5f;
		meshData->uvs[vertexCounter * 2 + 1] = 0.5f * seSin(currentAngle) * 0.5f;

		vertexCounter++;
	}

	indexIndices = 0;

	indexCounter = 1;
	for (i = 0; i < numberSectors; i++)
	{
		meshData->indices[indexIndices++] = 0;
		meshData->indices[indexIndices++] = indexCounter;
		meshData->indices[indexIndices++] = indexCounter + 1;

		indexCounter++;
	}

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

Mesh *newMeshCube()
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	const float halfExtend = 1.0f;

	uint i;

	uint numberVertices = 24;
	uint numberIndices = 36;

	float cubeVertices[] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f,
							-1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f,
							-1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, -1.0f,
							-1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f,
							-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f,
							+1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f};

	float cubeNormals[] = {0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
						   0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f,
						   0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,
						   0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,
						   -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
						   +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f};

	float cubeTangents[] = {+1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,
							+1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,
							-1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
							+1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,
							0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,
							0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f};

	float cubeTexCoords[] = {0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
							 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
							 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
							 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
							 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
							 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

	uint cubeIndices[] = {0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21};

	meshData->numberVertices = numberVertices;
	meshData->numberIndices = numberIndices;

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->normals = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->tangents = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	memcpy(meshData->vertices, cubeVertices, sizeof(cubeVertices));

	for (i = 0; i < numberVertices; i++)
	{
		meshData->vertices[i * 3 + 0] *= halfExtend;
		meshData->vertices[i * 3 + 1] *= halfExtend;
		meshData->vertices[i * 3 + 2] *= halfExtend;
	}

	memcpy(meshData->normals, cubeNormals, sizeof(cubeNormals));
	memcpy(meshData->tangents, cubeTangents, sizeof(cubeTangents));
	memcpy(meshData->uvs, cubeTexCoords, sizeof(cubeTexCoords));
	memcpy(meshData->indices, cubeIndices, sizeof(cubeIndices));

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

void MeshCreateDome(Mesh *handle, const uint numberSlices)
{
	MeshData shape;

	uint i, j;

	const float radius = 1.0f;

	uint numberParallels = numberSlices / 4;
	uint numberVertices = (numberParallels + 1) * (numberSlices + 1);
	uint numberIndices = numberParallels * numberSlices * 6;

	float angleStep = (2.0f * PI) / ((float)numberSlices);

	uint indexIndices;

	vec3 helpVector = {1.0f, 0.0f, 0.0f};
	seQuat helpQuaternion;
	mat4 helpMatrix;

	shape.numberVertices = numberVertices;
	shape.numberIndices = numberIndices;

	shape.vertices = (float *)malloc(4 * numberVertices * sizeof(float));
	shape.normals = (float *)malloc(3 * numberVertices * sizeof(float));
	shape.tangents = (float *)malloc(3 * numberVertices * sizeof(float));
	shape.uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	shape.indices = (uint *)malloc(numberIndices * sizeof(uint));

	for (i = 0; i < numberParallels + 1; i++)
	{
		for (j = 0; j < numberSlices + 1; j++)
		{
			uint vertexIndex = (i * (numberSlices + 1) + j) * 3;
			uint normalIndex = (i * (numberSlices + 1) + j) * 3;
			uint tangentIndex = (i * (numberSlices + 1) + j) * 3;
			uint texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

			shape.vertices[vertexIndex + 0] = radius * seSin(angleStep * (float)i) * seSin(angleStep * (float)j);
			shape.vertices[vertexIndex + 1] = radius * seCos(angleStep * (float)i);
			shape.vertices[vertexIndex + 2] = radius * seSin(angleStep * (float)i) * seCos(angleStep * (float)j);

			shape.normals[normalIndex + 0] = shape.vertices[vertexIndex + 0] / radius;
			shape.normals[normalIndex + 1] = shape.vertices[vertexIndex + 1] / radius;
			shape.normals[normalIndex + 2] = shape.vertices[vertexIndex + 2] / radius;

			shape.uvs[texCoordsIndex + 0] = (float)j / (float)numberSlices;
			shape.uvs[texCoordsIndex + 1] = 1.0f - (float)i / (float)numberParallels;

			// use quaternion to get the tangent vector
			quatRotateRy(helpQuaternion, 360.0f * shape.uvs[texCoordsIndex + 0]);
			quatGetMat4(&helpMatrix, helpQuaternion);

			vec3 tmp;

			vec3MultiplyMat4(&tmp, &helpMatrix, &helpVector);

			shape.tangents[tangentIndex + 0] = tmp.x;
			shape.tangents[tangentIndex + 1] = tmp.y;
			shape.tangents[tangentIndex + 2] = tmp.z;
		}
	}

	indexIndices = 0;
	for (i = 0; i < numberParallels; i++)
	{
		for (j = 0; j < numberSlices; j++)
		{
			shape.indices[indexIndices++] = i * (numberSlices + 1) + j;
			shape.indices[indexIndices++] = (i + 1) * (numberSlices + 1) + j;
			shape.indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);

			shape.indices[indexIndices++] = i * (numberSlices + 1) + j;
			shape.indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);
			shape.indices[indexIndices++] = i * (numberSlices + 1) + (j + 1);
		}
	}

	MeshFromData(handle, &shape);
	freeMeshData(&shape);
}

Mesh *newMeshGeoSphere()
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	int i, vertexIndex = 0;

	const float radius = 1.0f;

	//Initialize a platonic solid
	//Here we go with an Icosahedron
	//Icosahedron stats:
	//Vertices: 12
	//Edges: 30
	//Faces: 20
	//Edges per face: 3
	//Edges shared per vertex: 5

	//Edge length = golden ratio
	const float sqrt5 = sqrt(5.0f);
	const float phi = (1.0f + sqrt5) * 0.5f;
	// Circumscribed radius
	const float cRadius = sqrt(10.0f + (2.0f * sqrt5)) / (4.0f * phi);
	//Now we define constants which will define our Icosahedron's vertices
	double a = (radius / cRadius) * 0.5;
	double b = (radius / cRadius) / (2.0f * phi);
	uint indices[60] = {0, 1, 2,
						3, 2, 1,
						3, 4, 5,
						3, 8, 4,
						0, 6, 7,
						0, 9, 6,
						4, 10, 11,
						6, 11, 10,
						2, 5, 9,
						11, 9, 5,
						1, 7, 8,
						10, 8, 7,
						3, 5, 2,
						3, 1, 8,
						0, 2, 9,
						0, 7, 1,
						6, 9, 11,
						6, 10, 7,
						4, 11, 5,
						4, 8, 10};

	vec3 verts[12] = {{0, b, -a}, {b, a, 0}, {-b, a, 0}, {0, b, a}, {0, -b, a}, {-a, 0, b}, {0, -b, -a}, {a, 0, -b}, {a, 0, b}, {-a, 0, -b}, {b, -a, 0}, {-b, -a, 0}};

	const uint numberVertices = 12;
	const uint numberIndices = 60;

	meshData->numberVertices = numberVertices;
	meshData->numberIndices = numberIndices;

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->normals = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	vec3 *n = newVec3();

	for (i = 0; i < numberVertices; i++)
	{
		vec3Set(n, verts[i].x / radius, verts[i].y / radius, verts[i].z / radius);
		vec3Normalize(n);

		meshData->vertices[i * 3 + 0] = verts[i].x;
		meshData->vertices[i * 3 + 1] = verts[i].y;
		meshData->vertices[i * 3 + 2] = verts[i].z;

		meshData->normals[i * 3 + 0] = n->x;
		meshData->normals[i * 3 + 1] = n->y;
		meshData->normals[i * 3 + 2] = n->z;
	}

	for (i = 0; i < meshData->numberIndices; i++)
	{
		meshData->indices[i] = indices[i];
	}

	MeshFromData(result, meshData);
	freeMeshData(meshData);
	freeVec3(n);

	return result;
}

Mesh *newMeshSphere(const uint numberSlices)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();
	uint i, j, indexIndices;

	const float radius = 1.0f;

	const uint numberParallels = numberSlices / 2;
	const uint numberVertices = (numberParallels + 1) * (numberSlices + 1);
	const uint numberIndices = numberParallels * numberSlices * 6;

	const float angleStep = (2.0f * PI) / ((float)numberSlices);

	// used later to help us calculating tangents vectors
	vec3 *helpVector = newVec3At(1.0f, 0.0f, 0.0f);
	seQuat helpQuaternion;
	mat4 *helpMatrix = newMat4();

	meshData->numberVertices = numberVertices;
	meshData->numberIndices = numberIndices;

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->normals = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->tangents = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	for (i = 0; i < numberParallels + 1; i++)
	{
		for (j = 0; j < numberSlices + 1; j++)
		{
			uint vertexIndex = (i * (numberSlices + 1) + j) * 3;
			uint normalIndex = (i * (numberSlices + 1) + j) * 3;
			uint tangentIndex = (i * (numberSlices + 1) + j) * 3;
			uint texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

			meshData->vertices[vertexIndex + 0] = radius * seSin(angleStep * (float)i) * seSin(angleStep * (float)j);
			meshData->vertices[vertexIndex + 1] = radius * seCos(angleStep * (float)i);
			meshData->vertices[vertexIndex + 2] = radius * seSin(angleStep * (float)i) * seCos(angleStep * (float)j);

			meshData->normals[normalIndex + 0] = meshData->vertices[vertexIndex + 0] / radius;
			meshData->normals[normalIndex + 1] = meshData->vertices[vertexIndex + 1] / radius;
			meshData->normals[normalIndex + 2] = meshData->vertices[vertexIndex + 2] / radius;

			meshData->uvs[texCoordsIndex + 0] = (float)j / (float)numberSlices;
			meshData->uvs[texCoordsIndex + 1] = 1.0f - (float)i / (float)numberParallels;

			// use quaternion to get the tangent vector
			quatRotateRy(helpQuaternion, 360.0f * meshData->uvs[texCoordsIndex + 0]);
			quatGetMat4(helpMatrix, helpQuaternion);

			vec3 tmp;

			vec3MultiplyMat4(&tmp, helpMatrix, helpVector);

			meshData->tangents[tangentIndex + 0] = tmp.x;
			meshData->tangents[tangentIndex + 1] = tmp.x;
			meshData->tangents[tangentIndex + 2] = tmp.x;
		}
	}

	indexIndices = 0;
	for (i = 0; i < numberParallels; i++)
	{
		for (j = 0; j < numberSlices; j++)
		{
			meshData->indices[indexIndices++] = i * (numberSlices + 1) + j;
			meshData->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + j;
			meshData->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);

			meshData->indices[indexIndices++] = i * (numberSlices + 1) + j;
			meshData->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);
			meshData->indices[indexIndices++] = i * (numberSlices + 1) + (j + 1);
		}
	}

	MeshFromData(result, meshData);
	freeMeshData(meshData);
	freeVec3(helpVector);
	freeMat4(helpMatrix);

	return result;
}

Mesh *newMeshPlane(const float x, const float y)
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	const float scaleX = x / 2.0;
	const float scaleY = y / 2.0;

	uint i;

	uint numberVertices = 4;
	uint numberIndices = 6;

	float xy_vertices[] = {-scaleX, -scaleY, 0.0, scaleX, -scaleY, 0.0, -scaleX, scaleY, 0, scaleX, scaleY, 0};
	float xy_normals[] = {0.0, 0.0f, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0};
	float xy_tangents[] = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
	float xy_texCoords[] = {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0};
	uint xy_indices[] = {0, 1, 2, 1, 3, 2};

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->normals = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->tangents = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	memcpy(meshData->vertices, xy_vertices, sizeof(xy_vertices));
	memcpy(meshData->uvs, xy_texCoords, sizeof(xy_texCoords));
	memcpy(meshData->normals, xy_normals, sizeof(xy_normals));
	memcpy(meshData->tangents, xy_tangents, sizeof(xy_tangents));
	memcpy(meshData->indices, xy_indices, sizeof(xy_indices));

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

Mesh *newMeshPlane2D()
{
	Mesh *result = MeshAllocate();
	MeshData *meshData = MeshDataAllocate();

	const float scaleX = 1;
	const float scaleY = 1;

	uint i;

	uint numberVertices = 4;
	uint numberIndices = 6;

	float xy_vertices[] = {-1.0f, -1.0f, 0, +1.0f, -1.0f, 0, -1.0f, +1.0f, 0, +1.0f, +1.0f, 0};
	float xy_texCoords[] = {0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f};

	uint xy_indices[] = {0, 1, 2, 1, 3, 2};

	meshData->numberIndices = numberIndices;
	meshData->numberVertices = numberVertices;

	for (i = 0; i < numberIndices; i += 3)
	{
		xy_vertices[i + 0] *= scaleX;
		xy_vertices[i + 1] *= scaleY;
	}

	meshData->vertices = (float *)malloc(3 * numberVertices * sizeof(float));
	meshData->uvs = (float *)malloc(2 * numberVertices * sizeof(float));
	meshData->indices = (uint *)malloc(numberIndices * sizeof(uint));

	memcpy(meshData->vertices, xy_vertices, sizeof(xy_vertices));
	memcpy(meshData->uvs, xy_texCoords, sizeof(xy_texCoords));
	memcpy(meshData->indices, xy_indices, sizeof(xy_indices));

	MeshFromData(result, meshData);
	freeMeshData(meshData);

	return result;
}

void MeshCopy(Mesh *result, const Mesh *original)
{
	*result = *original;
}

uint MeshCreateGLBuffer(GLenum target, const void *bufferdata, GLsizei buffersize)
{
	uint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(target, buffer);
	glBufferData(target, buffersize, bufferdata, GL_STREAM_DRAW);
	return buffer;
}

static void MeshDataCalculateTangents(MeshData *mesh)
{
	int i;
	int vertsCount = mesh->numberVertices;

	vec3 *vecVert = vec3Allocate(vertsCount);
	vec3 *vecNormal = vec3Allocate(vertsCount);
	vec3 *vecTangent = vec3Allocate(vertsCount);
	vec3 *vecBitangent = vec3Allocate(vertsCount);

	seVec2 *vecUV = (seVec2 *)malloc(vertsCount * sizeof(seVec2));

	mesh->tangents = (float *)malloc(3 * vertsCount * sizeof(float));

	for (i = 0; i < vertsCount; i++)
	{
		vec3Set(&vecVert[i], mesh->vertices[i * 3 + 0], mesh->vertices[i * 3 + 1], mesh->vertices[i * 3 + 2]);
		vec3Set(&vecNormal[i], mesh->normals[i * 3 + 0], mesh->normals[i * 3 + 1], mesh->normals[i * 3 + 2]);
		vecUV[i][0] = mesh->uvs[i * 2 + 0];
		vecUV[i][1] = mesh->uvs[i * 2 + 1];
	}

	float r = 0.0f;

	for (i = 0; i < vertsCount; i += 3)
	{
		vec3 deltaPos1, deltaPos2;
		vec3 v0, v1, v2;
		vec3 tangent, taTmp1, taTmp2, taTmp3;
		seVec2 deltaUV1, deltaUV2, u0, u1, u2;

		vec3Copy(&v0, &vecVert[i + 0]);
		vec3Copy(&v1, &vecVert[i + 1]);
		vec3Copy(&v2, &vecVert[i + 2]);

		seVec2Copy(u0, vecUV[i + 0]);
		seVec2Copy(u1, vecUV[i + 1]);
		seVec2Copy(u2, vecUV[i + 2]);

		vec3Subtract(&deltaPos1, &v1, &v0);
		vec3Subtract(&deltaPos2, &v2, &v0);

		seVec2Subtract(deltaUV1, u1, u0);
		seVec2Subtract(deltaUV2, u2, u0);

		r = 1.0f / (deltaUV1[0] * deltaUV2[1] - deltaUV1[1] * deltaUV2[0]);

		vec3MultScalar(&taTmp1, &deltaPos1, deltaUV2[1]);
		vec3MultScalar(&taTmp2, &deltaPos2, deltaUV1[1]);
		vec3Subtract(&taTmp3, &taTmp1, &taTmp2);
		vec3MultScalar(&tangent, &taTmp3, r);

		vec3Copy(&vecTangent[i + 0], &tangent);
		vec3Copy(&vecTangent[i + 1], &tangent);
		vec3Copy(&vecTangent[i + 2], &tangent);
	}

	for (i = 0; i < vertsCount; i++)
	{
		vec3 tangent, bitangent, normal;

		vec3Copy(&tangent, &vecTangent[i]);
		vec3Copy(&normal, &vecNormal[i]);

		vec3Normalize(&tangent);
		vec3Cross(&bitangent, &normal, &tangent);
		vec3Copy(&vecBitangent[i], &bitangent);
	}

	for (i = 0; i < vertsCount; i++)
	{
		vec3 normal, tangent, bitangent;
		vec3 tmp1, tmp2, tmp3;
		float tmp;

		vec3Copy(&tangent, &vecTangent[i]);
		vec3Copy(&bitangent, &vecBitangent[i]);
		vec3Copy(&normal, &vecNormal[i]);

		tmp = vec3Dot(&normal, &tangent);
		vec3MultScalar(&tmp1, &normal, tmp);
		vec3Subtract(&tmp2, &tangent, &tmp1);
		vec3Normalize(&tmp2);

		vec3Cross(&tmp1, &normal, &tmp2);
		tmp = vec3Dot(&tmp1, &bitangent);

		vec3MultScalar(&tmp3, &tmp2, tmp < 0 ? -1.0f : 1.0f);
		vec3Copy(&vecTangent[i], &tmp3);

		mesh->tangents[i * 3 + 0] = vecTangent[i].x;
		mesh->tangents[i * 3 + 1] = vecTangent[i].y;
		mesh->tangents[i * 3 + 2] = vecTangent[i].z;
	}

	freeVec3(vecVert);
	freeVec3(vecNormal);
	freeVec3(vecTangent);
	freeVec3(vecBitangent);
	free(vecUV);
}

bool MeshDataLoadObjFile(MeshData *model, const char *filename)
{
	FILE *file;

	int result;

	char buffer[BUFFERSIZE];
	char identifier[3];

	float x, y, z;
	float s, t;

	float *vertices = 0;
	float *normals = 0;
	float *tangents = 0;
	float *bitangents = 0;
	float *uvs = 0;

	GLuint numberVertices = 0;
	GLuint numberNormals = 0;
	GLuint numberUvs = 0;

	float *triangleVertices = 0;
	float *triangleNormals = 0;
	float *triangleUvs = 0;

	GLuint totalNumberVertices = 0;
	GLuint totalNumberNormals = 0;
	GLuint totalNumberUvs = 0;

	GLuint facesEncoding = 0;

	if (!filename || !model)
	{
		return false;
	}

	file = fopen(filename, "r");
	if (!file)
	{
		printf("Error!!! No file %s found \n", filename);
		return true;
	}

	if (!MallocTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs))
	{
		FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
		fclose(file);
		printf("Error!!! Memory allocation error\n");
		return false;
	}

	while (!feof(file))
	{
		if (fgets(buffer, BUFFERSIZE, file) == 0)
		{
			if (ferror(file))
			{
				FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
				fclose(file);
				printf("Error!!! File %s reading error\n", filename);
				return false;
			}
		}

		if (strncmp(buffer, "vt", 2) == 0)
		{
			if (numberUvs == MAX_ATTRIBUTES)
			{
				FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
				fclose(file);
				return false;
			}

			sscanf(buffer, "%s %f %f", identifier, &s, &t);
			uvs[2 * numberUvs + 0] = s;
			uvs[2 * numberUvs + 1] = t;

			numberUvs++;
		}
		else if (strncmp(buffer, "vn", 2) == 0)
		{
			if (numberNormals == MAX_ATTRIBUTES)
			{
				FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
				fclose(file);
				return false;
			}

			sscanf(buffer, "%s %f %f %f", identifier, &x, &y, &z);

			normals[3 * numberNormals + 0] = x;
			normals[3 * numberNormals + 1] = y;
			normals[3 * numberNormals + 2] = z;

			numberNormals++;
		}
		else if (strncmp(buffer, "v", 1) == 0)
		{
			if (numberVertices == MAX_ATTRIBUTES)
			{
				FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
				fclose(file);
				return false;
			}
			sscanf(buffer, "%s %f %f %f", identifier, &x, &y, &z);

			vertices[3 * numberVertices + 0] = x;
			vertices[3 * numberVertices + 1] = y;
			vertices[3 * numberVertices + 2] = z;

			numberVertices++;
		}
		else if (strncmp(buffer, "f", 1) == 0)
		{
			char *token;
			int vIndex, vtIndex, vnIndex;
			GLuint edgeCount = 0;

			token = strtok(buffer, " \t");
			token = strtok(0, " \n");

			if (!token)
			{
				continue;
			}

			if (strstr(token, "//") != 0)
			{
				facesEncoding = 2;
			}
			else if (strstr(token, "/") == 0)
			{
				facesEncoding = 0;
			}
			else if (strstr(token, "/") != 0)
			{
				char *c = strstr(token, "/");
				c++;

				if (!c)
				{
					continue;
				}
				if (strstr(c, "/") == 0)
				{
					facesEncoding = 1;
				}
				else
				{
					facesEncoding = 3;
				}
			}
			while (token != 0)
			{
				vIndex = -1;
				vtIndex = -1;
				vnIndex = -1;

				switch (facesEncoding)
				{
				case 0:
					sscanf(token, "%d", &vIndex);
					break;
				case 1:
					sscanf(token, "%d/%d", &vIndex, &vtIndex);
					break;
				case 2:
					sscanf(token, "%d//%d", &vIndex, &vnIndex);
					break;
				case 3:
					sscanf(token, "%d/%d/%d", &vIndex, &vtIndex, &vnIndex);
					break;
				}

				vIndex--;
				vtIndex--;
				vnIndex--;

				if (vIndex >= 0)
				{
					if (edgeCount < 3)
					{
						if (totalNumberVertices >= MAX_TRIANGLE_ATTRIBUTES)
						{
							FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
							fclose(file);
							return false;
						}

						memcpy(&triangleVertices[3 * totalNumberVertices], &vertices[3 * vIndex], 3 * sizeof(float));

						totalNumberVertices++;
					}
					else
					{
						if (totalNumberVertices >= MAX_TRIANGLE_ATTRIBUTES - 2)
						{
							FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
							fclose(file);
							return false;
						}

						memcpy(&triangleVertices[3 * (totalNumberVertices)], &triangleVertices[3 * (totalNumberVertices - edgeCount)], 3 * sizeof(float));
						memcpy(&triangleVertices[3 * (totalNumberVertices + 1)], &triangleVertices[3 * (totalNumberVertices - 1)], 3 * sizeof(float));
						memcpy(&triangleVertices[3 * (totalNumberVertices + 2)], &vertices[3 * vIndex], 3 * sizeof(float));

						totalNumberVertices += 3;
					}
				}

				if (vnIndex >= 0)
				{
					if (edgeCount < 3)
					{
						if (totalNumberNormals >= MAX_TRIANGLE_ATTRIBUTES)
						{
							FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
							fclose(file);
							return false;
						}

						memcpy(&triangleNormals[3 * totalNumberNormals], &normals[3 * vnIndex], 3 * sizeof(float));

						totalNumberNormals++;
					}
					else
					{
						if (totalNumberNormals >= MAX_TRIANGLE_ATTRIBUTES - 2)
						{
							FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
							fclose(file);
							return false;
						}

						memcpy(&triangleNormals[3 * (totalNumberNormals)], &triangleNormals[3 * (totalNumberNormals - edgeCount)], 3 * sizeof(float));
						memcpy(&triangleNormals[3 * (totalNumberNormals + 1)], &triangleNormals[3 * (totalNumberNormals - 1)], 3 * sizeof(float));
						memcpy(&triangleNormals[3 * (totalNumberNormals + 2)], &normals[3 * vnIndex], 3 * sizeof(float));

						totalNumberNormals += 3;
					}
				}
				if (vtIndex >= 0)
				{
					if (edgeCount < 3)
					{
						if (totalNumberUvs >= MAX_TRIANGLE_ATTRIBUTES)
						{
							FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
							fclose(file);
							return false;
						}

						memcpy(&triangleUvs[2 * totalNumberUvs], &uvs[2 * vtIndex], 2 * sizeof(float));

						totalNumberUvs++;
					}
					else
					{
						if (totalNumberUvs >= MAX_TRIANGLE_ATTRIBUTES - 2)
						{
							FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
							fclose(file);
							return false;
						}

						memcpy(&triangleUvs[2 * (totalNumberUvs)], &triangleUvs[2 * (totalNumberUvs - edgeCount)], 2 * sizeof(float));
						memcpy(&triangleUvs[2 * (totalNumberUvs + 1)], &triangleUvs[2 * (totalNumberUvs - 1)], 2 * sizeof(float));
						memcpy(&triangleUvs[2 * (totalNumberUvs + 2)], &uvs[2 * vtIndex], 2 * sizeof(float));

						totalNumberUvs += 3;
					}
				}
				edgeCount++;
				token = strtok(0, " \n");
			}
		}
	}

	fclose(file);

	result = MeshCopyData(model, totalNumberVertices, triangleVertices, totalNumberNormals, triangleNormals, totalNumberUvs, triangleUvs);

	FreeTempMemory(&vertices, &normals, &uvs, &triangleVertices, &triangleNormals, &triangleUvs);
	return result;
}

static bool MeshCopyData(MeshData *model, GLuint totalNumberVertices, float *triangleVertices, GLuint totalNumberNormals, float *triangleNormals, GLuint totalNumberTexCoords, float *triangleTexCoords)
{
	GLuint indicesCounter = 0;

	if (!model || !triangleVertices || !triangleNormals || !triangleTexCoords)
	{
		return false;
	}

	model->numberVertices = totalNumberVertices;
	if (totalNumberVertices > 0)
	{
		model->vertices = (float *)malloc(totalNumberVertices * 3 * sizeof(float));

		if (model->vertices == 0)
		{
			return false;
		}

		memcpy(model->vertices, triangleVertices, totalNumberVertices * 3 * sizeof(float));
	}

	if (totalNumberNormals > 0)
	{
		model->normals = (float *)malloc(totalNumberNormals * 3 * sizeof(float));

		if (model->normals == 0)
		{
			return false;
		}

		memcpy(model->normals, triangleNormals, totalNumberNormals * 3 * sizeof(float));
	}
	if (totalNumberTexCoords > 0)
	{
		model->uvs = (float *)malloc(totalNumberTexCoords * 2 * sizeof(float));

		if (model->uvs == 0)
		{
			return false;
		}

		memcpy(model->uvs, triangleTexCoords, totalNumberTexCoords * 2 * sizeof(float));
	}

	// Just create the indices from the list of vertices.

	model->numberIndices = totalNumberVertices;

	if (totalNumberVertices > 0)
	{
		model->indices = (GLuint *)malloc(totalNumberVertices * sizeof(GLuint));

		if (model->indices == 0)
		{
			return false;
		}

		for (indicesCounter = 0; indicesCounter < totalNumberVertices; indicesCounter++)
		{
			model->indices[indicesCounter] = indicesCounter;
		}
	}
	return true;
}

static bool MallocTempMemory(float **vertices, float **normals, float **uvs, float **triangleVertices, float **triangleNormals, float **triangleUvs)
{
	*vertices = (float *)malloc(3 * MAX_ATTRIBUTES * sizeof(float));
	if (!*vertices)
	{
		return false;
	}
	*normals = (float *)malloc(3 * MAX_ATTRIBUTES * sizeof(float));
	if (!*normals)
	{
		return false;
	}
	*uvs = (float *)malloc(2 * MAX_ATTRIBUTES * sizeof(float));
	if (!*uvs)
	{
		return false;
	}
	*triangleVertices = (float *)malloc(3 * MAX_ATTRIBUTES * sizeof(float));
	if (!*triangleVertices)
	{
		return false;
	}
	*triangleNormals = (float *)malloc(3 * MAX_ATTRIBUTES * sizeof(float));
	if (!*triangleNormals)
	{
		return false;
	}
	*triangleUvs = (float *)malloc(2 * MAX_ATTRIBUTES * sizeof(float));
	if (!*triangleUvs)
	{
		return false;
	}

	return true;
}

static void FreeTempMemory(float **vertices, float **normals, float **uvs, float **triangleVertices, float **triangleNormals, float **triangleUvs)
{
	if (vertices && *vertices)
	{
		free(*vertices);
		*vertices = 0;
	}

	if (normals && *normals)
	{
		free(*normals);
		*normals = 0;
	}

	if (uvs && *uvs)
	{
		free(*uvs);
		*uvs = 0;
	}

	if (triangleVertices && *triangleVertices)
	{
		free(*triangleVertices);
		*triangleVertices = 0;
	}

	if (triangleNormals && *triangleNormals)
	{
		free(*triangleNormals);
		*triangleNormals = 0;
	}

	if (triangleUvs && *triangleUvs)
	{
		free(*triangleUvs);
		*triangleUvs = 0;
	}
}

bool getMeshAllocateStatus()
{
	if (meshAllocs != 0)
	{
		printf("Memory leak! ");
		printf("Mesh allocations left %i\n", meshAllocs);
		return false;
	}
	return true;
}

bool getMeshDataAllocateStatus()
{
	if (dataAllocs != 0)
	{
		printf("Memory leak! ");
		printf("Mesh data allocations left %i\n", dataAllocs);
		return false;
	}
	return true;
}