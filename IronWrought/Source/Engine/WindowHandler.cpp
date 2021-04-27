#include "WindowHandler.h"
#include "Input.h"
#include "JsonReader.h"
#ifdef _DEBUG
#include "imgui_impl_win32.h"
#endif
#include "PostMaster.h"

#ifdef _DEBUG
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // DEBUG

LRESULT CWindowHandler::WinProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
    static CWindowHandler* windowHandler = nullptr;
    CREATESTRUCT* createStruct;

#ifdef _DEBUG
    if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
    {
        return true;
    }
#endif

    switch (uMsg)
    {
        case WM_CLOSE:

        case WM_DESTROY:       
            PostQuitMessage(0);
            return 0;

        case WM_CREATE:
            createStruct = reinterpret_cast<CREATESTRUCT*>(lParam);
            windowHandler = reinterpret_cast<CWindowHandler*>(createStruct->lpCreateParams);
            break;

        case WM_KILLFOCUS:
            windowHandler->LockCursor(false); // If we use this here the myWindowIsInEditingMode bool will be preserved
            break;

        case WM_SETFOCUS:
#ifndef NDEBUG
            windowHandler->myWindowIsInEditingMode ? windowHandler->LockCursor(false) : windowHandler->LockCursor(true);
#else
        if (windowHandler->myGameIsInMenu)
            windowHandler->ShowAndUnlockCursor();
        else
            windowHandler->HideAndLockCursor();
#endif
            break;

        default:
            Input::GetInstance()->UpdateEvents(uMsg, wParam, lParam);
            break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

CWindowHandler::CWindowHandler()
{
    myWindowHandle = 0;
    myResolutionScale = 1.0f;
    myCursorIsLocked = false;
    myWindowIsInEditingMode = false;
}

CWindowHandler::~CWindowHandler()
{
#ifdef _DEBUG
    ImGui_ImplWin32_Shutdown();
#endif // _DEBUG
    LockCursor(false);
    myCursorIsLocked = false;
    myWindowIsInEditingMode = false;
    myWindowHandle = 0;
    delete myResolution;
    myResolution = nullptr;
    UnregisterClass(L"3DEngine", GetModuleHandle(nullptr));
}

bool CWindowHandler::Init(CWindowHandler::SWindowData someWindowData)
{
    myWindowData = someWindowData;
    
    rapidjson::Document document = CJsonReader::Get()->LoadDocument("Json/Settings/WindowSettings.json");
 
    if (document.HasMember("Window Width"))
        myWindowData.myWidth = document["Window Width"].GetInt();

    if (document.HasMember("Window Height"))
        myWindowData.myHeight = document["Window Height"].GetInt();

    if (document.HasMember("Window Starting Pos X"))
        myWindowData.myX = document["Window Starting Pos X"].GetInt();

    if (document.HasMember("Window Starting Pos Y"))
        myWindowData.myY = document["Window Starting Pos Y"].GetInt();

    HCURSOR customCursor = NULL;
    if (document.HasMember("Cursor Path"))
        customCursor = LoadCursorFromFileA(document["Cursor Path"].GetString());

    if (customCursor == NULL) 
        customCursor = LoadCursor(nullptr, IDC_ARROW);

    HICON customIcon = NULL;
    if (document.HasMember("Icon Path"))
        customIcon = (HICON)LoadImageA(NULL, document["Icon Path"].GetString(), IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

    if (customIcon == NULL) 
        customIcon = (HICON)LoadImageA(NULL, "ironwrought.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_DEFAULTSIZE);

    WNDCLASSEX windowclass = {};
    windowclass.cbSize = sizeof(WNDCLASSEX);
    windowclass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    windowclass.lpfnWndProc = CWindowHandler::WinProc;
    windowclass.cbClsExtra = 0;
    windowclass.cbWndExtra = 0;
    windowclass.hInstance = GetModuleHandle(nullptr);
    windowclass.hIcon = customIcon;
    windowclass.hCursor = customCursor;
    windowclass.lpszClassName = L"3DEngine";
    RegisterClassEx(&windowclass);

    std::string gameName = "IronWrought";
    if (document.HasMember("Game Name"))
    {
        gameName = document["Game Name"].GetString();
    }

    bool borderless = false;
    if (document.HasMember("Borderless Window"))
    {
        borderless = document["Borderless Window"].GetBool();
    }

    if (borderless)
    {
        // Start in fullscreen
        myWindowHandle = CreateWindowA("3DEngine", gameName.c_str(), 
            WS_POPUP | WS_VISIBLE,
            0, 0, /*GetSystemMetrics(SM_CXSCREEN)*/myWindowData.myWidth, /*GetSystemMetrics(SM_CYSCREEN)*/myWindowData.myHeight,
            NULL, NULL, GetModuleHandle(nullptr), this);
    }
    else 
    {
        // Start in bordered window
        myWindowHandle = CreateWindowA("3DEngine", gameName.c_str(),
            WS_OVERLAPPEDWINDOW | WS_POPUP | WS_VISIBLE,
            myWindowData.myX, myWindowData.myY, myWindowData.myWidth, myWindowData.myHeight,
            nullptr, nullptr, nullptr, this);
    }

    ::SetCursor(customCursor);

#ifdef _DEBUG
    ImGui_ImplWin32_Init(myWindowHandle);
#endif // _DEBUG

    //LockCursor(true);

    myResolution = new Vector2();
    return true;
}

const HWND CWindowHandler::GetWindowHandle() const
{
    return myWindowHandle;
}

DirectX::SimpleMath::Vector2 CWindowHandler::GetCenterPosition()
{
    Vector2 center = {};
    RECT rect = { 0 };
    if (GetWindowRect(myWindowHandle, &rect))
    {
        center.x = (rect.right - rect.left) * 0.5f;
        center.y = (rect.bottom - rect.top) * 0.5f;
    }
    return center;
}

DirectX::SimpleMath::Vector2 CWindowHandler::GetResolution()
{

    return *myResolution;
}

void CWindowHandler::SetResolution(DirectX::SimpleMath::Vector2 aResolution)
{
    ::SetWindowPos(myWindowHandle, 0, 0, 0, (UINT)aResolution.x, (UINT)aResolution.y, SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
    
    SetInternalResolution();
}

const bool CWindowHandler::CursorLocked() const
{
    return myCursorIsLocked;
}

void CWindowHandler::LockCursor(bool aShouldLock)
{
    myCursorIsLocked = aShouldLock;
    if (aShouldLock)
    {
        while (::ShowCursor(FALSE) >= 0);

        Vector2 center = GetCenterPosition();
        SetCursorPos(static_cast<int>(center.x), static_cast<int>(center.y));
    }
    else {
        while (::ShowCursor(TRUE) < 0);
    }
    aShouldLock ? SetCapture(myWindowHandle) : SetCapture(nullptr);
}

void CWindowHandler::HideAndLockCursor(const bool& anIsInEditorMode)
{
    std::cout << __FUNCTION__ << std::endl;
    while (::ShowCursor(FALSE) >= 0);
    SetCapture(myWindowHandle);
    myCursorIsLocked = true;
    myWindowIsInEditingMode = anIsInEditorMode;

    Vector2 center = GetCenterPosition();
    SetCursorPos(static_cast<int>(center.x), static_cast<int>(center.y));
    
    CMainSingleton::PostMaster().Send({ EMessageType::CursorHideAndLock, nullptr });
}

void CWindowHandler::ShowAndUnlockCursor(const bool& anIsInEditorMode)
{
    while (::ShowCursor(TRUE) < 0);
    SetCapture(nullptr);
    myCursorIsLocked = false;
    myWindowIsInEditingMode = anIsInEditorMode;
    CMainSingleton::PostMaster().Send({ EMessageType::CursorShowAndUnlock, nullptr });
}

void CWindowHandler::GameIsInMenu(const bool& aIsInMenu)
{
    myGameIsInMenu = aIsInMenu;
}

void CWindowHandler::SetInternalResolution()
{
    LPRECT rect = new RECT{ 0, 0, 0, 0 };
    if (GetClientRect(myWindowHandle, rect) != 0) {
        myResolution->x = static_cast<float>(rect->right);
        myResolution->y = static_cast<float>(rect->bottom);
    }
    ClipCursor(rect);
    delete rect;

    myResolutionScale = myResolution->y / 1080.0f;
}

void CWindowHandler::SetWindowTitle(std::string aString)
{
    SetWindowTextA(myWindowHandle, aString.c_str());
}

const float CWindowHandler::GetResolutionScale() const
{
    return myResolutionScale;
}
