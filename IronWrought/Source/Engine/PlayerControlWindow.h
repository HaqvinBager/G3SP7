#pragma once
#include "Window.h"

class CPlayerControllerComponent;
namespace IronWroughtImGui
{
    class CPlayerControlWindow : public CWindow
	{
    public:
        CPlayerControlWindow(const char* aName);
        ~CPlayerControlWindow() override;
        void OnEnable() override;
        void OnInspectorGUI() override;
        void OnDisable() override;

    private:
        float myPlayerWalkSpeed;
        float myPlayerCrouchSpeed;
        float myPlayerJumpHeight;
        float myPlayerFallSpeed;
        CPlayerControllerComponent* myPlayerShortcut;
	};
}

