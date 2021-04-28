#pragma once

struct ImColor;
struct SPin;
class CNodeInstance;
class CGraphManager;

class CDrawGraphManager
{
public:
	CDrawGraphManager() : myGraphManager(nullptr) {}
	~CDrawGraphManager() {}
	void GraphManager(CGraphManager& aGraphManager) { myGraphManager = &aGraphManager; }
	ImColor GetIconColor(unsigned int aType);
	void DrawPinIcon(const SPin& aPin, bool aIsConnected, int anAlpha);
	void DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance);

private:
	CGraphManager* myGraphManager;
};

