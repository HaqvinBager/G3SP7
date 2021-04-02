#pragma once
#include "Window.h"
#include "GravityGloveComponent.h"

//class CGravityGloveComponent;

namespace IronWroughtImGui {
    class CGravityGloveEditor : public CWindow
    {
    public:
        CGravityGloveEditor(const char* aName);
        ~CGravityGloveEditor() override;

        void OnEnable() override;
        void OnInspectorGUI() override;
        void OnDisable() override;

    private:
        CGravityGloveComponent* myGlove;   
        std::unordered_map<const char*, float*> myValues;
    }; 
}

