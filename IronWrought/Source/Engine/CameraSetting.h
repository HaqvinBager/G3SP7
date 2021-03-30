#pragma once
#include "Window.h"
class CCameraComponent;
class CGameObject;
namespace IronWroughtImGui {
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
        float myCameraFov;
    };
}

