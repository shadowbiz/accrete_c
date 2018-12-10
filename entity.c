#include "entity.h"

static uint allocs = 0;

seEntity *newEntity()
{
	seEntity *entity = seNew(seEntity, 1);

	if (!entity)
	{
		fprintf(stderr, "line %d: No more memory for allocating data\n", __LINE__);
		exit(EXIT_FAILURE);
	}
	allocs++;

	entity->scale = 1.0;
	entity->active = true;
	entity->renderable = false;

	entity->position = newVec3();
	entity->rotation = newVec3();

	sprintf(entity->name, "Entity %i", allocs);

	entity->AddComponent = seEntityAddComponent;
	entity->Render = seEntityRender;
	entity->Update = seEntityUpdate;
	entity->GetComponentType = getEntityComponentType;

	entity->useScript = false;

	return entity;
}

void seEntityInit(seEntity *entity)
{
	if (entity->useScript == true)
	{
		entity->script = luaL_newstate();
		luaL_openlibs(entity->script);

		int err = 0.0;
		lua_State *lua = entity->script;

		if (err = luaL_dofile(lua, "Data/Scripts/gameobject.lua") == 0.0)
		{
			lua_getglobal(lua, "init");
			if (lua_isfunction(lua, -1))
			{
				lua_pushnumber(lua, 0);
				lua_pushnumber(lua, 0);
				lua_pushnumber(lua, 0);
				lua_call(lua, 3, 0);
			}
			/*
			lua_getglobal(lua, "update");
			if (lua_isfunction(lua, -1))
			{
				lua_pushnumber(lua, DELTA_TIME);
				lua_call(lua, 1, 0);
			}
			*/
		}
		else
		{
			printf("Error, unable to run entity lua script...\n");
		}
	}
	else
	{
		entity->script = NULL;
	}
}

void seEntityUpdate(seEntity *entity)
{
	if (entity->active == false)
		return;

	if (entity->renderable)
	{
		entity->renderer->Update(entity);
	}

	if (entity->useScript)
	{
		lua_State *lua = entity->script;
		lua_getglobal(lua, "update");
		if (lua_isfunction(lua, -1))
		{
			lua_pushnumber(lua, DELTA_TIME);
			lua_call(lua, 1, 0);
		}

		float x = entity->position->x;
		float y = entity->position->y;
		float z = entity->position->z;

		lua_getglobal(lua, "getX");
		if (lua_isfunction(lua, -1))
		{
			lua_call(lua, 0, 1);
			x = lua_tonumber(lua, -1);
		}

		lua_getglobal(lua, "getY");
		if (lua_isfunction(lua, -1))
		{
			lua_call(lua, 0, 1);
			y = lua_tonumber(lua, -1);
		}

		lua_getglobal(lua, "getZ");
		if (lua_isfunction(lua, -1))
		{
			lua_call(lua, 0, 1);
			z = lua_tonumber(lua, -1);
		}

		setEntityPosition(entity, x, y, z);
	}
}

void seEntityRender(seEntity *entity)
{
}

void *getEntityComponentType(seEntity *entity, const seComponentType type)
{
	switch (type)
	{
	case CMP_RENDERER:
		return entity->renderer;
		break;
	}

	return NULL;
}

void seEntityAddComponent(seEntity *entity, void *component, const seComponentType type)
{
	switch (type)
	{
	case CMP_RENDERER:
		entity->renderer = (seRenderer *)component;
		entity->renderable = true;
		entity->Render = seRendererRender;
		break;
	}
}

void setEntityPositionVec(seEntity *entity, const vec3 *position)
{
	vec3Copy(entity->position, position);
}

void setEntityPosition(seEntity *entity, const float x, const float y, const float z)
{
	vec3Set(entity->position, x, y, z);

	if (entity->useScript)
	{
		lua_State *lua = entity->script;
		lua_getglobal(lua, "setPosition");
		if (lua_isfunction(lua, -1))
		{
			lua_pushnumber(lua, x);
			lua_pushnumber(lua, y);
			lua_pushnumber(lua, z);
			lua_call(lua, 3, 0);
		}
	}
}

void setEntityScale(seEntity *entity, const float s)
{
	entity->scale = s;
}

void freeEntity(seEntity *entity)
{
	if (!entity)
		return;

	printf("Destroying entity %s...\n", entity->name);

	freeVec3(entity->position);
	freeVec3(entity->rotation);

	freeRenderer(entity->renderer);

	if (entity->useScript)
	{
		lua_close(entity->script);
	}

	free(entity);

	entity = NULL;

	allocs--;
}

bool getEntityAllocStatus()
{
	if (allocs != 0)
	{
		printf("Memory leak! ");
		printf("Entity allocations left %i\n", allocs);
		return false;
	}
	return true;
}