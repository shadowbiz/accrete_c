#ifndef SE_STRUCTS_H
#define SE_STRUCTS_H

#include "globals.h"
#include "shared.h"
#include "acstructs.h"

typedef struct seNode seNode;
typedef struct seList seList;
typedef struct ShaderProgram ShaderProgram;
typedef struct Mesh Mesh;
typedef struct MeshData MeshData;
typedef struct seLight seLight;
typedef struct seCamera seCamera;
typedef struct Image Image;
typedef struct Material Material;

typedef struct Stage Stage;
typedef struct Input Input;
typedef struct PhState PhState;
typedef struct PhBody PhBody;
typedef struct PhState PhState;
typedef struct PhDerivative PhDerivative;
typedef struct seShape seShape;
typedef struct seBody seBody;
typedef struct seSpace seSpace;

typedef struct seParticleForceGenerator seParticleForceGenerator;
typedef struct seParticleForceData seParticleForceData;
typedef struct seParticleEmitter seParticleEmitter;

typedef enum PrimitiveType PrimitiveType;
typedef enum SpaceBodyType SpaceBodyType;

typedef struct InputConfig InputConfig;
typedef struct InputKey InputKey;

typedef struct seEntity seEntity;

struct seNode
{
	void *element;
	seNode *next;
};

struct seList
{
	seNode *firstNode;
	uint size;
};

typedef struct seArray
{
	size_t length;
	size_t capacity;

	void **data;

} seArray;

typedef struct seVector
{
	void *items;
	size_t capacity;
	size_t size;
	size_t itemSize;

} seVector;

struct ShaderProgram
{
	uint program;
	uint vars[32];
};

struct Mesh
{
	uint vars[32];
};

struct MeshData
{
	float *vertices;
	float *normals;
	float *tangents;
	float *uvs;

	uint *indices;

	uint numberVertices;
	uint numberIndices;
};

struct seLight
{
	char name[30];

	vec3 *position;
	vec3 *ambient;
	vec3 *diffuse;
	vec3 *specular;
};

struct seCamera
{
	char name[30];

	float fov;

	vec3 *up;
	vec3 *left;
	vec3 *forward;
	vec3 *position;
	vec3 *rotation;
	vec3 *target;

	mat4 *viewMatrix;
	mat4 *projectionMatrix;

	bool isMain;

	Stage *stage;
};

struct Image
{
	int width;
	int height;
	uint format;
	ubyte *data;
};

typedef enum TextureType
{
	TEX_DIFFUSE,
	TEX_NORMAL,
	TEX_HEIGHT

} TextureType;

struct Material
{
	ShaderProgram *shader;

	vec3 *diffuseColor;
	vec3 *ambientColor;
	vec3 *specularColor;

	uint diffuseMap;
	uint normalMap;
	uint heightMap;

	GLenum renderMode;

	bool transparent;
};

/*
enum PrimitiveType
{
	Cube, Sphere, Disc, Plane, File
};


struct GameObject 
{
	char			name[80];

	Mesh			*mesh;
	Material		*material;

	vec3			position;
	vec3			rotation;

	mat4			positionMatrix;
	mat4			rotationMatrix;
	mat4			scaleMatrix;
	mat4			modelMatrix;
	mat4			modelViewMatrix;
	mat3			normalMatrix;
	float			scale;
	bool			hasParent;
	GameObject		*parent;

	seCamera		*camera;
	seLight			*light;

	int				type;
	float			delta;

	void			(*Render)(GameObject* entity);
};

enum SpaceBodyType
{
	Star, Planet, Moon

};

struct SpaceBody
{
	SpaceBodyType	type;
	GameObject		*entity;

	bool			hasOrbitDisk;
	GameObject		*orbitDisk;

	float			radius;
	float			yearLength;
	float			dayLength;
	float			yearTime;
	float			dayTime;
	float			inclination;
	float			orbit;
	float			axialTilt;

	uint			bodyClass;
	uint			bodyNo;

	bool			hasParent;
	SpaceBody		*parent;

	bool			hasChildren;
	uint			totalChildren;
	SpaceBody		*children;

	void			(*Update)(SpaceBody *body);
};

*/

struct Stage
{
	char name[30];

	seArray *stageObjects;

	seLight *light;
	seCamera *camera;

	lua_State *script;

	void (*Update)(Stage *);
	void (*Render)(Stage *);
};

struct InputConfig
{
	int exitCode;

	int rightCode;
	int leftCode;
	int upCode;
	int downCode;

	int actionCode;

	int special1Code;
	int special2Code;
};

struct InputKey
{
	int keyCode;
	bool isPressed;
};

struct Input
{
	InputConfig *inputConfig;

	bool rightPressed;
	bool leftPressed;
	bool upPressed;
	bool downPressed;

	bool actionPressed;
	bool exitPressed;
	bool special1Pressed;
	bool special2Pressed;

	bool leftMouse;
	bool rightMouse;
	bool middleMouse;

	vec3 *mouseWorldPosition;
	vec3 *mouseScreenPosition;

	byte keysPressed;

	float pressTime;

	bool isActive;

	int lastX;
	int lastY;
	int lastZ;

	float mouseDeltaX;
	float mouseDeltaY;
	float mouseDeltaZ;

	float mouseX;
	float mouseY;

	bool (*Update)(Input *input);
};

struct seShape
{
	char name[80];

	vec3 *position;
	float scale;

	Mesh *mesh;
	Material *material;

	seCamera *camera;
	seLight *light;

	mat4 *MVP;
	mat4 *modelViewMatrix;
	mat3 normalMatrix;

	void (*Update)(seShape *entity);
	void (*Render)(seShape *entity);
	void (*RenderAt)(seShape *entity, const vec3 *position);
};

typedef struct Renderer
{
	seArray *beautyPassArray;

} Renderer;

typedef enum seParticleType
{
	NONE,
	PISTOL,
	ARTILLERY,
	FIREBALL,
	LASER,
	EXPLOSION

} seParticleType;

typedef struct seParticle seParticle;

struct seParticle
{
	vec3 forceAccum;

	vec3 position;
	vec3 velocity;
	vec3 acceleration;

	float damping;
	float inverseMass;

	float bornTime;

	seParticleType type;

	void (*ResetForce)(seParticle *particle);
	void (*AddForce)(seParticle *particle, const vec3 *force);
	void (*Update)(seParticle *particle);
};

struct seParticleForceGenerator
{
	vec3 gravity;
	float velDrag;
	float vecDragSquared;

	bool hasGravity;
	bool hasDrag;

	void (*SetDrag)(seParticleForceGenerator *manager, const float drag);
	void (*SetGravity)(seParticleForceGenerator *manager, const float x, const float y, const float z);
	void (*SetGravityVec)(seParticleForceGenerator *manager, const vec3 *gravity);

	void (*UpdateForce)(seParticleForceGenerator *manager, seParticle *particle);
};

struct seParticleForceData
{
	seParticle *particle;
	seParticleForceGenerator *forceGenerator;
};

struct seParticleEmitter
{
	seArray *particles;

	seParticleForceGenerator *generator;

	float boundX;
	float boundY;
	float boundZ;

	void (*Render)(seParticleEmitter *emitter, seShape *shape);
	void (*Update)(seParticleEmitter *emitter);
	void (*AddParticle)(seParticleEmitter *emitter, const vec3 *position, const seParticleType type);
	void (*RemoveParticle)(seParticleEmitter *emitter, seParticle *particle);
};

typedef enum seComponentType
{
	CMP_RENDERER,
	CMP_PHYSICS

} seComponentType;

typedef enum seComponentFlag
{
	CMP_RENDERER_TYPE,
	CMP_RENDERER_TYPE_TRIANGLES,
	CMP_RENDERER_TYPE_LINES,
	CMP_RENDERER_MESH,
	CMP_RENDERER_MATERIAL,
	CMP_RENDERER_CAMERA,
	CMP_RENDERER_LIGHT

} seComponentFlag;

typedef struct seRenderer
{
	seEntity *entity;

	Mesh *mesh;
	Material *material;

	seCamera *camera;
	seLight *light;

	mat4 *MVP;
	mat4 *modelViewMatrix;

	mat3 normalMatrix;

	void (*Render)(seEntity *entity);
	void (*Update)(seEntity *entity);
	void (*RenderAt)(seEntity *entity, const vec3 *position);

} seRenderer;

struct seEntity
{
	char name[80];

	bool active;

	vec3 *position;
	vec3 *rotation;

	float scale;

	bool renderable;
	seRenderer *renderer;

	bool useScript;
	lua_State *script;

	void (*AddComponent)(seEntity *entity, void *component, const seComponentType type);
	void (*RemoveCompponentType)(seEntity *entity, const seComponentType type);
	void *(*GetComponentType)(seEntity *entity, const seComponentType type);

	void (*Init)(seEntity *entity);
	void (*Update)(seEntity *entity);

	void (*Render)(seEntity *entity);
	void (*RenderAt)(seEntity *entity, const vec3 *position);
};

typedef struct seKerning
{
	wchar_t charcode;
	float kerning;

} seKerning;

typedef struct seTextureGlypth
{
	wchar_t charcode;

	uint id;

	size_t width;
	size_t height;

	int offset_x;
	int offset_y;

	float advance_x;
	float advance_y;

	float s0;
	float t0;
	float s1;
	float t1;

	seVector *kerning;

	int outline_type;

	float outline_thickness;

} seTextureGlypth;

typedef struct seTextureAtlas
{
	seVector *nodes;

	size_t width;
	size_t height;

	size_t depth;

	size_t used;

	uint id;
	ubyte *data;

} seTextureAtlas;

typedef struct seTextureFont
{
	seVector *glyphs;

	seTextureAtlas *atlas;

	char *filename;

	float size;

	int hinting;

	/**
     * Outline type (0 = None, 1 = line, 2 = inner, 3 = outer)
     */
	int outline_type;
	float outline_thickness;

	int filtering;

	ubyte lcd_weights[5];

	float height;

	float linegap;
	float ascender;
	float descender;
	float underline_position;
	float underline_thickness;

} seTextureFont;

typedef struct Text2D
{
	uint indexCount;
	uint vertexBuffer;
	uint uvBuffer;

	uint EBO;
	uint VAO;

	uint texture;
	uint shader;
	uint uniform;

	char *text;

} Text2D;

#endif // !STRUCTS_H
