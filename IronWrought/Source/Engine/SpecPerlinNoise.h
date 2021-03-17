#pragma once
class CSpecPerlinOctave;

class CSpecPerlinNoise
{
public:
	CSpecPerlinNoise(std::vector<float> someWeights);
	float Noise(float aPositionX, float aPositionZ);

private:
	std::vector<CSpecPerlinOctave> myOctaves;
	std::vector<float> myWeights;
};

