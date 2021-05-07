#pragma once
#include <imgui_node_editor.h>

class CNodeInstance;
class CGraphManager;

struct ImColor;
struct SPin;

namespace ed = ax::NodeEditor;

class CDrawNodePins
{
public:
	CDrawNodePins() : myGraphManager(nullptr), myHeaderTextureID(nullptr) {}
	~CDrawNodePins() { delete myHeaderTextureID; myHeaderTextureID = nullptr; myGraphManager = nullptr; }
	void DrawNodes();
	void GraphManager(CGraphManager& aGraphManager) { myGraphManager = &aGraphManager; }
	ImColor GetIconColor(unsigned int aType);
	void DrawPinIcon(const SPin& aPin, bool aIsConnected, int anAlpha);
	void DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance);
	ImTextureID HeaderTextureID();

private:
	CGraphManager* myGraphManager;
	ImTextureID myHeaderTextureID;

};

