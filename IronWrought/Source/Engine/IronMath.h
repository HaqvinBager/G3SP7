#pragma once
class IronMath {
public:

	static float Lerp(float a, float b, float t)
	{
		return a + t * (b - a);
	}
	static float InverseLerp(float a, float b, float t)
	{
		return (t - a) / (b - a);
	}

	static float Remap(const float iMin, const float iMax, const float oMin, const float oMax, const float v)
	{
		float t = InverseLerp(iMin, iMax, v);
		return Lerp(oMin, oMax, t);
	}
	static const float CalculateInterpolator(const std::vector<Vector2>& somePoints, const float t)
	{
		unsigned int pointIndex = static_cast<unsigned int>(somePoints.size() - 1);
		for (unsigned int j = 1; j < somePoints.size() - 1; ++j)
		{
			if (t < somePoints[j].x &&
				t > somePoints[j - 1].x)
			{
				pointIndex = j;
				break;
			}
		}

		float val = Remap(somePoints[pointIndex - 1].x, somePoints[pointIndex].x, 0.0f, 1.0f, t);
		return Lerp(somePoints[pointIndex - 1].y, somePoints[pointIndex].y, val);
	}
};
