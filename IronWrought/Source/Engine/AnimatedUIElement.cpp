#include "stdafx.h"
#include "AnimatedUIElement.h"
#include "SpriteFactory.h"
#include "SpriteInstance.h"
#include "Engine.h"
#include "WindowHandler.h"
#include "Scene.h"

#include "JsonReader.h"

#include "rapidjson\document.h"
#include "rapidjson\istreamwrapper.h"

CAnimatedUIElement::CAnimatedUIElement(CScene& aScene, bool addToScene)
    :  mySpriteInstance(new CSpriteInstance(aScene, addToScene))
    , myData(nullptr)
    , myLevel(1.0f)
{
    if (addToScene == false) {
        mySpriteInstance->SetShouldRender(false);
    }
    aScene.AddInstance(this);
}

CAnimatedUIElement::CAnimatedUIElement(const std::string& aFilePath, CScene& aScene, bool addToScene) 
    : mySpriteInstance(nullptr)
    , myLevel(1.0f)
{
    mySpriteInstance = new CSpriteInstance(aScene, addToScene);
    Init(aFilePath, addToScene, myLevel);
    aScene.AddInstance(this);
}

CAnimatedUIElement::~CAnimatedUIElement()
{
    // This is done for reinit of canvas. / Aki 20210401
    CScene& scene = IRONWROUGHT_ACTIVE_SCENE;
    scene.RemoveInstance(mySpriteInstance);
    delete mySpriteInstance;
    mySpriteInstance = nullptr;
}

void CAnimatedUIElement::Init(const std::string& aFilePath, const bool& aAddToScene, const float& aLevel)
{
    using namespace rapidjson;
    Document document = CJsonReader::Get()->LoadDocument(aFilePath);

    mySpriteInstance->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Texture Overlay"].GetString())));
    if (aAddToScene == false) {
        mySpriteInstance->SetShouldRender(false);
    }

    myData = CSpriteFactory::GetInstance()->GetVFXSprite(aFilePath);

    myLevel = aLevel;
}

void CAnimatedUIElement::Level(float aLevel)
{
    myLevel = aLevel;
}

float CAnimatedUIElement::Level() const
{
    return myLevel;
}

const DirectX::SimpleMath::Vector2 CAnimatedUIElement::GetPosition()
{
    return std::move(Vector2(mySpriteInstance->GetPosition().x, mySpriteInstance->GetPosition().y));
}
void CAnimatedUIElement::SetPosition(DirectX::SimpleMath::Vector2 aPosition)
{
    mySpriteInstance->SetPosition(aPosition);
}

void CAnimatedUIElement::SetShouldRender(const bool aShouldRender)
{
    mySpriteInstance->SetShouldRender(aShouldRender);
}

void CAnimatedUIElement::SetScale(const Vector2& aScale)
{
    mySpriteInstance->SetSize(aScale);
}

void CAnimatedUIElement::SetRenderLayer(const ERenderOrder& aRenderLayer, CScene& aScene)
{
    mySpriteInstance->SetRenderOrder(aRenderLayer, aScene);
}

void CAnimatedUIElement::ClearFromScene(CScene& aScene)
{
    aScene.RemoveInstance(mySpriteInstance);
}

CSpriteInstance* CAnimatedUIElement::GetInstance() const
{
    return mySpriteInstance;
}

SAnimatedSpriteData* CAnimatedUIElement::GetVFXBaseData()
{
    return myData;
}
