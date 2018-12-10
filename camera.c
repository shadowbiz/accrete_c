#include "camera.h"

static uint allocs = 0;

void freeCamera(seCamera *camera)
{
    if (!camera)
        return;

    printf("Destroying camera %s...\n", camera->name);
    freeVec3(camera->up);
    freeVec3(camera->left);
    freeVec3(camera->forward);
    freeVec3(camera->position);
    freeVec3(camera->rotation);
    freeVec3(camera->target);
    freeMat4(camera->projectionMatrix);
    freeMat4(camera->viewMatrix);
    free(camera);
    camera = NULL;
    allocs--;
}

seCamera *newCamera(const float fov, const vec3 *position, const vec3 *target, Stage *stage)
{
    static int num = 0;
    seCamera *result = seNew(seCamera, 1);

    if (!result)
    {
        fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    printf("Creating camera...\n");

    result->up = newVec3();
    result->left = newVec3();
    result->forward = newVec3();
    result->position = newVec3();
    result->rotation = newVec3();
    result->target = newVec3();

    result->projectionMatrix = newMat4();
    result->viewMatrix = newMat4();

    allocs++;

    result->stage = stage;
    result->fov = fov;

    vec3Copy(result->position, position);
    vec3Copy(result->target, target);

    sePerspective(result->projectionMatrix, result->fov, (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, NEAR_CLIPPING_RANGE, FAR_CLIPPPING_RANGE);
    CameraCalculateVectors(result);

    sprintf(result->name, "Camera %i", num);
    num++;

    return result;
}

static void CameraCalculateVectors(seCamera *camera)
{
    seCamera *localCamera = camera;

    vec3Set(localCamera->up, 0, 0, 1);
    vec3Subtract(localCamera->forward, localCamera->target, localCamera->position);
    vec3Normalize(localCamera->forward);
    vec3Cross(localCamera->left, localCamera->up, localCamera->forward);
    vec3Normalize(localCamera->left);
    vec3Cross(localCamera->up, localCamera->forward, localCamera->left);
    vec3Normalize(localCamera->up);

    return;
}

void CameraUpdate(seCamera *camera)
{
    seCamera *localCamera = camera;

    CameraCalculateVectors(localCamera);
    LookAt(localCamera);
}

void CameraTranslate(seCamera *camera, const vec3 *delta)
{
    seCamera *localCamera = camera;
    vec3 tranlation = *delta;
    tranlation.z = 0;
    vec3Invert(&tranlation);

    vec3Add(localCamera->position, localCamera->position, &tranlation);

    if (delta->z)
    {
        vec3 tmp;
        vec3MultScalar(&tmp, localCamera->forward, delta->z);
        vec3Add(&tmp, localCamera->position, &tmp);
        vec3Copy(localCamera->position, &tmp);
    }

    return;

    if (delta->x)
    {
        vec3MultScalar(&tranlation, localCamera->left, delta->x);
        vec3Add(localCamera->position, localCamera->position, &tranlation);
    }

    if (delta->y)
    {
        vec3MultScalar(&tranlation, localCamera->up, -delta->y);
        vec3Add(localCamera->position, localCamera->position, &tranlation);
    }

    if (delta->z)
    {
        vec3MultScalar(&tranlation, localCamera->forward, delta->z);
        vec3Add(localCamera->position, localCamera->position, &tranlation);
    }
}

void CameraTargetTranslate(seCamera *camera, const vec3 *delta)
{
    seCamera *localCamera = camera;
    vec3 tranlation = *delta;
    tranlation.z = 0;
    vec3Invert(&tranlation);
    vec3Add(localCamera->target, localCamera->target, &tranlation);

    return;
    //tranlation = {0.0, 0.0, 0.0};

    if (delta->x)
    {
        vec3MultScalar(&tranlation, localCamera->left, delta->x);
        vec3Add(localCamera->target, localCamera->target, &tranlation);
    }

    if (delta->y)
    {
        vec3MultScalar(&tranlation, localCamera->up, -delta->y);
        vec3Add(localCamera->target, localCamera->target, &tranlation);
    }

    if (delta->z)
    {
        //vec3Scale(tranlation, camera->forward, delta[2]);
        //vec3Add(camera->target, camera->target, tranlation);
    }
}

void setCameraTargetPosition(seCamera *camera, const vec3 *target)
{
    vec3Copy(camera->target, target);
}

void setCameraPosition(seCamera *camera, const vec3 *position)
{
    vec3Copy(camera->position, position);
}

void setCameraRotation(seCamera *camera, const vec3 *rotation)
{
    vec3Copy(camera->rotation, rotation);
}

void CameraRotate(seCamera *camera, const vec3 *delta)
{
    vec3Add(camera->rotation, camera->rotation, delta);
}

static void CameraGetRotationMatrix(mat3 matrix, const vec3 *yawPitchRoll)
{
    float cYaw, cPitch, cRoll, sYaw, sPitch, sRoll;

    cYaw = seCos(yawPitchRoll->x);
    cPitch = seCos(yawPitchRoll->y);
    cRoll = seCos(yawPitchRoll->z);

    sYaw = seSin(yawPitchRoll->x);
    sPitch = seSin(yawPitchRoll->y);
    sRoll = seSin(yawPitchRoll->z);

    matrix[0] = cRoll * cYaw + sRoll * sPitch * sYaw;
    matrix[1] = sRoll * cPitch;
    matrix[2] = cRoll * -sYaw + sRoll * sPitch * cYaw;

    matrix[3] = -sRoll * cYaw + cRoll * sPitch * sYaw;
    matrix[4] = cRoll * cPitch;
    matrix[5] = sRoll * sYaw + cRoll * sPitch * cYaw;

    matrix[6] = cPitch * sYaw;
    matrix[7] = -sPitch;
    matrix[8] = cPitch * cYaw;
}

inline static void LookAt(seCamera *camera)
{
    seCamera *localCamera = camera;
    vec3 minus_eye;
    vec3MultScalar(&minus_eye, localCamera->position, -1.0);

    localCamera->viewMatrix->rows[0] = _mm_set_ps(0.0f, -localCamera->forward->x, localCamera->up->x, localCamera->left->x);
    localCamera->viewMatrix->rows[1] = _mm_set_ps(0.0f, -localCamera->forward->y, localCamera->up->y, localCamera->left->y);
    localCamera->viewMatrix->rows[2] = _mm_set_ps(0.0f, -localCamera->forward->z, localCamera->up->z, localCamera->left->z);
    localCamera->viewMatrix->rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

    mat4TranslateVec(localCamera->viewMatrix, &minus_eye);
}

static bool seFrustum(mat4 *result, const float left, const float right, const float bottom, const float top, const float nearVal, const float farVal)
{
    const float rightMinusLeft = right - left;
    const float topMinusBottom = top - bottom;
    const float farMinusNear = farVal - nearVal;

    if (rightMinusLeft == 0.0)
        return false;
    if (topMinusBottom == 0.0)
        return false;
    if (farMinusNear == 0.0)
        return false;

    result->rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 2.0 * nearVal / (rightMinusLeft));
    result->rows[1] = _mm_set_ps(0.0f, 0.0f, 2.0 * nearVal / (topMinusBottom), 0.0);
    result->rows[2] = _mm_set_ps(-1.0, -(farVal + nearVal) / (farMinusNear), (top + bottom) / (topMinusBottom), (right + left) / (rightMinusLeft));
    result->rows[3] = _mm_set_ps(0.0f, -(2.0 * farVal * nearVal) / (farMinusNear), 0.0f, 0.0f);

    return true;
}

bool sePerspective(mat4 *result, const float fovy, const float aspect, const float zNear, const float zFar)
{
    float xmin, xmax, ymin, ymax;

    ymax = zNear * seTan(fovy * PI / 360.0);
    ymin = -ymax;
    xmin = ymin * aspect;
    xmax = ymax * aspect;

    return seFrustum(result, xmin, xmax, ymin, ymax, zNear, zFar);
}

bool seUnproject(vec3 *result, const vec3 *point, const mat4 *viewMat, const mat4 *projection)
{
    mat4 m, A;
    mat4MultiplyAsm(A.data, projection->data, viewMat->data);
    mat4InverseSSE(m.data, A.data);

    seQuat in, out;
    seVec4Set(in, point->x, point->y, point->z, 1.0);

    in[0] /= WINDOW_WIDTH;
    in[1] /= WINDOW_HEIGHT;

    in[0] = 2.0 * in[0] - 1.0;
    in[1] = 2.0 * in[1] - 1.0;
    in[2] = 2.0 * in[2] - 1.0;

    quatMultiplyMat4(out, &m, in);

    if (out[3] == 0.0)
        return false;

    float invW = 1.0 / out[3];

    vec3Set(result, out[0] * invW, out[1] * invW, out[2] * invW);
    return true;
}

void seProject(vec3 *result, const vec3 *position, const mat4 *modelview, const mat4 *projection)
{
    int viewport[4] = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    float fTempo[8], objX = position->x, objY = position->y, objZ = position->z;

    fTempo[0] = modelview->data[0] * objX + modelview->data[4] * objY + modelview->data[8] * objZ + modelview->data[12];
    fTempo[1] = modelview->data[1] * objX + modelview->data[5] * objY + modelview->data[9] * objZ + modelview->data[13];
    fTempo[2] = modelview->data[2] * objX + modelview->data[6] * objY + modelview->data[10] * objZ + modelview->data[14];
    fTempo[3] = modelview->data[3] * objX + modelview->data[7] * objY + modelview->data[11] * objZ + modelview->data[15];

    fTempo[4] = projection->data[0] * fTempo[0] + projection->data[4] * fTempo[1] + projection->data[8] * fTempo[2] + projection->data[12] * fTempo[3];
    fTempo[5] = projection->data[1] * fTempo[0] + projection->data[5] * fTempo[1] + projection->data[9] * fTempo[2] + projection->data[13] * fTempo[3];
    fTempo[6] = projection->data[2] * fTempo[0] + projection->data[6] * fTempo[1] + projection->data[10] * fTempo[2] + projection->data[14] * fTempo[3];
    fTempo[7] = -fTempo[2];

    if (fTempo[7] == 0.0)
        return;

    fTempo[7] = 1.0 / fTempo[7];
    fTempo[4] *= fTempo[7];
    fTempo[5] *= fTempo[7];
    fTempo[6] *= fTempo[7];

    vec3Set(result, (fTempo[4] * 0.5 + 0.5) * viewport[2] + viewport[0], (fTempo[5] * 0.5 + 0.5) * viewport[3] + viewport[1], (1.0 + fTempo[6]) * 0.5);

    result->x /= viewport[2];
    result->y /= viewport[3];
    result->y = 1.0 - result->y;
}

bool getCameraAllocStatus()
{
    if (allocs != 0)
    {
        printf("Memory leak! ");
        printf("Camera allocations left %i\n", allocs);
        return false;
    }

    return true;
}
