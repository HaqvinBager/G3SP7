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

CAnimatedUIElement::CAnimatedUIElement()
    : mySpriteInstance(nullptr)
    , myLevel(1.0f)
    , myData(nullptr)
{}

CAnimatedUIElement::CAnimatedUIElement(const std::string& aFilePath, const float& aLevel) 
    : mySpriteInstance(nullptr)
    , myLevel(aLevel)
{
    mySpriteInstance = new CSpriteInstance();
    Init(aFilePath, myLevel);
}

CAnimatedUIElement::~CAnimatedUIElement()
{
    delete mySpriteInstance;
    mySpriteInstance = nullptr;
}

void CAnimatedUIElement::Init(const std::string& aFilePath, const float& aLevel)
{
    using namespace rapidjson;
    Document document = CJsonReader::Get()->LoadDocument(aFilePath);

    if(!mySpriteInstance)
        mySpriteInstance = new CSpriteInstance();

    mySpriteInstance->Init(CSpriteFactory::GetInstance()->GetSprite(ASSETPATH(document["Texture Overlay"].GetString())));

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

const bool CAnimatedUIElement::GetShouldRender() const
{
    return mySpriteInstance->GetShouldRender();
}

void CAnimatedUIElement::SetScale(const Vector2& aScale)
{
    mySpriteInstance->SetSize(aScale);
}

void CAnimatedUIElement::SetRenderLayer(const ERenderOrder& aRenderLayer)
{
    mySpriteInstance->SetRenderOrder(aRenderLayer);
}

const ERenderOrder CAnimatedUIElement::GetRendererLayer() const
{
    return mySpriteInstance->GetRenderOrder();
}

CSpriteInstance* CAnimatedUIElement::GetInstance() const
{
    return mySpriteInstance;
}

SAnimatedSpriteData* CAnimatedUIElement::GetVFXBaseData()
{
    return myData;
}
