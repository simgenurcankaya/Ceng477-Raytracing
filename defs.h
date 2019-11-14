#ifndef _DEFS_H_
#define _DEFS_H_
#include "math.h"


class Scene;

/* Structure to hold return value from ray intersection routine.
This should hold information related to the intersection point,
for example, coordinate of the intersection point, surface normal at the intersection point etc.
Think about the variables you will need for this purpose and declare them here inside of this structure. */


/* 3 dimensional vector holding floating point numbers.
Used for both coordinates and color.
Use x, y, z for coordinate computations, and use r, g, b for color computations.
Note that you do not have to use this one if you use any vector computation library like Eigen. */
typedef struct Vector3f
{
	union
	{
		float x;
		float r;
	};
	union
	{
		float y;
		float g;
	};
	union
	{
		float z;
		float b;
	};
} Vector3f;

typedef struct ReturnVal
{
	float t;
	bool isIntersect;
	Vector3f normal;
	int materialIndex;

} ReturnVal;

//extern float intersectEpsilon; 
// extern float shadowEpsilon;

inline float dotProduct(const Vector3f& v1, const Vector3f& v2) {
	float ret = 0;
	ret += (v1.x * v2.x);
	ret += (v1.y * v2.y);
	ret += (v1.z * v2.z);
	return  ret;
}
inline Vector3f crossProduct(const Vector3f& v1, const Vector3f& v2) {
	Vector3f ret;
	ret.x = v1.y * v2.z - v1.z * v2.y;
	ret.y = v1.z * v2.x - v1.x * v2.z;
	ret.z = v1.x * v2.y - v1.y * v2.x;
	return ret;
}

inline Vector3f vectorMult(const float number, const Vector3f& v) {
	Vector3f ret;
	ret.x = v.x * number;
	ret.y = v.y * number;
	ret.z = v.z * number;
	return ret;
}

inline Vector3f vectorSum(const Vector3f& v1, const Vector3f& v2) {
	Vector3f ret;
	ret.x = v1.x + v2.x;
	ret.y = v1.y + v2.y;
	ret.z = v1.z + v2.z;
	return ret;
}

inline Vector3f vectorSubs(const Vector3f& v1, const Vector3f& v2) {
	Vector3f ret;
	ret.x = v1.x - v2.x;
	ret.y = v1.y - v2.y;
	ret.z = v1.z - v2.z;
	return ret;
}

inline Vector3f vectorDiv(const Vector3f& v, float number) {
	Vector3f ret;
	ret.x = v.x / number;
	ret.y = v.y / number;
	ret.z = v.z / number;
	return ret;
}


inline float findTriangleArea(const Vector3f& v1, const Vector3f& v2,const Vector3f& v3) {
	float ret;
	Vector3f dummy = crossProduct(vectorSubs(v3,v1), vectorSubs(v2,v1));
	ret = dummy.x * dummy.x + dummy.y * dummy.y + dummy.z * dummy.z;
	ret = sqrt(ret);
	ret = ret / 2;
	return ret;
}

inline float findDistance(const Vector3f &v1, const Vector3f &v2){
	float ret;
	Vector3f dummy = vectorSubs(v1,v2);
	ret = dummy.x * dummy.x + dummy.y * dummy.y + dummy.z * dummy.z;
	ret = sqrt(ret);
	return ret;

}

inline float distance(const Vector3f& v ){
	float ret = (float) v.x *  v.x + v.y * v.y + v.z * v.z;
	return sqrt(ret);
}

inline float findCosinus(const Vector3f &v1, const Vector3f &v2) {
	float ret;
	float dum1,dum2;
	dum1 = distance(v1);
	dum2 = distance(v2);
	return dotProduct(v1,v2) / (dum1 * dum2);
}


extern Scene* pScene;

#endif