#include "stdafx.h"
#include "DrawNodePins.h"
#include "NodeInstance.h"
#include <imgui_node_editor.h>
#include "Drawing.h"
#include "Widgets.h"
#include "NodeType.h"
#include "GraphManager.h"
#include "Interop.h"
#include <imgui_impl_dx11.h>

namespace ed = ax::NodeEditor;
using namespace ax::Drawing;

void CDrawNodePins::DrawNodes()
{
	for (auto& nodeInstance : myGraphManager->CurrentGraph().myNodeInstances)
	{
		if (!nodeInstance->myHasSetEditorPosition)
		{
			ed::SetNodePosition(nodeInstance->myUID.AsInt(), ImVec2(nodeInstance->myEditorPosition[0], nodeInstance->myEditorPosition[1]));
			nodeInstance->myHasSetEditorPosition = true;
		}

		ed::PushStyleVar(ed::StyleVar_NodePadding, ImVec4(8, 4, 8, 8));
		ed::BeginNode(nodeInstance->myUID.AsInt());
		ImGui::PushID(nodeInstance->myUID.AsInt());
		ImGui::BeginVertical("node");

		ImGui::BeginHorizontal("header");
		ImGui::Spring(0);
		ImGui::TextUnformatted(nodeInstance->GetNodeName().c_str());
		ImGui::Spring(1);
		ImGui::Dummy(ImVec2(0, 28));
		ImGui::Spring(0);

		ImGui::EndHorizontal();
		ax::rect HeaderRect = ImGui_GetItemRect();
		ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);

		bool previusWasOut = false;
		bool isFirstInput = true;
		bool isFirstIteration = true;
		for (auto& pin : nodeInstance->GetPins())
		{
			if (isFirstIteration)
			{
				if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_OUT)
					isFirstInput = false;
				isFirstIteration = false;
			}

			if (pin.myPinType == SPin::EPinTypeInOut::EPinTypeInOut_IN)
			{
				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Input);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);
				if (pin.myVariableType == SPin::EPinType::EFlow)
					DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				else
					DrawTypeSpecificPin(pin, nodeInstance);

				ed::EndPin();
				previusWasOut = false;

			}
			else
			{
				if (isFirstInput)
					ImGui::SameLine(100, 0);

				ImGui::Indent(150.0f);


				ed::BeginPin(pin.myUID.AsInt(), ed::PinKind::Output);

				ImGui::Text(pin.myText.c_str());
				ImGui::SameLine(0, 0);

				DrawPinIcon(pin, nodeInstance->IsPinConnected(pin), 255);
				ed::EndPin();
				previusWasOut = true;
				ImGui::Unindent(150.0f);
				isFirstInput = false;
			}
		}

		ImGui::EndVertical();
		auto ContentRect = ImGui_GetItemRect();
		ed::EndNode();

		if (ImGui::IsItemVisible())
		{
			auto drawList = ed::GetNodeBackgroundDrawList(nodeInstance->myUID.AsInt());

			const auto halfBorderWidth = ed::GetStyle().NodeBorderWidth * 0.5f;
			auto headerColor = nodeInstance->GetColor();
			const auto uv = ImVec2(
				HeaderRect.w / (float)(4.0f * ImGui_GetTextureWidth(HeaderTextureID())),
				HeaderRect.h / (float)(4.0f * ImGui_GetTextureHeight(HeaderTextureID())));

			drawList->AddImageRounded(HeaderTextureID(),
				to_imvec(HeaderRect.top_left()) - ImVec2(8 - halfBorderWidth, 4 - halfBorderWidth),
				to_imvec(HeaderRect.bottom_right()) + ImVec2(8 - halfBorderWidth, 0),
				ImVec2(0.0f, 0.0f), uv,
				headerColor, ed::GetStyle().NodeRounding, 1 | 2);


			auto headerSeparatorRect = ax::rect(HeaderRect.bottom_left(), ContentRect.top_right());
			drawList->AddLine(
				to_imvec(headerSeparatorRect.top_left()) + ImVec2(-(8 - halfBorderWidth), -0.5f),
				to_imvec(headerSeparatorRect.top_right()) + ImVec2((8 - halfBorderWidth), -0.5f),
				ImColor(255, 255, 255, 255), 1.0f);
		}
		ImGui::PopID();
		ed::PopStyleVar();
	}

	for (auto& linkInfo : myGraphManager->CurrentGraph().myLinks)
		ed::Link(linkInfo.myID, linkInfo.myInputID, linkInfo.myOutputID);
}

ImColor CDrawNodePins::GetIconColor(unsigned int aType)
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

void CDrawNodePins::DrawPinIcon(const SPin& pin, bool connected, int alpha)
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

void CDrawNodePins::DrawTypeSpecificPin(SPin& aPin, CNodeInstance* aNodeInstance)
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

ImTextureID CDrawNodePins::HeaderTextureID()
{
	if (!myHeaderTextureID)
		myHeaderTextureID = ImGui_LoadTexture("Imgui/Sprites/BlueprintBackground.png");

	return myHeaderTextureID;
}