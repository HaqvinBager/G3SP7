#include "assimp/scene.h"
#include "assimp/DefaultLogger.hpp"
#include "WinInclude.h"

typedef unsigned int uint;

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

float Lerp(float a, float b, float t);
float InvLerp(float a, float b, float v);
float Remap(float inMin, float inMax, float outMin, float outMax, float v);


void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);

void InitM4FromM3(aiMatrix4x4& out, const aiMatrix3x3& in);
void InitIdentityM4(aiMatrix4x4& m);
long long GetCurrentTimeMillis();


template<class T>
T Interpolate(const T* keys, const uint size, float tick)
{
	int aStartKey = size - 1;
	int aEndKey = size - 1;

	for (uint i = 1; i < size; ++i)
	{
		if (tick <= static_cast<float>(keys[i].mTime) && tick >= static_cast<float>(keys[i - 1].mTime))
		{
			aStartKey = i - 1;
			aEndKey = i;
			break;
		}
	}

	float timeBetweenKeys = Remap(
		static_cast<float>(keys[aStartKey].mTime),
		static_cast<float>(keys[aEndKey].mTime),
		0.0f,
		1.0f,
		tick);

	T interpolatedKey = {};
	Assimp::Interpolator<T>()(
		interpolatedKey.mValue,
		keys[aStartKey],
		keys[aEndKey],
		timeBetweenKeys);
	//interpolatedKey.mValue.Normalize();
	return interpolatedKey;
}

template<class T>
T CalculateInterpolation(const T* someKeys, const unsigned int aNumKeys, const float tick)
{
	if (aNumKeys == 1)
		return someKeys[0];

	return Interpolate<T>(someKeys, aNumKeys, tick);
}

//uint FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
//uint FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
//uint FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
//void set_float4(float f[4], float a, float b, float c, float d);
//void color4_to_float4(const aiColor4D* c, float f[4]);
//void Color4f(const aiColor4D* color);
//void createAILogger();
//void destroyAILogger();
//void logDebug(const char* logString);
//void logInfo(const std::string& logString);
//void MulM4(aiMatrix4x4& out, aiMatrix4x4& in, float m);
//void ShortMulM4(aiVector3D& out, const aiMatrix4x4& m, const aiVector3D& in);
//std::string getBasePath(const std::string& path);
//aiQuaternion Interpolate(aiQuatKey* keys, const uint size, float tick);
//SKeyTime GetRotationKeys(const float anAnimationTime, const aiNodeAnim* pNodeAnim);
//std::pair<uint, uint> GetTranslationKeys(const float anAnimationTime, const aiNodeAnim* pNodeAnim);
//aiVector3D LerpScaling(float t, const aiNodeAnim* pNodeAnim);