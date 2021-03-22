#include "stdafx.h"
#include "ModelHelperFunctions.h"

#include "ModelComponent.h"
#include "GameObject.h"

#include "JsonReader.h"
#include <fstream>
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include <time.h>

#define TINTED_MODEL_DATA_PATH ASSETPATH("Assets/Graphics/TintedModels/Data/")
//#define TINTED_MODEL_DATA_PATH "Assets/TintedModels/Data/"
#define TINTED_MODEL_MAX_NR_OF_TINTS 4

bool CModelHelperFunctions::LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath)
{
    using namespace rapidjson;
    Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
   
    assert(document["Tints"].IsArray() && std::string("Tints is not an array in: " + aTintDataPath).c_str());
    if (!document["Tints"].IsArray())
        return false;
    GenericArray tintsArray = document["Tints"].GetArray();
    std::vector<Vector3> tints(tintsArray.Size());
    for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    {
        GenericObject tintRGB = tintsArray[i].GetObjectW();
        //if (tintRGB.HasMember("Texture")) // THE FUTURE? :)
        //    // read the texture instead
        //else
        //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
        tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    }
    return aModelComponent->SetTints(tints);
}

bool CModelHelperFunctions::LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath, std::string& anOutModelPath)
{
    using namespace rapidjson;
    Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);

    anOutModelPath = document["Model path"].GetString();

    assert(document["Tints"].IsArray() && std::string("Tints is not an array in: " + aTintDataPath).c_str());
    if (!document["Tints"].IsArray())
        return false;
    GenericArray tintsArray = document["Tints"].GetArray();
    std::vector<Vector3> tints(tintsArray.Size());
    for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    {
        GenericObject tintRGB = tintsArray[i].GetObjectW();
        //if (tintRGB.HasMember("Texture")) // THE FUTURE?
        //    // read the texture instead
        //else
        //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
        tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    }
    return aModelComponent->SetTints(tints);
}

bool CModelHelperFunctions::LoadTintsToModelComponent(CModelComponent* aModelComponent, rapidjson::Document& aDocument)
{
    using namespace rapidjson;
    assert(aDocument["Tints"].IsArray() && std::string("rapidjson::Document is missing Tints array!.").c_str());
    if (!aDocument["Tints"].IsArray())
        return false;

    GenericArray tintsArray = aDocument["Tints"].GetArray();
    std::vector<Vector3> tints(tintsArray.Size());
    for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    {
        GenericObject tintRGB = tintsArray[i].GetObjectW();
        //if (tintRGB.HasMember("Texture")) // THE FUTURE?
        //    // read the texture instead
        //else
        //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
        tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    }
    return aModelComponent->SetTints(tints);
}

bool CModelHelperFunctions::LoadTintsToModelComponent(CModelComponent* aModelComponent, rapidjson::Document& aDocument, std::string& anOutModelPath)
{
    using namespace rapidjson;
    assert(aDocument["Tints"].IsArray() && std::string("rapidjson::Document is missing Tints array!.").c_str());
    if (!aDocument["Tints"].IsArray())
        return false;

    anOutModelPath = aDocument["Model path"].GetString();

    GenericArray tintsArray = aDocument["Tints"].GetArray();
    std::vector<Vector3> tints(tintsArray.Size());
    for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    {
        GenericObject tintRGB = tintsArray[i].GetObjectW();
        //if (tintRGB.HasMember("Texture")) // THE FUTURE?
        //    // read the texture instead
        //else
        //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
        tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    }
    return aModelComponent->SetTints(tints);
}

bool CModelHelperFunctions::SaveTintsFromModelComponent(CModelComponent* aModelComponent, const std::string& aModelPath, const std::string& aTintDataPath)
{
    std::string aFileName;
    if (aTintDataPath.empty())
    {
        aFileName = TINTED_MODEL_DATA_PATH;
        aFileName.append("TintData");

        time_t rawTime = time(0);
        struct tm timeInfo;
        time(&rawTime);
        localtime_s(&timeInfo, &rawTime);
        char timeBuffer[18];// YYYY-MM-DD-hhmmss == 17 + '\0' = 18
        ZeroMemory(timeBuffer, 18);
        strftime(timeBuffer, 18, "%F-%H%M%S", &timeInfo);//%F gets YYYY-MM-DD, %H gets hours: 00->24, %M gets minutes: 00->59, %S gets seconds: 00->59. 

        aFileName.append(timeBuffer);
        aFileName.append(".json");
    }
    else
    {
        aFileName = aTintDataPath;
    }

    const std::vector<Vector3>& tints = aModelComponent->GetTints();
    std::array<std::string, TINTED_MODEL_MAX_NR_OF_TINTS> tintKeys = {
        "Tint1-Primary-Red Channel",
        "Tint2-Secondary-Green Channel",
        "Tint3-Tertiary-Blue Channel",
        "Tint4-Accents-Alpha Channel"
    };

    using namespace rapidjson;
    StringBuffer stringBuffer;
    PrettyWriter<StringBuffer> prettyWriter(stringBuffer);
    prettyWriter.StartObject();
    {
        prettyWriter.Key("Model path");
        prettyWriter.String(aModelPath.c_str());
        prettyWriter.Key("Tints");
        prettyWriter.StartArray();
        {
            for (short i = 0; i < TINTED_MODEL_MAX_NR_OF_TINTS; ++i)
            {
                // Could be made into a Serialize function in CModelComponent
                prettyWriter.StartObject();
                {
                    prettyWriter.Key(tintKeys[i].c_str()); 
                    prettyWriter.String("");
                    const Vector3& tint = tints[i];
                    prettyWriter.Key("Red");
                    prettyWriter.Double(static_cast<double>(tint.x));
                    prettyWriter.Key("Green");
                    prettyWriter.Double(static_cast<double>(tint.y));
                    prettyWriter.Key("Blue");
                    prettyWriter.Double(static_cast<double>(tint.z));
                }
                prettyWriter.EndObject();
            }
        }
        prettyWriter.EndArray();
    }
    prettyWriter.EndObject();
       
    std::ofstream outFileStream(aFileName);
    outFileStream << stringBuffer.GetString();
    outFileStream.close();

    return true;
}

bool CModelHelperFunctions::LoadTintsToModelComponent(CGameObject* aGameObject, const std::string& aTintDataPath)
{
    CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
    assert(modelComp != nullptr && "GameObject has no CModelComponent!");
    if (!modelComp)
        return false;

    return LoadTintsToModelComponent(modelComp, aTintDataPath);
}

bool CModelHelperFunctions::LoadTintsToModelComponent(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
{
    CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
    assert(modelComp != nullptr && "GameObject has no CModelComponent!");
    if (!modelComp)
        return false;

    return LoadTintsToModelComponent(modelComp, aTintDataPath, anOutModelPath);
}

bool CModelHelperFunctions::SaveTintsFromModelComponent(CGameObject* aGameObject, const std::string& aModelPath, const std::string& aTintDataPath)
{
    CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
    assert(modelComp != nullptr && "GameObject has no CModelComponent!");
    if (!modelComp)
        return false;

    return SaveTintsFromModelComponent(modelComp, aModelPath, aTintDataPath);
}

CGameObject* CModelHelperFunctions::CreateGameObjectWithTintedModel(const int anInstanceId, const std::string& aModelPath, const std::string& aTintDataPath)
{
    CGameObject* go = new CGameObject(anInstanceId);
    LoadTintsToModelComponent(go->AddComponent<CModelComponent>(*go, aModelPath), aTintDataPath);
    return go;
}

bool CModelHelperFunctions::AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath)
{
    using namespace rapidjson;
    Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
    return LoadTintsToModelComponent(aGameObject->AddComponent<CModelComponent>(*aGameObject, document["Model path"].GetString()), document);
}

bool CModelHelperFunctions::AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
{
    using namespace rapidjson;
    Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
    anOutModelPath = document["Model path"].GetString();
    return LoadTintsToModelComponent(aGameObject->AddComponent<CModelComponent>(*aGameObject, document["Model path"].GetString()), document);
}


