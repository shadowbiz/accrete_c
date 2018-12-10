#ifndef SE_CAMERA_H
#define SE_CAMERA_H

#include "engine.h"
#include "structs.h"
#include "math.h"

void freeCamera(seCamera *camera);

seCamera *newCamera(const float fov, const vec3 *position, const vec3 *target, Stage *stage);

void setCameraPosition(seCamera *camera, const vec3 *position);
void setCameraTargetPosition(seCamera *camera, const vec3 *target);
void setCameraRotation(seCamera *camera, const vec3 *rotation);

void CameraTranslate(seCamera *camera, const vec3 *delta);
void CameraTargetTranslate(seCamera *camera, const vec3 *delta);
void CameraRotate(seCamera *camera, const vec3 *delta);

void CameraUpdate(seCamera *camera);

static void CameraCalculateVectors(seCamera *camera);
static void CameraGetRotationMatrix(mat3 result, const vec3 *axis);
static void CameraTransformCoord(vec3 *vector, const mat3 matrix);
static void LookAt(seCamera *camera);
static bool seFrustum(mat4 *result, const float left, const float right, const float bottom, const float top, const float nearVal, const float farVal);
bool sePerspective(mat4 *result, const float fovy, const float aspect, const float zNear, const float zFar);
bool ScreenToWorld(vec3 *result, const vec3 *pointOnScreen, const seCamera *camera);
void seProject(vec3 *result, const vec3 *position, const mat4 *modelViewMatrix, const mat4 *projectionMatrix);

bool seUnproject(vec3 *worldPoint, const vec3 *point, const mat4 *viewMat, const mat4 *projection);

bool getCameraAllocStatus();

#endif
