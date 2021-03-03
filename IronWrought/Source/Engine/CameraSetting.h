#pragma once
#include "Window.h"
namespace ImGuiWindow {
    class CCameraSetting : public CWindow
    {
    public:

        CCameraSetting(const char* aName);
        ~CCameraSetting() override;
        void OnEnable() override;
        void OnInspectorGUI() override;
        void OnDisable() override;

    private:

        float myCameraSpeed;
    };
}

