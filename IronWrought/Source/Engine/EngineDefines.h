#pragma once
#define LIGHTCOUNT 32
#define SAFE_DELETE(aPointer) delete aPointer; aPointer = nullptr;

#ifdef EXCELSIOR
#define ASSETPATH std::string ("") // EXCELSIOR
#else
#define ASSETPATH std::string ("../../SP7UnityEditor/") //UNITY
#endif 

