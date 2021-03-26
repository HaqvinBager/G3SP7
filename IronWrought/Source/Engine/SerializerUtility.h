#pragma once
#include <rapidjson\prettywriter.h>
#include <imgui.h>

template<class T>
struct Serializer {
    void Serialize(const char* aKey, T& aValue, rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
    {

    }
};

template<>
struct Serializer<Vector3> {
    void Serialize(const char* aKey, Vector3& aValue, rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
    {
        aWriter.Key(aKey);

        aWriter.StartObject();
        aWriter.Key("x");
        aWriter.Double(aValue.x);
        aWriter.EndObject();

        aWriter.StartObject();
        aWriter.Key("y");
        aWriter.Double(aValue.y);
        aWriter.EndObject();

        aWriter.StartObject();
        aWriter.Key("z");
        aWriter.Double(aValue.z);
        aWriter.EndObject();
    }
};

template<>
struct Serializer<Vector2> {
    void Serialize(const char* /*aKey*/, Vector2& aValue, rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
    {
        //aWriter.Key(aKey);

        //aWriter.StartObject();
        aWriter.Key("x");
        aWriter.Double(aValue.x);
        //aWriter.EndObject();

        //aWriter.StartObject();
        aWriter.Key("y");
        aWriter.Double(aValue.y);
        //aWriter.EndObject();

        //aWriter.EndObject();
    }
};

template<>
struct Serializer<std::vector<Vector2>> {
    void Serialize(const char* aKey, std::vector<Vector2>& someValues, rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
    {
        aWriter.Key(aKey);
        aWriter.StartArray();
        for (const auto& value : someValues)
        {
            aWriter.StartObject();
            aWriter.Key("x"); aWriter.Double(value.x);
            aWriter.Key("y"); aWriter.Double(value.y);
            aWriter.EndObject();
        }
        aWriter.EndArray();
    }
};

template<>
struct Serializer<ImVec2*> {
    void Serialize(const char* aKey, ImVec2* someValues, rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
    {
        aWriter.Key(aKey);
        aWriter.StartArray();
        for (unsigned int i = 0; i < 4; ++i)
        {
            aWriter.StartObject();
            aWriter.Key("x"); aWriter.Double(someValues[i].x);
            aWriter.Key("y"); aWriter.Double(someValues[i].y);
            aWriter.EndObject();
        }
        aWriter.EndArray();
    }
};

template<>
struct Serializer<std::vector<Vector3>> {
    void Serialize(const char* aKey, std::vector<Vector3>& someValues, rapidjson::PrettyWriter<rapidjson::StringBuffer>& aWriter)
    {
        aWriter.Key(aKey);
        aWriter.StartArray();
        for (const auto& value : someValues)
        {
            aWriter.StartObject();
            aWriter.Key("x");
            aWriter.Double(value.x);
            aWriter.Key("y");
            aWriter.Double(value.y);
            aWriter.Key("z");
            aWriter.Double(value.z);
            aWriter.EndObject();
        }
        aWriter.EndArray();
    }
};