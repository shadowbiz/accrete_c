#ifndef SE_MESH_H
#define SE_MESH_H

#include "structs.h"
#include "loader.h"
#include "math.h"
#include "engine.h"
#include "array.h"

Mesh *defaultSphereMesh;
Mesh *defaultCubeMesh;
Mesh *defaultRectMesh;
Mesh *defaultTerrainMesh;
Mesh *defaultGridMesh;
Material *defaultMaterial;
Material *defaultGridMaterial;
Material *defaultSelectionMaterial;
Material *defaultTerrainMaterial;

static Mesh *MeshAllocate();
void freeMesh(Mesh *handle);

static MeshData *MeshDataAllocate();
void freeMeshData(MeshData *model);

Mesh *newMeshDisc(const uint numberSectors);
Mesh *newMeshCube();
Mesh *newMeshSphere(const uint numberSlices);
Mesh *newMeshPlane2D();
Mesh *newMeshPlane(const float x, const float y);
Mesh *newMeshObjFile(const char *filename);
Mesh *newMeshCircle(const uint numberSectors);
Mesh *newMeshRectangle(const vec3 *recStart, const vec3 *recEnd);
Mesh *newArrow();
Mesh *newMeshGrid(const float gridStep, const float gridDimensions);
Mesh *newMeshGeoSphere();
Mesh *newMeshPlane128();

void MeshCreateDefault(void);
void MeshClearDefault(void);

void meshRectangleReshape(Mesh *mesh, const vec3 *recStart, const vec3 *recEnd);

void MeshCreateDome(Mesh *handle, const uint numberSlices);

void MeshCopy(Mesh *result, const Mesh *handle);
static void MeshFromData(Mesh *result, const MeshData *model);

static void MeshDataCalculateTangents(MeshData *model);
bool MeshDataLoadObjFile(MeshData *model, const char *filename);

uint MeshCreateGLBuffer(GLenum target, const void *bufferdata, GLsizei buffersize);
static bool MeshCopyData(MeshData *model, GLuint totalNumberVertices, float *triangleVertices, GLuint totalNumberNormals, float *triangleNormals, GLuint totalNumberTexCoords, float *triangleTexCoords);
static bool MallocTempMemory(float **vertices, float **normals, float **uvs, float **triangleVertices, float **triangleNormals, float **triangleUvs);
static void FreeTempMemory(float **vertices, float **normals, float **uvs, float **triangleVertices, float **triangleNormals, float **triangleUvs);

bool getMeshAllocateStatus();
bool getMeshDataAllocateStatus();

#endif
