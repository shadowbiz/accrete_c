#ifndef SE_MATH_H
#define SE_MATH_H

#include "shared.h"
#include "structs.h"

#define mi(w, t, i, j) 4 * ((i * w + j) * (1 - t) + (j * w + i) * t)

#define flm(k, i, j, mat, n, a, b) \
	__asm fld dword ptr[edx + mi(mat, a, i, k)] __asm fmul dword ptr[ecx + mi(n, b, k, j)]

#define e4(i, j, l, mat, n, a, b)                             \
	flm(0, i, j, mat, n, a, b)                                \
		flm(1, i, j, mat, n, a, b)                            \
			flm(2, i, j, mat, n, a, b)                        \
				flm(3, i, j, mat, n, a, b) __asm faddp st(1), \
		st(0) __asm fxch st(2) __asm faddp st(1), st(0) __asm faddp st(1), st(0) __asm fstp dword ptr[eax + mi(l, 0, i, j)]

#define seVec2Set(vector, x, y) vector[0] = x, vector[1] = y
#define seVec3Set(vector, x, y, z) vector[0] = x, vector[1] = y, vector[2] = z
#define seQuatSet(vector, x, y, z, w) vector[0] = x, vector[1] = y, vector[2] = z, vector[3] = w
#define seVec4Set(vector, x, y, z, w) seQuatSet(vector, x, y, z, w)

#define seVec2SetZero(a) (a[0] = a[1] = 0)
#define seVec3SetZero(a) (a[0] = a[1] = a[2] = 0)
#define seQuatSetZero(a) (a[0] = a[1] = a[2] = a[3] = 0)
#define seVec4SetZero(a) seQuatSetZero(a)

#define seVec2SetOne(a) (a[0] = a[1] = 1.0)
#define seVec3SetOne(a) (a[0] = a[1] = a[2] = 1.0)
#define seQuatSetOne(a) (a[0] = a[1] = a[2] = a[3] = 1.0)
#define seVec4SetOne(a) seQuatSetOne(a)

#define seQuatSetIdentity(a) seQuatSet(a, 0, 0, 0, 1.0f)

#define seVec2Copy(a, b) (a[0] = b[0], a[1] = b[1])
#define seVec3Copy(a, b) (a[0] = b[0], a[1] = b[1], a[2] = b[2])
#define seQuatCopy(a, b) (a[0] = b[0], a[1] = b[1], a[2] = b[2], a[3] = b[3])
#define seVec4Copy(a, b) seQuatCopy(a, b)

#define seVec2Print(a) printf("%.2f, %.2f\n", a[0], a[1]);
#define seVec3Print(a) printf("%.2f, %.2f, %.2f\n", a[0], a[1], a[2]);
#define seQuatPrint(a) printf("%.2f, %.2f, %.2f, %.2f\n", a[0], a[1], a[2], a[2]);
#define seVec4Print(a) seQuatPrint(a)

#ifndef RAND_MAX
#define RAND_MAX 32767
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795f
#endif
#define TWOPI 3.1415926535897932384626433832795f * 2

#define DEGS_IN_RAD PI / 180.0f
#define RADS_IN_DEG 180.0f / PI

#define DegToRad(a) a *DEGS_IN_RAD
#define RadToDeg(a) a *RADS_IN_DEG

#define EPSILON 0.00001 ///< floating point epsilon for single precision. todo: verify epsilon value and usage
#define EPSILON_SQUARED EPSILON *EPSILON

#define INFINITY FLT_MAX

#define sePow2(a) ((a) * (a))
#define sePow3(a) ((a) * (a) * (a))
#define sePow4(a) ((a) * (a) * (a) * (a))
#define sePow1_4(a) seSqrt(seSqrt(a))
#define sePow1_3(a) sePow(a, (1.0 / 3.0))

#define seMax(a, b) a > b ? a : b
#define seMin(a, b) a < b ? a : b

#define seClamp(a, b, c) seMin(seMax(a, b), c)

#define random() ((rand() & 0x7fff) / ((float)0x7fff))
#define crandom() (2.0 * (random() - 0.5))

//////////////////////////////////////////////////////////////////////////
///	Helpers
//////////////////////////////////////////////////////////////////////////

inline static float seRsqrt(float number)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y = number;
	i = *(long *)&y;		   // evil floating point bit level hacking
	i = 0x5f3759df - (i >> 1); // what the fuck?
	y = *(float *)&i;
	y = y * (threehalfs - (x2 * y * y)); // 1st iteration

	return y;
}

inline static float seAbs(float f)
{
	int tmp = *(int *)&f;
	tmp &= 0x7FFFFFFF;
	return *(float *)&tmp;
}

inline static float seRandGauss(void)
{
	static float V1, V2, S, Z;
	static int phase = 0;

	if (phase == 0)
	{
		do
		{
			float U1 = (float)rand() / RAND_MAX;
			float U2 = (float)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);

		Z = V1 * seSqrt(-2 * seLog(S) / S);
	}
	else
	{
		Z = V2 * seSqrt(-2 * seLog(S) / S);
	}

	phase = 1 - phase;

	return (float)Z;
}

inline static float seRandomNumber(float inner, float outer)
{
	float range = outer - inner;
	return (float)rand() / (float)RAND_MAX * range + inner;
}

inline static float seAbout(float value, float variation)
{
	return value + value * seRandomNumber(-variation, variation);
}

inline static float seGauss(float sigma)
{
	float x, y, r;

	do
	{
		x = seRandomNumber(-1.0, 1.0);
		y = seRandomNumber(-1.0, 1.0);
		r = x * x + y * y;
	} while (r == 0 || r > 1.0);

	r = sigma * seSqrt(-2.0 * seLog(r) / r);

	return x * r;
}

inline static float seExponential(float value)
{
	return (-value * seLog(1.0 - seRandomNumber(0.0, 1.0)));
}

inline static long sePoisson(float value)
{
	float t = 0.0;
	long x = 0;

	while (t < value)
	{
		t += seExponential(1.0);
		x++;
	}

	return x - 1;
}

inline static float seLimit(float x)
{
	return x / seSqrt(seSqrt(1 + x * x * x * x));
}

inline static float seSoft(float v, float maximum, float minimum)
{
	float dv = v - minimum;
	float dm = maximum - minimum;

	return (seLimit(2 * dv / dm - 1) + 1) / 2 * dm + minimum;
}

#define seVec2Add(a, b, c) ((a)[0] = (b)[0] + (c)[0], (a)[1] = (b)[1] + (c)[1])
#define seQuatAdd(a, b, c) ((a)[0] = (b)[0] + (c)[0], (a)[1] = (b)[1] + (c)[1], (a)[2] = (b)[2] + (c)[2], (a)[3] = (b)[3] + (c)[3])

#define seVec2Subtract(a, b, c) ((a)[0] = (b)[0] - (c)[0], (a)[1] = (b)[1] - (c)[1])
#define seQuatSubtract(a, b, c) ((a)[0] = (b)[0] - (c)[0], (a)[1] = (b)[1] - (c)[1], (a)[2] = (b)[2] - (c)[2], (a)[3] = (b)[3] - (c)[3])

#define seVec3Dot(x, y) ((x)[0] * (y)[0] + (x)[1] * (y)[1] + (x)[2] * (y)[2])

#define seVec2MultScalar(a, b, s) ((a)[0] = (b)[0] * (s), (a)[1] = (b)[1] * (s))
#define seQuatMultScalar(a, b, s) ((a)[0] = (b)[0] * (s), (a)[1] = (b)[1] * (s), (a)[2] = (b)[2] * (s), (a)[3] = (b)[3] * (s))

#define seVec2Invert(a) ((a)[0] = (a)[0] * (-1), (a)[1] = (a)[1] * (-1))
#define seQuatInvert(a) ((a)[0] = (a)[0] * (-1), (a)[1] = (a)[1] * (1 -), (a)[2] = (a)[2] * (-1), (a)[3] = (a)[3] * (-1))

#define seVec2DivideScalar(a, b, s) ((a)[0] = (b)[0] / (s), (a)[1] = (b)[1] / (s))
#define seQuatDivideScalar(a, b, s) ((a)[0] = (b)[0] / (s), (a)[1] = (b)[1] / (s), (a)[2] = (b)[2] / (s), (a)[3] = (b)[3] / (s))

#define seVec4Add(a, b, c) seQuatAdd(a, b, c)
#define seVec4Subtract(a, b, c) seQuatSubtract(a, b, c)
#define seVec4MultScalar(a, b, s) seQuatMultScalar(a, b, s)
#define seVec4DivideScalar(a, b, s) seQuatDivideScalar(a, b, s)
#define seVec4Invert(a) seQuatInvert(a)

#define seVec4MultiplyMat4(a, b, c) quatMultiplyMat4(a, b, c)

//////////////////////////////////////////////////////////////////////////
///	Vector2D
//////////////////////////////////////////////////////////////////////////

inline static void vec2Set(vec2 *a, const float x, const float y)
{
	a->x = x;
	a->y = y;
}

inline static void vec2SetZero(vec2 *a)
{
	a->x = a->y = 0;
}

inline static void vec2SetOne(vec2 *a)
{
	a->x = a->y = 1.0;
}

inline static void vec2Copy(vec2 *a, const vec2 *b)
{
	*a = *b;
}

inline static void vec2Print(const vec2 *a)
{
	printf("%.2f, %.2f\n", a->x, a->y);
}

inline static void vec2Add(vec2 *result, const vec2 *a, const vec2 *b)
{
	result->x = a->x + b->x;
	result->y = a->y + b->y;
}

inline static void vec2Subtract(vec2 *result, const vec2 *a, const vec2 *b)
{
	result->x = a->x - b->x;
	result->y = a->y - b->y;
}

inline static void vec2MultScalar(vec2 *result, const vec2 *a, const float b)
{
	result->x = a->x * b;
	result->y = a->y * b;
}

inline static void vec2DivideScalar(vec2 *result, const vec2 *a, const float b)
{
	if (b != 0)
	{
		result->x = a->x / b;
		result->y = a->y / b;
	}
	else
	{
		vec2Copy(result, a);
	}
}

inline static void vec2Invert(vec2 *a)
{
	a->x *= -1;
	a->y *= -1;
}

inline static float seVec2Length(const seVec2 vector)
{
	return seSqrt(vector[0] * vector[0] + vector[1] * vector[1]);
}

inline static float seVec2LengthSquared(const seVec2 vector)
{
	return (vector[0] * vector[0] + vector[1] * vector[1]);
}

inline static void seVec2Normalize(seVec2 vector)
{
	float length = seVec2Length(vector);

	if (!length)
		return;

	vector[0] /= length;
	vector[1] /= length;
}

inline static void seVec2Limit(seVec2 result, const float maximum)
{
	if (seVec2LengthSquared(result) > maximum * maximum)
	{
		seVec2Normalize(result);
		seVec2MultScalar(result, result, maximum);
	}
}

inline static void seVec2Random(seVec2 result)
{
	result[0] = seRandomNumber(-1, 1);
	result[1] = seRandomNumber(-1, 1);

	seVec2Normalize(result);
}

inline static float seVec2Dot(const seVec2 v1, const seVec2 v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1];
}

inline static float seVec2Cross(const seVec2 v1, const seVec2 v2)
{
	return v1[0] * v2[1] + v1[1] * v2[0];
}

inline static void seVec2Clamp(seVec2 result, const seVec2 vector, const float len)
{
	seVec2Copy(result, vector);
	if (seVec2Dot(vector, vector) > len * len)
	{
		seVec2Normalize(result);
		seVec2MultScalar(result, result, len);
	}
}

inline static float vec2Length(const vec2 *vector)
{
	return seSqrt(vector->x * vector->x + vector->y * vector->y);
}

inline static float vec2LengthSquared(const vec2 *vector)
{
	return (vector->x * vector->x + vector->y * vector->y);
}

inline static void vec2Normalize(vec2 *vector)
{
	float length = vec2Length(vector);

	if (!length)
		return;

	vector->x /= length;
	vector->y /= length;
}

inline static void vec2Limit(vec2 *result, const float maximum)
{
	if (vec2LengthSquared(result) > maximum * maximum)
	{
		vec2Normalize(result);
		vec2MultScalar(result, result, maximum);
	}
}

inline static void vec2Random(vec2 *result)
{
	result->x = seRandomNumber(-1, 1);
	result->y = seRandomNumber(-1, 1);

	vec2Normalize(result);
}

inline static float vec2Dot(const vec2 *v1, const vec2 *v2)
{
	return v1->x * v2->x + v1->y * v2->y;
}

inline static float vec2Cross(const vec2 *v1, const vec2 *v2)
{
	return v1->x * v2->y + v1->y * v2->x;
}

inline static void vec2Clamp(vec2 *result, const vec2 *vector, const float len)
{
	vec2Copy(result, vector);
	if (vec2Dot(vector, vector) > len * len)
	{
		vec2Normalize(result);
		vec2MultScalar(result, result, len);
	}
}

//////////////////////////////////////////////////////////////////////////
///	Vector3D
//////////////////////////////////////////////////////////////////////////

inline static vec3 *vec3Allocate(size_t count)
{
	vec3 *result = (vec3 *)_aligned_malloc(sizeof(vec3) * count, 16);
	if (!result)
	{
		exit(EXIT_FAILURE);
	}
	return result;
}

inline static void vec3Set(vec3 *a, const float x, const float y, const float z)
{
	a->mmvalue = _mm_set_ps(0.0f, z, y, x);
}

inline static void vec3SetZero(vec3 *a)
{
	a->mmvalue = _mm_setzero_ps();
}

inline static void vec3SetOne(vec3 *a)
{
	a->mmvalue = _mm_set1_ps(1.0f);
}

inline static vec3 *newVec3()
{
	vec3 *result = vec3Allocate(1);
	vec3SetZero(result);
	return result;
}

inline static vec3 *newVec3At(const float x, const float y, const float z)
{
	vec3 *result = vec3Allocate(1);
	vec3Set(result, x, y, z);
	return result;
}

inline static void freeVec3(vec3 *vec)
{
	if (!vec)
		return;
	_aligned_free(vec);
}

inline static void vec3Copy(vec3 *a, const vec3 *b)
{
	*a = *b;
}

inline static void vec3Print(const vec3 *a)
{
	printf("%.2f, %.2f, %.2f\n", a->x, a->y, a->z);
}

inline static void vec3Add(vec3 *result, const vec3 *a, const vec3 *b)
{
	result->mmvalue = _mm_add_ps(a->mmvalue, b->mmvalue);
}

inline static void vec3Subtract(vec3 *result, const vec3 *a, const vec3 *b)
{
	result->mmvalue = _mm_sub_ps(a->mmvalue, b->mmvalue);
}

inline static void vec3MultScalar(vec3 *result, const vec3 *a, const float b)
{
	result->mmvalue = _mm_mul_ps(a->mmvalue, _mm_set1_ps(b));
}

inline static void vec3Multiply(vec3 *result, const vec3 *a, const vec3 *b)
{
	result->mmvalue = _mm_mul_ps(a->mmvalue, b->mmvalue);
}

inline static void vec3DivideScalar(vec3 *result, const vec3 *a, const float b)
{
	result->mmvalue = _mm_div_ps(a->mmvalue, _mm_set1_ps(b));
}

inline static void vec3Divide(vec3 *result, const vec3 *a, const vec3 *b)
{
	result->mmvalue = _mm_div_ps(a->mmvalue, b->mmvalue);
}

inline static void vec3Invert(vec3 *a)
{
	vec3MultScalar(a, a, -1.0f);
}

inline static void GetIntersectionXY(vec3 *result, const vec3 *a, const vec3 *b)
{
	const float r = -a->z / b->z;
	result->mmvalue = _mm_set_ps(0.0f, 0.0f, (r * b->y + a->y) / (r + 1), (r * b->x + a->x) / (r + 1));
}

inline static float vec3Dot(const vec3 *a, const vec3 *b)
{
	return _mm_cvtss_f32(_mm_dp_ps(a->mmvalue, b->mmvalue, 0x71));
}

inline static float vec3Length(const vec3 *a)
{
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(a->mmvalue, a->mmvalue, 0x71)));
}

inline static float vec3LengthSquared(const vec3 *a)
{
	return _mm_cvtss_f32(_mm_dp_ps(a->mmvalue, a->mmvalue, 0x71));
}

inline static float vec3Distance(const vec3 *vector0, const vec3 *vector1)
{
	vec3 tmp;
	vec3Subtract(&tmp, vector1, vector0);
	return vec3Length(&tmp);
}

inline static float vec3DistanceSquared(const vec3 *vector0, const vec3 *vector1)
{
	vec3 tmp;
	vec3Subtract(&tmp, vector1, vector0);
	return vec3LengthSquared(&tmp);
}

inline static void vec3Normalize(vec3 *vector)
{
	vector->mmvalue = _mm_mul_ps(vector->mmvalue, _mm_rsqrt_ps(_mm_dp_ps(vector->mmvalue, vector->mmvalue, 0x7F)));
}

inline static void vec3Cross(vec3 *result, const vec3 *a, const vec3 *b)
{
	result->mmvalue = _mm_sub_ps(_mm_mul_ps(_mm_shuffle_ps(a->mmvalue, a->mmvalue, _MM_SHUFFLE(3, 0, 2, 1)), _mm_shuffle_ps(b->mmvalue, b->mmvalue, _MM_SHUFFLE(3, 1, 0, 2))), _mm_mul_ps(_mm_shuffle_ps(a->mmvalue, a->mmvalue, _MM_SHUFFLE(3, 1, 0, 2)), _mm_shuffle_ps(b->mmvalue, b->mmvalue, _MM_SHUFFLE(3, 0, 2, 1))));
}

inline static void vec3MultiplyMat3(vec3 *result, const mat3 matrix, const vec3 *vector)
{
	int i, j, pos = 0;
	for (i = 0; i < 3; i++)
	{
		result->data[i] = 0;
		for (j = 0; j < 3; j++)
		{
			result->data[i] += vector->data[i] * matrix[pos];
			pos++;
		}
	}
}

inline static void vec3MultiplyMat4(vec3 *result, const mat4 *matrix, const vec3 *vector)
{
	__m128 prod1 = _mm_mul_ps(matrix->rows[0], vector->mmvalue);
	__m128 prod2 = _mm_mul_ps(matrix->rows[1], vector->mmvalue);
	__m128 prod3 = _mm_mul_ps(matrix->rows[2], vector->mmvalue);
	__m128 prod4 = _mm_mul_ps(matrix->rows[3], vector->mmvalue);

	result->mmvalue = _mm_hadd_ps(_mm_hadd_ps(prod1, prod2), _mm_hadd_ps(prod3, prod4));

	//////////////////////////////////////////////////////////////////////////
	///	SSE4
	//////////////////////////////////////////////////////////////////////////
	/*
	__m128 prod1 = _mm_dp_ps(matrix->rows[0], vector->mmvalue, 0xFF);
	__m128 prod2 = _mm_dp_ps(matrix->rows[1], vector->mmvalue, 0xFF);
	__m128 prod3 = _mm_dp_ps(matrix->rows[2], vector->mmvalue, 0xFF);
	__m128 prod4 = _mm_dp_ps(matrix->rows[3], vector->mmvalue, 0xFF);

	result->mmvalue = _mm_shuffle_ps(_mm_movelh_ps(prod1, prod2), _mm_movelh_ps(prod3, prod4),  _MM_SHUFFLE(2, 0, 2, 0));
	*/
}

inline static bool vec3Compare(vec3 *a, vec3 *b)
{
	return (a->x == b->x && a->y == b->y && a->z == b->z) ? true : false;
}

//////////////////////////////////////////////////////////////////////////
///	Quaternion
//////////////////////////////////////////////////////////////////////////

inline static void quatMultiplyMat4(seQuat result, const mat4 *matrix, const seQuat pvector)
{
	/*//__m128 vec = _mm_set_ps(pvector[3], pvector[2], pvector[1], pvector[0]);// _mm_load_ps(&pvector[0]);
	__m128 vec = _mm_load_ps(&pvector[0]);
	
	__m128 prod1 = _mm_dp_ps(matrix->rows[0], vec, 0xFF);
	__m128 prod2 = _mm_dp_ps(matrix->rows[1], vec, 0xFF);
	__m128 prod3 = _mm_dp_ps(matrix->rows[2], vec, 0xFF);
	__m128 prod4 = _mm_dp_ps(matrix->rows[3], vec, 0xFF);

	__m128 mmvalue = _mm_shuffle_ps(_mm_movelh_ps(prod1, prod2), _mm_movelh_ps(prod3, prod4),  _MM_SHUFFLE(2, 0, 2, 0));

	_mm_store_ps(result, mmvalue);

	printf("---------------\n");

	seVec4Print(result);
*/

	const float *mat = matrix->data;
	result[0] = mat[0] * pvector[0] + mat[4] * pvector[1] + mat[8] * pvector[2] + mat[12] * pvector[3];
	result[1] = mat[1] * pvector[0] + mat[5] * pvector[1] + mat[9] * pvector[2] + mat[13] * pvector[3];
	result[2] = mat[2] * pvector[0] + mat[6] * pvector[1] + mat[10] * pvector[2] + mat[14] * pvector[3];
	result[3] = mat[3] * pvector[0] + mat[7] * pvector[1] + mat[11] * pvector[2] + mat[15] * pvector[3];
}

inline static void quatMultiply(seQuat result, const seQuat quaternion0, const seQuat quaternion1)
{
	result[0] = quaternion0[3] * quaternion1[0] + quaternion0[0] * quaternion1[3] + quaternion0[1] * quaternion1[2] - quaternion0[2] * quaternion1[1];
	result[1] = quaternion0[3] * quaternion1[1] - quaternion0[0] * quaternion1[2] + quaternion0[1] * quaternion1[3] + quaternion0[2] * quaternion1[0];
	result[2] = quaternion0[3] * quaternion1[2] + quaternion0[0] * quaternion1[1] - quaternion0[1] * quaternion1[0] + quaternion0[2] * quaternion1[3];
	result[3] = quaternion0[3] * quaternion1[3] - quaternion0[0] * quaternion1[0] - quaternion0[1] * quaternion1[1] - quaternion0[2] * quaternion1[2];
}

inline static void quatConjugate(seQuat quaternion)
{
	quaternion[0] = -quaternion[0];
	quaternion[1] = -quaternion[1];
	quaternion[2] = -quaternion[2];
}

inline static void quatInverseUnit(seQuat quaternion)
{
	quatConjugate(quaternion);
}

inline static void quatRotate(seQuat quaternion, const float angle, const vec3 *axis)
{
	float halfAngleRadian = DegToRad(angle) * 0.5f;
	float s = seSin(halfAngleRadian);
	vec3 normalized;

	vec3Copy(&normalized, axis);
	vec3Normalize(&normalized);

	quaternion[0] = normalized.x * s;
	quaternion[1] = normalized.y * s;
	quaternion[2] = normalized.z * s;
	quaternion[3] = seCos(halfAngleRadian);
}

inline static void quatRotateRx(seQuat quaternion, const float angle)
{
	float halfAngleRadian = DegToRad(angle) * 0.5f;

	quaternion[0] = seSin(halfAngleRadian);
	quaternion[1] = 0.0f;
	quaternion[2] = 0.0f;
	quaternion[3] = seCos(halfAngleRadian);
}

inline static void quatRotateRy(seQuat quaternion, const float angle)
{
	float halfAngleRadian = DegToRad(angle) * 0.5f;

	quaternion[0] = 0.0f;
	quaternion[1] = seSin(halfAngleRadian);
	quaternion[2] = 0.0f;
	quaternion[3] = seCos(halfAngleRadian);
}

inline static void quatRotateRz(seQuat quaternion, const float angle)
{
	float halfAngleRadian = DegToRad(angle) * 0.5f;

	quaternion[0] = 0.0f;
	quaternion[1] = 0.0f;
	quaternion[2] = seSin(halfAngleRadian);
	quaternion[3] = seCos(halfAngleRadian);
}

inline static void quatRotateRzRxRy(seQuat quaternion, const float anglez, const float anglex, const float angley)
{
	seQuat rotZ;
	seQuat rotX;
	seQuat rotY;

	quatRotateRz(rotZ, anglez);
	quatRotateRx(rotX, anglex);
	quatRotateRy(rotY, angley);

	quatMultiply(quaternion, rotZ, rotX);
	quatMultiply(quaternion, quaternion, rotY);
}

inline static void quatGetMat4(mat4 *matrix, const seQuat quaternion)
{
	float x = quaternion[0];
	float y = quaternion[1];
	float z = quaternion[2];
	float w = quaternion[3];

	matrix->rows[0] = _mm_set_ps(0.0, 2.0 * x * z - 2.0 * w * y, 2.0 * x * y + 2.0 * w * z, 1.0 - 2.0 * y * y - 2.0 * z * z);
	matrix->rows[1] = _mm_set_ps(0.0, 2.0 * y * z + 2.0 * w * x, 1.0 - 2.0 * x * x - 2.0 * z * z, 2.0 * x * y - 2.0 * w * z);
	matrix->rows[2] = _mm_set_ps(0.0, 1.0f - 2.0f * x * x - 2.0f * y * y, 2.0f * y * z - 2.0f * w * x, 2.0 * x * z + 2.0f * w * y);
	matrix->rows[3] = _mm_set_ps(0.1f, 0.0f, 0.0f, 0.0f);
}

inline static void quatGetMat3(mat3 matrix, const seQuat quaternion)
{
	float x = quaternion[0];
	float y = quaternion[1];
	float z = quaternion[2];
	float w = quaternion[3];

	matrix[0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
	matrix[1] = 2.0f * x * y + 2.0f * w * z;
	matrix[2] = 2.0f * x * z - 2.0f * w * y;

	matrix[3] = 2.0f * x * y - 2.0f * w * z;
	matrix[4] = 1.0f - 2.0f * x * x - 2.0f * z * z;
	matrix[5] = 2.0f * y * z + 2.0f * w * x;

	matrix[6] = 2.0f * x * z + 2.0f * w * y;
	matrix[7] = 2.0f * y * z - 2.0f * w * x;
	matrix[8] = 1.0f - 2.0f * x * x - 2.0f * y * y;
}

inline static void quatGetEulerRzRxRy(vec3 *angles, const seQuat quaternion)
{
	float x = quaternion[0];
	float y = quaternion[1];
	float z = quaternion[2];
	float w = quaternion[3];

	float f11 = 1.0f - 2.0f * x * x - 2.0f * z * z;
	float f21 = 2.0f * y * z + 2.0f * w * x;

	if (f11 != 0.0f)
	{
		float f20 = 2.0f * x * z - 2.0f * w * y;
		float f22 = 1.0f - 2.0f * x * x - 2.0f * y * y;
		float f01 = 2.0f * x * y - 2.0f * w * z;
		vec3Set(angles, RADS_IN_DEG * seAsin(f21), RADS_IN_DEG * atan2f(-f20, f22), RADS_IN_DEG * atan2f(-f01, f11));
	}
	else
	{
		float f10 = 2.0f * x * y + 2.0f * w * z;
		float f00 = 1.0f - 2.0f * y * y - 2.0f * z * z;
		vec3Set(angles, RADS_IN_DEG * seAsin(f21), 0.0f, RADS_IN_DEG * atan2f(f10, f00));
	}
}

//inline static void quatSlerp(seQuat result, const vec3 *a, const vec3 *b, const float t);

inline static void quatToAngleAxis(float *angle, vec3 *axis, seQuat quaternion)
{
	const float x = quaternion[0];
	const float y = quaternion[1];
	const float z = quaternion[2];
	const float w = quaternion[3];

	const float squareLength = x * x + y * y + z * z;

	if (squareLength > EPSILON_SQUARED)
	{
		*angle = 2.0f * (float)acos(w);
		const float inverseLength = 1.0 / sePow(squareLength, 0.5);
		vec3Set(axis, x * inverseLength, y * inverseLength, z * inverseLength);
	}
	else
	{
		*angle = 0.0;
		vec3Set(axis, 1.0, 0, 0);
	}
}

inline static float quatLength(const seQuat quaternion)
{
	__m128 mvalue = _mm_set_ps(quaternion[3], quaternion[2], quaternion[1], quaternion[0]);
	return _mm_cvtss_f32(_mm_sqrt_ss(_mm_dp_ps(mvalue, mvalue, 0x71)));
}

inline static bool quatInverse(seQuat quaternion)
{
	float norm = quatLength(quaternion);

	if (norm == 0.0f)
	{
		return false;
	}

	quatConjugate(quaternion);
	quaternion[0] /= (norm * norm);
	quaternion[1] /= (norm * norm);
	quaternion[2] /= (norm * norm);
	quaternion[3] /= (norm * norm);

	return true;
}

inline static bool quatNormalize(seQuat quaternion)
{
	const float length = quatLength(quaternion);

	if (length == 0.0f)
	{
		seQuatSetIdentity(quaternion);
	}
	else
	{
		quaternion[0] /= length;
		quaternion[1] /= length;
		quaternion[2] /= length;
		quaternion[3] /= length;
	}

	return true;
}

inline static bool quatGetPoint4(seQuat point, const seQuat quaternion)
{
	if (quaternion[3] != 0.0f)
		return false;

	point[0] = quaternion[0];
	point[1] = quaternion[1];
	point[2] = quaternion[2];
	point[3] = 1.0f;

	return true;
}

inline static bool quatGetVec3(vec3 *vector, const seQuat quaternion)
{
	if (quaternion[3] != 0.0f)
		return false;

	vector->x = quaternion[0];
	vector->y = quaternion[1];
	vector->z = quaternion[2];

	return false;
}

//////////////////////////////////////////////////////////////////////////
///	Matrix
//////////////////////////////////////////////////////////////////////////

inline static mat4 *mat4Allocate(size_t count)
{
	mat4 *result = (mat4 *)_aligned_malloc(sizeof(mat4) * count, 16);
	if (!result)
	{
		exit(EXIT_FAILURE);
	}
	return result;
}

inline static mat4 *newMat4()
{
	mat4 *result = mat4Allocate(1);
	if (!result)
	{
		exit(EXIT_FAILURE);
	}
	return result;
}

inline static void mat4Identity(mat4 *matrix)
{
	matrix->rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
	matrix->rows[1] = _mm_set_ps(0.0f, 0.0f, 1.0f, 0.0f);
	matrix->rows[2] = _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f);
	matrix->rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
}

inline static mat4 *newMat4Identity()
{
	mat4 *result = mat4Allocate(1);
	if (!result)
	{
		exit(EXIT_FAILURE);
	}
	mat4Identity(result);

	return result;
}

inline static void freeMat4(mat4 *mat)
{
	if (!mat)
		return;
	_aligned_free(mat);
}

inline static void mat4InverseSSE(float *result, const float *src)
{
	__m128 minor0 = _mm_setzero_ps();
	__m128 minor1 = _mm_setzero_ps();
	__m128 minor2 = _mm_setzero_ps();
	__m128 minor3 = _mm_setzero_ps();

	__m128 row0 = _mm_setzero_ps();
	__m128 row1 = _mm_setzero_ps();
	__m128 row2 = _mm_setzero_ps();
	__m128 row3 = _mm_setzero_ps();

	__m128 det = _mm_setzero_ps();

	__m128 tmp1 = _mm_setzero_ps();

	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64 *)(src)), (__m64 *)(src + 4));
	row1 = _mm_loadh_pi(_mm_loadl_pi(row1, (__m64 *)(src + 8)), (__m64 *)(src + 12));
	row0 = _mm_shuffle_ps(tmp1, row1, 0x88);
	row1 = _mm_shuffle_ps(row1, tmp1, 0xDD);
	tmp1 = _mm_loadh_pi(_mm_loadl_pi(tmp1, (__m64 *)(src + 2)), (__m64 *)(src + 6));
	row3 = _mm_loadh_pi(_mm_loadl_pi(row3, (__m64 *)(src + 10)), (__m64 *)(src + 14));
	row2 = _mm_shuffle_ps(tmp1, row3, 0x88);
	row3 = _mm_shuffle_ps(row3, tmp1, 0xDD);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row2, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_mul_ps(row1, tmp1);
	minor1 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(_mm_mul_ps(row1, tmp1), minor0);
	minor1 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor1);
	minor1 = _mm_shuffle_ps(minor1, minor1, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row1, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor0 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor0);
	minor3 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor3);
	minor3 = _mm_shuffle_ps(minor3, minor3, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(_mm_shuffle_ps(row1, row1, 0x4E), row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	row2 = _mm_shuffle_ps(row2, row2, 0x4E);
	minor0 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor0);
	minor2 = _mm_mul_ps(row0, tmp1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor0 = _mm_sub_ps(minor0, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_sub_ps(_mm_mul_ps(row0, tmp1), minor2);
	minor2 = _mm_shuffle_ps(minor2, minor2, 0x4E);
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row1);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor2 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(_mm_mul_ps(row2, tmp1), minor3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor2 = _mm_sub_ps(_mm_mul_ps(row3, tmp1), minor2);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row2, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row3);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row2, tmp1));
	minor2 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_add_ps(_mm_mul_ps(row2, tmp1), minor1);
	minor2 = _mm_sub_ps(minor2, _mm_mul_ps(row1, tmp1));
	// -----------------------------------------------
	tmp1 = _mm_mul_ps(row0, row2);
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0xB1);
	minor1 = _mm_add_ps(_mm_mul_ps(row3, tmp1), minor1);
	minor3 = _mm_sub_ps(minor3, _mm_mul_ps(row1, tmp1));
	tmp1 = _mm_shuffle_ps(tmp1, tmp1, 0x4E);
	minor1 = _mm_sub_ps(minor1, _mm_mul_ps(row3, tmp1));
	minor3 = _mm_add_ps(_mm_mul_ps(row1, tmp1), minor3);
	// -----------------------------------------------
	det = _mm_mul_ps(row0, minor0);
	det = _mm_add_ps(_mm_shuffle_ps(det, det, 0x4E), det);
	det = _mm_add_ss(_mm_shuffle_ps(det, det, 0xB1), det);
	tmp1 = _mm_rcp_ss(det);
	det = _mm_sub_ss(_mm_add_ss(tmp1, tmp1), _mm_mul_ss(det, _mm_mul_ss(tmp1, tmp1)));
	det = _mm_shuffle_ps(det, det, 0x00);
	minor0 = _mm_mul_ps(det, minor0);
	_mm_storel_pi((__m64 *)(result), minor0);
	_mm_storeh_pi((__m64 *)(result + 2), minor0);
	minor1 = _mm_mul_ps(det, minor1);
	_mm_storel_pi((__m64 *)(result + 4), minor1);
	_mm_storeh_pi((__m64 *)(result + 6), minor1);
	minor2 = _mm_mul_ps(det, minor2);
	_mm_storel_pi((__m64 *)(result + 8), minor2);
	_mm_storeh_pi((__m64 *)(result + 10), minor2);
	minor3 = _mm_mul_ps(det, minor3);
	_mm_storel_pi((__m64 *)(result + 12), minor3);
	_mm_storeh_pi((__m64 *)(result + 14), minor3);
}

inline static void mat4Invert(float *src)
{
	mat4InverseSSE(src, src);
}

inline static void mat4TransposeSSE(mat4 *result)
{
	_MM_TRANSPOSE4_PS(result->rows[0], result->rows[1], result->rows[2], result->rows[3]);
}

inline static void mat4ExtractMat3(mat3 matrix, const float *source)
{
	matrix[0] = source[0];
	matrix[1] = source[1];
	matrix[2] = source[2];
	matrix[3] = source[4];
	matrix[4] = source[5];
	matrix[5] = source[6];
	matrix[6] = source[8];
	matrix[7] = source[9];
	matrix[8] = source[10];
}

inline static void mat4MultiplyAsm(float *result, const float *matrix0, const float *matrix1)
{
	__asm mov edx, DWORD PTR matrix1 __asm mov ecx, DWORD PTR matrix0 __asm mov eax, DWORD PTR result e4(0, 0, 4, 4, 4, 0, 0) e4(0, 1, 4, 4, 4, 0, 0) e4(0, 2, 4, 4, 4, 0, 0) e4(0, 3, 4, 4, 4, 0, 0) e4(1, 0, 4, 4, 4, 0, 0) e4(1, 1, 4, 4, 4, 0, 0) e4(1, 2, 4, 4, 4, 0, 0) e4(1, 3, 4, 4, 4, 0, 0) e4(2, 0, 4, 4, 4, 0, 0) e4(2, 1, 4, 4, 4, 0, 0) e4(2, 2, 4, 4, 4, 0, 0) e4(2, 3, 4, 4, 4, 0, 0) e4(3, 0, 4, 4, 4, 0, 0) e4(3, 1, 4, 4, 4, 0, 0) e4(3, 2, 4, 4, 4, 0, 0) e4(3, 3, 4, 4, 4, 0, 0)
}

inline static void mat4Translate(float *matrix, const float x, const float y, const float z)
{
	mat4 temp;

	mat4Identity(&temp);

	temp.data[12] = x;
	temp.data[13] = y;
	temp.data[14] = z;

	mat4MultiplyAsm(matrix, matrix, temp.data);
}

inline static void mat4TranslateVec(mat4 *matrix, const vec3 *delta)
{
	mat4Translate(matrix->data, delta->x, delta->y, delta->z);
}

inline static void mat4RotateX(mat4 *matrix, const float rot)
{
	float rad = DegToRad(rot);
	float c = seCos(rad);
	float s = seSin(rad);
	matrix->rows[0] = _mm_set_ps(0.0f, 0.0f, 0.0f, 1.0f);
	matrix->rows[1] = _mm_set_ps(0.0f, -s, c, 0.0f);
	matrix->rows[2] = _mm_set_ps(0.0f, c, s, 0.0f);
	matrix->rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);
}

inline static void mat4Rotate(float *matrix, const float angle, const vec3 *axis)
{
	mat4 temp;
	vec3 vector;

	float rad = DegToRad(angle);

	float s = seSin(rad);
	float c = seCos(rad);

	vec3Copy(&vector, axis);
	vec3Normalize(&vector);

	temp.rows[0] = _mm_set_ps(0.0f, vector.x * vector.z * (1 - c) - vector.y * s, vector.x * vector.y * (1 - c) + vector.z * s, vector.x * vector.x * (1 - c) + c);
	temp.rows[1] = _mm_set_ps(0.0f, vector.y * vector.z * (1 - c) + vector.x * s, vector.y * vector.y * (1 - c) + c, vector.x * vector.y * (1 - c) - vector.z * s);
	temp.rows[2] = _mm_set_ps(0.0f, vector.z * vector.z * (1 - c) + c, vector.y * vector.z * (1 - c) - vector.x * s, vector.x * vector.z * (1 - c) + vector.y * s);
	temp.rows[3] = _mm_set_ps(1.0f, 0.0f, 0.0f, 0.0f);

	mat4MultiplyAsm(matrix, matrix, temp.data);
}

inline static void mat4MultiplyScalar(mat4 *result, const mat4 *matrix, const float scalar)
{
	__m128 scaleVec = _mm_load1_ps(&scalar);
	result->rows[0] = _mm_mul_ps(matrix->rows[0], scaleVec);
	result->rows[1] = _mm_mul_ps(matrix->rows[1], scaleVec);
	result->rows[2] = _mm_mul_ps(matrix->rows[2], scaleVec);
	result->rows[3] = _mm_mul_ps(matrix->rows[3], scaleVec);
}

inline static void mat4Add(mat4 *result, const mat4 *mat1, const mat4 *mat2)
{
	result->rows[0] = _mm_add_ps(mat1->rows[0], mat2->rows[0]);
	result->rows[1] = _mm_add_ps(mat1->rows[1], mat2->rows[1]);
	result->rows[2] = _mm_add_ps(mat1->rows[2], mat2->rows[2]);
	result->rows[3] = _mm_add_ps(mat1->rows[3], mat2->rows[3]);
}

inline static void mat4Scale(float *result, const float scale)
{
	result[0 + 0] *= scale;
	result[4 + 1] *= scale;
	result[8 + 2] *= scale;
}

//////////////////////////////////////////////////////////////////////////
///	Matrix 3x3
//////////////////////////////////////////////////////////////////////////

inline static void mat3Copy(mat3 matrix, const mat3 source, const bool rotationOnly)
{
	int i;

	if (rotationOnly)
	{
		for (i = 0; i < 6; i++)
		{
			if ((i + 1) % 3 == 0)
			{
				continue;
			}

			matrix[i] = source[i];
		}

		matrix[2] = 0.0f;
		matrix[5] = 0.0f;

		matrix[6] = 0.0f;
		matrix[7] = 0.0f;
		matrix[8] = 1.0f;
	}
	else
	{
		*matrix = *source;
	}
}

inline static void mat3Identity(mat3 matrix)
{
	matrix[0] = 1.0f;
	matrix[1] = 0.0f;
	matrix[2] = 0.0f;
	matrix[3] = 0.0f;
	matrix[4] = 1.0f;
	matrix[5] = 0.0f;
	matrix[6] = 0.0f;
	matrix[7] = 0.0f;
	matrix[8] = 1.0f;
}

inline static void mat3Inverse(mat3 matrix)
{
	int index[4];

	float det;
	mat3 temp;
	float sign;

	det = matrix[0] * matrix[4] * matrix[8] + matrix[3] * matrix[7] * matrix[2] + matrix[6] * matrix[1] * matrix[5] - matrix[6] * matrix[4] * matrix[2] - matrix[3] * matrix[1] * matrix[8] - matrix[0] * matrix[7] * matrix[5];

	if (det == 0.0f)
	{
		return;
	}

	mat3Copy(temp, matrix, false);

	omp_set_num_threads(3);
#pragma omp parallel for shared(matrix, temp, index, sign)
	for (int i = 0; i < 9; i++)
	{
		int column = i / 3;
		int row = i % 3;

		int counter = 0;

		for (int j = 0; j < 9; j++)
		{
			if (j / 3 == row || j % 3 == column)
			{
				continue;
			}

			index[counter] = j;

			counter++;
		}

		sign = (column + row) % 2 == 0 ? 1.0f : -1.0f;

		matrix[i] = sign * (temp[index[0]] * temp[index[3]] - temp[index[2]] * temp[index[1]]) / det;
	}
}

inline static void mat3Transpose(mat3 matrix)
{
	mat3 temp;
	mat3Copy(temp, matrix, false);

	omp_set_num_threads(3);
#pragma omp parallel for shared(matrix, temp)
	for (int column = 0; column < 3; column++)
	{
		for (int row = 0; row < 3; row++)
		{
			matrix[row * 3 + column] = temp[column * 3 + row];
		}
	}
}

inline static void mat3MultiplyScalar(mat3 result, mat3 matrix, const float scalar)
{
	omp_set_num_threads(3);
#pragma omp parallel for shared(result)
	for (int i = 0; i < 9; i++)
	{
		result[i] = matrix[i] * scalar;
	}
}

inline static void mat3Add(mat3 result, mat3 mat1, mat3 mat2)
{
	omp_set_num_threads(3);
#pragma omp parallel for shared(result)
	for (int i = 0; i < 9; i++)
	{
		result[i] = mat1[i] + mat2[i];
	}
}

#endif // !MATH_H
