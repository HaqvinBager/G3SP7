#pragma once
#include "Window.h"
namespace ImGuiWindow {
    class CCameraSpeedSlider : public CWindow
    {
    public:

        CCameraSpeedSlider(const char* aName);
        ~CCameraSpeedSlider() override;
        void OnEnable() override;
        void OnInspectorGUI() override;
        void OnDisable() override;

    private:

        float myCameraSpeed;
    };
}

