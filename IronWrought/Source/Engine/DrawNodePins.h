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
	
	void GraphManager(CGraphManager& aGraphManager) { myGraphManager = &aGraphManager; }
	
	void DrawNodes();
	
	void DrawPinIcon(const SPin& aPin, bool aIsConnected, int anAlpha);
	void DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance);
	ImColor GetIconColor(unsigned int aType);
	
	ImTextureID HeaderTextureID();

private:
	CGraphManager* myGraphManager;
	ImTextureID myHeaderTextureID;

};

