#pragma once

class CDecal
{
public:
	struct SDecalData {
		std::array<ID3D11ShaderResourceView*, 3> myMaterial {};
		std::string myMaterialName = "";
		float myAlphaClipThreshold = 0.0f;
	};

public:
	CDecal();
	~CDecal();

	void Init(SDecalData data);
	SDecalData& GetDecalData();

private:
	SDecalData myDecalData;
};

