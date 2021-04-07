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
#define TINTED_MODEL_MAX_NR_OF_TINTS 4

namespace ModelHelperFunctions
{

    bool ReplaceModelAndLoadTints(CGameObject* aGameObject, const std::string& aTintDataPath)
    {
        CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
        assert(modelComp != nullptr && "GameObject has no CModelComponent!");
        if (!modelComp)
            return false;
        
        bool success = modelComp->DeserializeTintData(aTintDataPath);

        modelComp->SetModel(modelComp->GetModelPath());

        return success;
    }
    bool ReplaceModelAndLoadTints(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
    {
        CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
        assert(modelComp != nullptr && "GameObject has no CModelComponent!");
        if (!modelComp)
            return false;

        bool success = modelComp->DeserializeTintData(aTintDataPath);

        modelComp->SetModel(modelComp->GetModelPath());
        anOutModelPath = modelComp->GetModelPath();
        modelComp->SetModel(anOutModelPath);

        return success;
    }

    CGameObject* CreateGameObjectWithTintedModel(const int anInstanceId, const std::string& aModelPath, const std::string& aTintDataPath)
    {
        CGameObject* go = new CGameObject(anInstanceId);
        CModelComponent* modelComp = go->AddComponent<CModelComponent>(*go, aModelPath);
        modelComp->DeserializeTintData(aTintDataPath);

        return go;
    }

    bool AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath)
    {
        using namespace rapidjson;
        Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
        CModelComponent* modelComp = aGameObject->AddComponent<CModelComponent>(*aGameObject, document["Model path"].GetString());
        modelComp->DeserializeTintData(aTintDataPath);
        return true;
    }

    bool AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
    {
        using namespace rapidjson;
        Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
        anOutModelPath = document["Model path"].GetString();
        return AddModelComponentWithTintsFromData(aGameObject, aTintDataPath);
    }

    //namespace Internal
    //{
    //    std::vector<DirectX::SimpleMath::Vector4> DeserializeTintsData(rapidjson::Document& aDocument, const std::string& aTintDataPath)
    //    {
    //        using namespace rapidjson;
    //        std::vector<Vector4> tints;
    //
    //        assert(aDocument["Tints"].IsArray() && std::string("Tints is not an array in: " + aTintDataPath).c_str());
    //        if (!aDocument["Tints"].IsArray())
    //            return std::move(tints);
    //
    //        GenericArray tintsArray = aDocument["Tints"].GetArray();
    //        tints.resize(tintsArray.Size());
    //        for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    //        {
    //            GenericObject tintRGB = tintsArray[i].GetObjectW();
    //            //if (tintRGB.HasMember("Texture")) // THE FUTURE?
    //            //    // read the texture instead
    //            //else
    //            //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    //            tints[i].x = tintRGB.HasMember("Red")   ? tintRGB["Red"].GetFloat()   : 1.0f;
    //            tints[i].y = tintRGB.HasMember("Green") ? tintRGB["Green"].GetFloat() : 1.0f;
    //            tints[i].z = tintRGB.HasMember("Blue")  ? tintRGB["Blue"].GetFloat()  : 1.0f;
    //            tints[i].w = tintRGB.HasMember("Alpha") ? tintRGB["Alpha"].GetFloat() : 1.0f;
    //        }
    //        return std::move(tints);
    //    }
    //
    //    DirectX::SimpleMath::Vector4 DeserializeEmissiveData(rapidjson::Document& aDocument, const std::string& aTintDataPath)
    //    {
    //        using namespace rapidjson;
    //        Vector4 emissive;
    //
    //        assert(aDocument["Emissive"].IsObject() && std::string("Emissive is not an object in: " + aTintDataPath).c_str());
    //        if (!aDocument["Emissive"].IsObject())
    //            return std::move(emissive);
    //
    //        auto emissiveObject = aDocument["Emissive"].GetObjectW();
    //        emissive.x = emissiveObject["Red"].GetFloat();
    //        emissive.y = emissiveObject["Green"].GetFloat();
    //        emissive.z = emissiveObject["Blue"].GetFloat();
    //        emissive.w = emissiveObject["Alpha"].GetFloat();
    //        return std::move(emissive);
    //    }
    //}
}

/*
namespace ModelHelperFunctions
{
    //bool LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath)
    //{
    //    using namespace rapidjson;
    //    Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
    //
    //    //assert(document["Tints"].IsArray() && std::string("Tints is not an array in: " + aTintDataPath).c_str());
    //    //if (!document["Tints"].IsArray())
    //    //    return false;
    //    //GenericArray tintsArray = document["Tints"].GetArray();
    //    //std::vector<Vector3> tints(tintsArray.Size());
    //    //for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    //    //{
    //    //    GenericObject tintRGB = tintsArray[i].GetObjectW();
    //    //    //if (tintRGB.HasMember("Texture")) // THE FUTURE? :)
    //    //    //    // read the texture instead
    //    //    //else
    //    //    //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    //    //    tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    //    //}
    //    std::vector<Vector4> tints = Internal::DeserializeTintsData(document, aTintDataPath);
    //    Vector4 emissive = Internal::DeserializeEmissiveData(document, aTintDataPath);
    //    bool result = aModelComponent->SetTints(tints);
    //    aModelComponent->Emissive(emissive);
    //    return result;
    //}
    //
    //bool LoadTintsToModelComponent(CModelComponent* aModelComponent, const std::string& aTintDataPath, std::string& anOutModelPath)
    //{
    //    using namespace rapidjson;
    //    Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
    //
    //    anOutModelPath = document["Model path"].GetString();
    //
    //    return LoadTintsToModelComponent(aModelComponent, aTintDataPath);
    //}
    //
    //bool LoadTintsToModelComponent(CModelComponent* aModelComponent, rapidjson::Document& aDocument)
    //{
    //    return LoadTintsToModelComponent(aModelComponent, "unknown");
    //}
    //
    //bool LoadTintsToModelComponent(CModelComponent* aModelComponent, rapidjson::Document& aDocument, std::string& anOutModelPath)
    //{
    //    using namespace rapidjson;
    //    assert(aDocument["Tints"].IsArray() && std::string("rapidjson::Document is missing Tints array!.").c_str());
    //    if (!aDocument["Tints"].IsArray())
    //        return false;
    //
    //    anOutModelPath = aDocument["Model path"].GetString();
    //
    //    return LoadTintsToModelComponent(aModelComponent, aDocument);
    //}
    //
    //bool SaveTintsFromModelComponent(CModelComponent* aModelComponent, const std::string& aModelPath, const std::array<std::string, 4>& someTexturePaths, const std::string& aTintDataPath)
    //{
    //    std::string aFileName;
    //    if (aTintDataPath.empty())
    //    {
    //        aFileName = TINTED_MODEL_DATA_PATH;
    //        aFileName.append("TintData");
    //
    //        time_t rawTime = time(0);
    //        struct tm timeInfo;
    //        time(&rawTime);
    //        localtime_s(&timeInfo, &rawTime);
    //        char timeBuffer[18];// YYYY-MM-DD-hhmmss == 17 + '\0' = 18
    //        ZeroMemory(timeBuffer, 18);
    //        strftime(timeBuffer, 18, "%F-%H%M%S", &timeInfo);//%F gets YYYY-MM-DD, %H gets hours: 00->24, %M gets minutes: 00->59, %S gets seconds: 00->59. 
    //
    //        aFileName.append(timeBuffer);
    //        aFileName.append(".json");
    //    }
    //    else
    //    {
    //        aFileName = aTintDataPath;
    //    }
    //
    //    const std::vector<Vector4>& tints = aModelComponent->GetTints();
    //    std::array<std::string, TINTED_MODEL_MAX_NR_OF_TINTS> tintKeys = {
    //        "Tint1-Primary-Red Channel",
    //        "Tint2-Secondary-Green Channel",
    //        "Tint3-Tertiary-Blue Channel",
    //        "Tint4-Accents-Alpha Channel"
    //    };
    //    
    //
    //    using namespace rapidjson;
    //    StringBuffer stringBuffer;
    //    PrettyWriter<StringBuffer> prettyWriter(stringBuffer);
    //    prettyWriter.StartObject();
    //    {
    //        prettyWriter.Key("Model path");
    //        prettyWriter.String(aModelPath.c_str());
    //        prettyWriter.Key("Tints");
    //        prettyWriter.StartArray();
    //        {
    //            for (short i = 0; i < TINTED_MODEL_MAX_NR_OF_TINTS; ++i)
    //            {
    //                // Could be made into a Serialize function in CModelComponent
    //                prettyWriter.StartObject();
    //                {
    //                    prettyWriter.Key(tintKeys[i].c_str()); 
    //                    prettyWriter.String("");
    //                    const Vector4& tint = tints[i];
    //                    prettyWriter.Key("Red");
    //                    prettyWriter.Double(static_cast<double>(tint.x));
    //                    prettyWriter.Key("Green");
    //                    prettyWriter.Double(static_cast<double>(tint.y));
    //                    prettyWriter.Key("Blue");
    //                    prettyWriter.Double(static_cast<double>(tint.z));
    //                    prettyWriter.Key("Alpha");
    //                    prettyWriter.Double(static_cast<double>(tint.w));
    //                }
    //                prettyWriter.EndObject();
    //            }
    //        }
    //        prettyWriter.EndArray();
    //
    //        const std::string emissiveKey = "Emissive";
    //        prettyWriter.Key(emissiveKey.c_str()); 
    //        prettyWriter.StartObject();
    //        {
    //            const Vector4& tint = aModelComponent->Emissive();
    //            prettyWriter.Key("Red");
    //            prettyWriter.Double(static_cast<double>(tint.x));
    //            prettyWriter.Key("Green");
    //            prettyWriter.Double(static_cast<double>(tint.y));
    //            prettyWriter.Key("Blue");
    //            prettyWriter.Double(static_cast<double>(tint.z));
    //            prettyWriter.Key("Alpha");
    //            prettyWriter.Double(static_cast<double>(tint.w));
    //        }
    //        prettyWriter.EndObject();
    //    }
    //    prettyWriter.EndObject();
    //
    //    std::ofstream outFileStream(aFileName);
    //    outFileStream << stringBuffer.GetString();
    //    outFileStream.close();
    //
    //    return true;
    //}
    //bool SaveTintsFromModelComponent(CGameObject* aGameObject, const std::string& aModelPath, const std::string& aTintDataPath)
    //{
    //    CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
    //    assert(modelComp != nullptr && "GameObject has no CModelComponent!");
    //    if (!modelComp)
    //        return false;
    //
    //    return SaveTintsFromModelComponent(modelComp, aModelPath, {"","","",""}, aTintDataPath);
    //}
    //
    //
    //
    //bool LoadTintsToModelComponent(CGameObject* aGameObject, const std::string& aTintDataPath)
    //{
    //    CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
    //    assert(modelComp != nullptr && "GameObject has no CModelComponent!");
    //    if (!modelComp)
    //        return false;
    //
    //    return LoadTintsToModelComponent(modelComp, aTintDataPath);
    //}
    //
    //bool LoadTintsToModelComponent(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
    //{
    //    CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
    //    assert(modelComp != nullptr && "GameObject has no CModelComponent!");
    //    if (!modelComp)
    //        return false;
    //
    //    return LoadTintsToModelComponent(modelComp, aTintDataPath, anOutModelPath);
    //}

    bool ReplaceModelAndLoadTints(CGameObject* aGameObject, const std::string& aTintDataPath)
    {
        CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
        assert(modelComp != nullptr && "GameObject has no CModelComponent!");
        if (!modelComp)
            return false;

        std::string newModelPath = "";
        bool returnValue = LoadTintsToModelComponent(modelComp, aTintDataPath, newModelPath);

        modelComp->SetModel(newModelPath);
        return returnValue;
    }
    bool ReplaceModelAndLoadTints(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
    {
        CModelComponent* modelComp = aGameObject->GetComponent<CModelComponent>();
        assert(modelComp != nullptr && "GameObject has no CModelComponent!");
        if (!modelComp)
            return false;

        bool returnValue = LoadTintsToModelComponent(modelComp, aTintDataPath, anOutModelPath);

        modelComp->SetModel(anOutModelPath);
        return returnValue;
    }

    CGameObject* CreateGameObjectWithTintedModel(const int anInstanceId, const std::string& aModelPath, const std::string& aTintDataPath)
    {
        CGameObject* go = new CGameObject(anInstanceId);
        LoadTintsToModelComponent(go->AddComponent<CModelComponent>(*go, aModelPath), aTintDataPath);
        return go;
    }

    bool AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath)
    {
        using namespace rapidjson;
        Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
        return LoadTintsToModelComponent(aGameObject->AddComponent<CModelComponent>(*aGameObject, document["Model path"].GetString()), document);
    }

    bool AddModelComponentWithTintsFromData(CGameObject* aGameObject, const std::string& aTintDataPath, std::string& anOutModelPath)
    {
        using namespace rapidjson;
        Document document = CJsonReader::Get()->LoadDocument(aTintDataPath);
        anOutModelPath = document["Model path"].GetString();
        return LoadTintsToModelComponent(aGameObject->AddComponent<CModelComponent>(*aGameObject, document["Model path"].GetString()), document);
    }

    //namespace Internal
    //{
    //    std::vector<DirectX::SimpleMath::Vector4> DeserializeTintsData(rapidjson::Document& aDocument, const std::string& aTintDataPath)
    //    {
    //        using namespace rapidjson;
    //        std::vector<Vector4> tints;
    //
    //        assert(aDocument["Tints"].IsArray() && std::string("Tints is not an array in: " + aTintDataPath).c_str());
    //        if (!aDocument["Tints"].IsArray())
    //            return std::move(tints);
    //
    //        GenericArray tintsArray = aDocument["Tints"].GetArray();
    //        tints.resize(tintsArray.Size());
    //        for (short i = 0; i < static_cast<short>(tintsArray.Size()); ++i)
    //        {
    //            GenericObject tintRGB = tintsArray[i].GetObjectW();
    //            //if (tintRGB.HasMember("Texture")) // THE FUTURE?
    //            //    // read the texture instead
    //            //else
    //            //    //tints[i] = Vector3(tintRGB["Red"].GetFloat(), tintRGB["Green"].GetFloat(), tintRGB["Blue"].GetFloat());
    //            tints[i].x = tintRGB.HasMember("Red")   ? tintRGB["Red"].GetFloat()   : 1.0f;
    //            tints[i].y = tintRGB.HasMember("Green") ? tintRGB["Green"].GetFloat() : 1.0f;
    //            tints[i].z = tintRGB.HasMember("Blue")  ? tintRGB["Blue"].GetFloat()  : 1.0f;
    //            tints[i].w = tintRGB.HasMember("Alpha") ? tintRGB["Alpha"].GetFloat() : 1.0f;
    //        }
    //        return std::move(tints);
    //    }
    //
    //    DirectX::SimpleMath::Vector4 DeserializeEmissiveData(rapidjson::Document& aDocument, const std::string& aTintDataPath)
    //    {
    //        using namespace rapidjson;
    //        Vector4 emissive;
    //
    //        assert(aDocument["Emissive"].IsObject() && std::string("Emissive is not an object in: " + aTintDataPath).c_str());
    //        if (!aDocument["Emissive"].IsObject())
    //            return std::move(emissive);
    //
    //        auto emissiveObject = aDocument["Emissive"].GetObjectW();
    //        emissive.x = emissiveObject["Red"].GetFloat();
    //        emissive.y = emissiveObject["Green"].GetFloat();
    //        emissive.z = emissiveObject["Blue"].GetFloat();
    //        emissive.w = emissiveObject["Alpha"].GetFloat();
    //        return std::move(emissive);
    //    }
    //}
}
*/