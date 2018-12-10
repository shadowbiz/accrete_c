#include "renderercomponent.h"

static uint allocs = 0;

seRenderer *seRendererAllocate()
{
    seRenderer *renderer = seNew(seRenderer, 1);

    if (!renderer)
    {
        fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
        exit(EXIT_FAILURE);
    }

    allocs++;

    mat3Identity(renderer->normalMatrix);

    renderer->modelViewMatrix = newMat4Identity();
    renderer->MVP = newMat4();

    renderer->entity = NULL;
    renderer->mesh = NULL;
    renderer->material = NULL;
    renderer->camera = NULL;
    renderer->light = NULL;

    renderer->Render = seRendererRender;
    renderer->Update = seRendererUpdate;
    renderer->RenderAt = seRendererRenderAt;

    return renderer;
}

void freeRenderer(seRenderer *renderer)
{
    if (!renderer)
        return;

    printf("Destroying renderer component of %s...\n", renderer->entity->name);

    freeMat4(renderer->modelViewMatrix);
    freeMat4(renderer->MVP);
    free(renderer);

    renderer = NULL;
    allocs--;
}

seRenderer *newRenderer(seEntity *entity, Mesh *mesh, Material *material, seCamera *camera, seLight *light)
{
    seRenderer *renderer = seRendererAllocate();
    renderer->entity = entity;
    renderer->mesh = mesh;
    renderer->material = material;
    renderer->camera = camera;
    renderer->light = light;

    return renderer;
}

void seRendererUpdate(seEntity *entity)
{
    seRenderer *renderer = entity->renderer;

    mat4 transform;

    mat4Identity(&transform);
    mat4TranslateVec(&transform, entity->position);
    mat4Scale(transform.data, entity->scale);

    mat4MultiplyAsm(renderer->modelViewMatrix->data, renderer->camera->viewMatrix->data, transform.data);
    mat4MultiplyAsm(renderer->MVP->data, renderer->camera->projectionMatrix->data, renderer->modelViewMatrix->data);

    if (entity->scale == 1.0)
    {
        mat4ExtractMat3(renderer->normalMatrix, renderer->modelViewMatrix->data);
    }
    else
    {
        mat4 ITModelViewMat;
        mat4InverseSSE(ITModelViewMat.data, renderer->modelViewMatrix->data);
        mat4TransposeSSE(&ITModelViewMat);
        mat4ExtractMat3(renderer->normalMatrix, ITModelViewMat.data);
    }
}

void seRendererRender(seEntity *entity)
{
    if (entity->active == false)
        return;

    static uint activeVAO = 0;
    static uint activeEBO = 0;

    seRenderer *renderer = entity->renderer;

    bool firstUse = MaterialPrepare(renderer->material);

    if (firstUse)
    {
        glUniform3fv(renderer->material->shader->vars[SHADER_LIGHT_POSITION], 1, renderer->light->position->data);
        glUniform3fv(renderer->material->shader->vars[SHADER_LIGHT_LA], 1, renderer->light->ambient->data);
        glUniform3fv(renderer->material->shader->vars[SHADER_LIGHT_LD], 1, renderer->light->diffuse->data);
        glUniform3fv(renderer->material->shader->vars[SHADER_LIGHT_LS], 1, renderer->light->specular->data);
    }

    glUniformMatrix4fv(renderer->material->shader->vars[SHADER_MVP], 1, GL_FALSE, renderer->MVP->data);
    glUniformMatrix4fv(renderer->material->shader->vars[SHADER_VIEW_MATRIX], 1, GL_FALSE, renderer->camera->viewMatrix->data);
    glUniformMatrix4fv(renderer->material->shader->vars[SHADER_MODELVIEW_MATRIX], 1, GL_FALSE, renderer->modelViewMatrix->data);
    glUniformMatrix3fv(renderer->material->shader->vars[SHADER_NORMAL_MATRIX], 1, GL_FALSE, renderer->normalMatrix);

    if (activeVAO != renderer->mesh->vars[BUFFER_VAO])
    {
        glBindVertexArray(renderer->mesh->vars[BUFFER_VAO]);
        activeVAO = renderer->mesh->vars[BUFFER_VAO];
    }

    if (activeEBO != renderer->mesh->vars[BUFFER_EBO])
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->mesh->vars[BUFFER_EBO]);
        activeEBO = renderer->mesh->vars[BUFFER_EBO];
    }

    glDrawElements(renderer->material->renderMode, renderer->mesh->vars[BUFFER_INDEX_COUNT], GL_UNSIGNED_INT, 0);
}

void seRendererRenderAt(seEntity *entity, const vec3 *position)
{
}

bool getRendererAllocStatus()
{
    if (allocs != 0)
    {
        printf("Memory leak! ");
        printf("Renderer component allocations left %i\n", allocs);
        return false;
    }
    return true;
}
