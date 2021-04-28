#include "stdafx.h"
#include "DrawGraphManager.h"
#include "NodeInstance.h"
#include <imgui_node_editor.h>
#include "Drawing.h"
#include "Widgets.h"
#include "NodeType.h"
#include "GraphManager.h"

namespace ed = ax::NodeEditor;
using namespace ax::Drawing;

ImColor CDrawGraphManager::GetIconColor(unsigned int aType)
{
	SPin::EPinType type = static_cast<SPin::EPinType>(aType);
	switch (type)
	{
	default:
	case SPin::EPinType::EFlow:
		return ImColor(255, 255, 255);
	case SPin::EPinType::EBool:
		return ImColor(220, 48, 48);
	case SPin::EPinType::EInt:
		return ImColor(68, 201, 156);
	case SPin::EPinType::EFloat:
		return ImColor(147, 226, 74);
	case SPin::EPinType::EString:
		return ImColor(124, 21, 153);
	case SPin::EPinType::EVector3:
		return ImColor(255, 166, 0);
	case SPin::EPinType::EStringListIndexed:
		return ImColor(0, 255, 0);
	case SPin::EPinType::EUnknown:
		return ImColor(255, 0, 0);
	}
};

void CDrawGraphManager::DrawPinIcon(const SPin& pin, bool connected, int alpha)
{
	IconType iconType;
	ImColor  color = GetIconColor(static_cast<unsigned int>(pin.myVariableType));
	color.Value.w = alpha / 255.0f;
	switch (pin.myVariableType)
	{
	case SPin::EPinType::EFlow:
	{
		iconType = IconType::Flow;
	}
	break;
	case SPin::EPinType::EBool:
	{
		iconType = IconType::Circle;
	}
	break;
	case SPin::EPinType::EInt:
	{
		iconType = IconType::Circle;
	}
	break;
	case SPin::EPinType::EFloat:
	{
		iconType = IconType::Circle;
	}
	break;
	case SPin::EPinType::EString:
	{
		iconType = IconType::Circle;
	}
	break;
	case SPin::EPinType::EVector3:
	{
		iconType = IconType::Circle;
	}
	break;
	case SPin::EPinType::EStringListIndexed:
	{
		iconType = IconType::Circle;
	}
	break;
	case SPin::EPinType::EUnknown:
	{
		iconType = IconType::Circle;
	}
	break;
	default:
		return;
	}
	const int s_PinIconSize = 24;
	ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
};

void CDrawGraphManager::DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance)
{
	switch (aPin.myVariableType)
	{
	case SPin::EPinType::EString:
	{
		if (!aPin.myData)
		{
			aPin.myData = new char[128];
			static_cast<char*>(aPin.myData)[0] = '\0';
		}

		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
			DrawPinIcon(aPin, true, 255);
		else
			if (!myGraphManager->RunScripts())
				ImGui::InputText("##edit", (char*)aPin.myData, 127);

		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EInt:
	{
		if (!aPin.myData)
		{
			aPin.myData = new int;
			int* c = ((int*)aPin.myData);
			*c = 0;
		}
		int* c = ((int*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
			DrawPinIcon(aPin, true, 255);
		else
			if (!myGraphManager->RunScripts())
				ImGui::InputInt("##edit", c);

		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EBool:
	{
		if (!aPin.myData)
		{
			aPin.myData = new bool;
			bool* c = ((bool*)aPin.myData);
			*c = false;
		}
		bool* c = ((bool*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(100.0f);
		if (aNodeInstance->IsPinConnected(aPin))
			DrawPinIcon(aPin, true, 255);
		else
			if (!myGraphManager->RunScripts())
				ImGui::Checkbox("##edit", c);

		ImGui::PopItemWidth();

		ImGui::PopID();
		break;
	}
	case SPin::EPinType::EFloat:
	{
		if (!aPin.myData)
		{
			aPin.myData = new float;
			float* c = ((float*)aPin.myData);
			*c = 1.0f;
		}
		float* c = ((float*)aPin.myData);
		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(70.0f);
		if (aNodeInstance->IsPinConnected(aPin))
			DrawPinIcon(aPin, true, 255);
		else
			if (!myGraphManager->RunScripts())
				ImGui::InputFloat("##edit", c);

		ImGui::PopItemWidth();

		ImGui::PopID();
	}
	break;
	case SPin::EPinType::EVector3:
	{
		if (!aPin.myData)
			aPin.myData = new DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

		DirectX::SimpleMath::Vector3* c;
		c = static_cast<DirectX::SimpleMath::Vector3*>(aPin.myData);

		ImGui::PushID(aPin.myUID.AsInt());
		ImGui::PushItemWidth(35.0f);
		if (aNodeInstance->IsPinConnected(aPin))
			DrawPinIcon(aPin, true, 255);
		else
		{
			if (!myGraphManager->RunScripts())
			{
				ImGui::InputFloat("##edit", &c->x);
				ImGui::SameLine();
				ImGui::InputFloat("##edit1", &c->y);
				ImGui::SameLine();
				ImGui::InputFloat("##edit2", &c->z);
			}
		}
		ImGui::PopItemWidth();

		ImGui::PopID();
	}
	break;
	case SPin::EPinType::EUnknown:
	{
		if (!myGraphManager->RunScripts())
		{
			ImGui::PushID(aPin.myUID.AsInt());
			ImGui::PushItemWidth(100.0f);

			int selectedIndex = -1;
			if (ImGui::RadioButton("Bool", false))
				selectedIndex = (int)SPin::EPinType::EBool;
			if (ImGui::RadioButton("Int", false))
				selectedIndex = (int)SPin::EPinType::EInt;
			if (ImGui::RadioButton("Float", false))
				selectedIndex = (int)SPin::EPinType::EFloat;
			if (ImGui::RadioButton("Vector3", false))
				selectedIndex = (int)SPin::EPinType::EVector3;
			if (ImGui::RadioButton("String", false))
				selectedIndex = (int)SPin::EPinType::EString;

			if (selectedIndex != -1)
			{
				CNodeInstance* instance = myGraphManager->GetNodeFromPinID(aPin.myUID.AsInt());
				instance->ChangePinTypes((SPin::EPinType)selectedIndex);
			}

			ImGui::PopItemWidth();
			ImGui::PopID();
		}
		break;
	}
	default:
		assert(0);
	}
}