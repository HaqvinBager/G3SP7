#pragma once
#define LIGHTCOUNT 32
#define SAFE_DELETE(aPointer) delete aPointer; aPointer = nullptr;

//#define EXCELSIOR_COMPILE// If define disables using Unity Asset path prefix (UnityProject/Assets), enables Bin/Assets

//#ifndef EXCELSIOR_COMPILE
//#define ASSETPATH(path) std::string("../../SP7UnityEditor/").append(path)//UNITY
//#else
//#define ASSETPATH(path) std::string (path) // EXCELSIOR
//#endif

#ifdef EXCELSIOR
#define ASSETPATH(path) std::string (path) // EXCELSIOR
#else
#define ASSETPATH(path) std::string("../../SP7UnityEditor/").append(path)//UNITY
#endif 
