#pragma once

struct ImColor;
struct SPin;
class CNodeInstance;
class CGraphManager;
#ifdef _DEBUG
#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;
#endif // DEBUG
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

