#include "stdafx.h"
#include "AnimMathFunc.h"

void CalcInterpolatedRotation(aiQuaternion& Out, float tick, const aiNodeAnim* pNodeAnim)
{	
	// we need at least two values to interpolate.
	if (pNodeAnim->mNumRotationKeys == 1)
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}
	Out = Interpolate<aiQuatKey>(pNodeAnim->mRotationKeys, pNodeAnim->mNumRotationKeys, tick).mValue;
}

void CalcInterpolatedPosition(aiVector3D& Out, float tick, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate.
	if (pNodeAnim->mNumPositionKeys == 1)
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}
	Out = Interpolate<aiVectorKey>(pNodeAnim->mPositionKeys, pNodeAnim->mNumPositionKeys, tick).mValue;
}

void CalcInterpolatedScaling(aiVector3D& Out, float tick, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate.
	if (pNodeAnim->mNumScalingKeys == 1)
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}
	Out = Interpolate<aiVectorKey>(pNodeAnim->mScalingKeys, pNodeAnim->mNumScalingKeys, tick).mValue;
}


const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; ++i)
	{
		if (strcmp(pAnimation->mChannels[i]->mNodeName.C_Str(), nodeName.c_str()) == 0)
		{
			return pAnimation->mChannels[i];
		}
	}
	return NULL;
}

float Lerp(float a, float b, float t)
{
	return (1.0f - t) * a + b * t;
}

float InvLerp(float a, float b, float v)
{
	return (v - a) / (b - a);
}

float Remap(float inMin, float inMax, float outMin, float outMax, float v)
{
	float t = InvLerp(inMin, inMax, v);
	return Lerp(outMin, outMax, t);
}

void InitM4FromM3(aiMatrix4x4& out, const aiMatrix3x3& in)
{
	out.a1 = in.a1; out.a2 = in.a2; out.a3 = in.a3; out.a4 = 0.f;
	out.b1 = in.b1; out.b2 = in.b2; out.b3 = in.b3; out.b4 = 0.f;
	out.c1 = in.c1; out.c2 = in.c2; out.c3 = in.c3; out.c4 = 0.f;
	out.d1 = 0.f;   out.d2 = 0.f;   out.d3 = 0.f;   out.d4 = 1.f;
}


void InitIdentityM4(aiMatrix4x4& m)
{
	m.a1 = 1.f; m.a2 = 0.f; m.a3 = 0.f; m.a4 = 0.f;
	m.b1 = 0.f; m.b2 = 1.f; m.b3 = 0.f; m.b4 = 0.f;
	m.c1 = 0.f; m.c2 = 0.f; m.c3 = 1.f; m.c4 = 0.f;
	m.d1 = 0.f; m.d2 = 0.f; m.d3 = 0.f; m.d4 = 1.f;
	assert(m.IsIdentity());
}

void MulM4(aiMatrix4x4& out, aiMatrix4x4& in, float m)
{
	out.a1 += in.a1 * m; out.a2 += in.a2 * m; out.a3 += in.a3 * m; out.a4 += in.a4 * m;
	out.b1 += in.b1 * m; out.b2 += in.b2 * m; out.b3 += in.b3 * m; out.b4 += in.b4 * m;
	out.c1 += in.c1 * m; out.c2 += in.c2 * m; out.c3 += in.c3 * m; out.c4 += in.c4 * m;
	out.d1 += in.d1 * m; out.d2 += in.d2 * m; out.d3 += in.d3 * m; out.d4 += in.d4 * m;
}

void ShortMulM4(aiVector3D& out, const aiMatrix4x4& m, const aiVector3D& in)
{
	out.x = m.a1 * in.x + m.a2 * in.y + m.a3 * in.z;
	out.y = m.b1 * in.x + m.b2 * in.y + m.b3 * in.z;
	out.z = m.c1 * in.x + m.c2 * in.y + m.c3 * in.z;
}

long long GetCurrentTimeMillis()
{
#ifdef WIN32    
	return GetTickCount64();
#else
	timeval t;
	gettimeofday(&t, NULL);

	long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
	return ret;
#endif    
}