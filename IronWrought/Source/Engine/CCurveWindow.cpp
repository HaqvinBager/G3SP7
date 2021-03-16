#include "stdafx.h"
#include "CCurveWindow.h"
#include <widget_curve.h>

IronWroughtImGui::CCurveWindow::CCurveWindow(const char* aName)
	: CWindow(aName)
{
	std::fill_n(myValueMap["Uniform Scale"], 4, 0.0f);
	std::fill_n(myValueMap["Opacity"], 4, 0.0f);
	std::fill_n(myValueMap["Speed"], 4, 0.0f);

/* 1hr */	//Read all VFX Json Files 
/* 1hr */		//Drop-Down Selector for choosing which VFX you want to edit
/* ^^^ */		//Load Selected Data into memory
/* 4hr */			//Visualize the Data for each different Type
						//Vector3
						//Vector2
						//Bezier Curve (For any single-float values?)
						//Select Other Json file w/ specific Prefix (_PD or something)?
						//Names for all the values
			
/* 1hr */		//Save Edited Data back into Json File
/* 2hr */			//Use a temp-json file that saves the current state of the data
/* 1hr */			//If the user closes the window, this temp file writes to the VFX Json (aka it saves whatever u did):))<3

}
IronWroughtImGui::CCurveWindow::~CCurveWindow()
{
}

void IronWroughtImGui::CCurveWindow::OnEnable()
{
}

void IronWroughtImGui::CCurveWindow::OnInspectorGUI()
{
	ImGui::Begin(Name(), Open());

	ImGui::Begin("Test");
	for (auto& keyValue : myValueMap)
	{
		ImGui::Bezier(keyValue.first.c_str(), keyValue.second);
	}
	ImGui::End();

	ImGui::End();
}

void IronWroughtImGui::CCurveWindow::OnDisable()
{
}