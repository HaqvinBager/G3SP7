#pragma once
//Windows API
#include "WinInclude.h"

//C++
#include <comdef.h>
#include <d3d11.h>
#include <codecvt>
#include <cstdarg>
#include <iostream>
//#include <fstream> Om den behövs, behöver du förmodligen JsonReader. Inkludera JsonReader // Nico 19 jan 2021
#include <vector>
#include <array>
#include <string>
//
//DirectXTK
#include <DDSTextureLoader.h>
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;
//
//Utility
#include "Input.h"
#include "Timer.h"
#include "EngineException.h"
//#include "ModelMath.h"
//#include "JsonReader.h"
//#include "Utility.h"
//
//Engine
#include "DirectXFramework.h"
#include "WindowHandler.h"
#include "EngineDefines.h"
//
//Components
#include "TransformComponent.h"
//
//Singleton
#include "MainSingleton.h"
//

//When Completed
//#include "Engine.h"
//#include "Scene.h"
//
//#include "PostMaster.h"
//
//#include "CTextInstance.h" <-- Changed to Component
//#include "CSpriteInstance.h" <-- Changed to Component
//#include "CVFXInstance.h" <-- Changed to Component
//#include "CParticleComponent.h"
//
//#include "CollisionManager.h"
//
//#include "FModWrapper.h"
//
//#include "PopupTextService.h"
//#include "DialogueSystem.h"


