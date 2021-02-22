#pragma once
#define LIGHTCOUNT 32
#define SAFE_DELETE(aPointer) delete aPointer; aPointer = nullptr;

#ifdef EXCELSIOR
#define ASSETPATH(path) std::string (path) // EXCELSIOR
#else
#define ASSETPATH(path) std::string("../../SP7UnityEditor/").append(path)//UNITY
#endif 

