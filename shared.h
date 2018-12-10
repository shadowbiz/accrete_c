#ifndef SE_SHARED_H
#define SE_SHARED_H

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#include <limits.h>
#include <assert.h>
#include <float.h>
#include <intrin.h>
#include <xmmintrin.h>
#include <malloc.h>
#include <omp.h>

#include "lua/lua.h"
#include "lua/lauxlib.h"
#include "lua/lualib.h"

#define GLEW_STATIC

#include <gl/glew.h>
#include <glfw/glfw3.h>

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned char ubyte;
typedef char byte;
typedef unsigned short ushort;
typedef long double ldouble;

#define bool unsigned char
#define true 1
#define false 0

#define seSqrt sqrtf
#define seSin sinf
#define seCos cosf
#define seAsin asinf
#define seAcos acosf
#define seAtan2 atan2f
#define seFmod fmodf
#define seExp expf
#define sePow powf
#define seFloor floorf
#define seCeil ceilf
#define seLog logf
#define seTan tanf

typedef float seVec2[2];
typedef float seVec3[3];
typedef float seQuat[4];

typedef seQuat seVec4;

typedef float mat3[9];

typedef union vec2 {
	float data[2];
	struct
	{
		float x, y;
	};
	__m128 mmvalue;

} vec2;

typedef union vec3 {
	float data[3];
	struct
	{
		float x, y, z;
	};
	__m128 mmvalue;

} vec3;

typedef union vec4 {
	float data[4];
	struct
	{
		float x, y, z, w;
	};
	__m128 mmvalue;

} vec4;

typedef union mat4 {
	float data[16];
	__m128 rows[4];

} mat4;

#define setName(a, b) sprintf(a, "%s", b)

#ifdef __cplusplus
#define seNew(type, count) (type *)calloc((count), sizeof(type))
#else
#define seNew(type, count) calloc((count), sizeof(type))
#endif

#include "structs.h"
#include "math.h"
#endif // !SHARED_H
