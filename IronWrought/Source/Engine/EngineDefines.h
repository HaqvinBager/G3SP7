#pragma once
#define LIGHTCOUNT 32
#define SAFE_DELETE(aPointer) delete aPointer; aPointer = nullptr;

// Uncomment before release.
//#ifdef NDEBUG
//#define EXCELSIOR_BUILD
//#else
////#define EXCELSIOR_BUILD // ! Manual toggle
//#endif

#ifdef EXCELSIOR_BUILD
#define ASSETPATH(path) std::string(path).c_str() // EXCELSIOR
#else
#define ASSETPATH(path) std::string("../../SP7UnityEditor/").append(path)//UNITY
#endif 

#define PLAYER_CAMERA_ID 1000
#define PLAYER_MODEL_ID 1001
#define PLAYER_GLOVE_ID 1002