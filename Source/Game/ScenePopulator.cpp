#include "pch.h"
#include "ScenePopulator.h"

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <Engine/GameObject.h>
#include <fstream>

// Components
#include "ModelComponent.h"

bool ScenePopulator::PopulateVectorFromJson(std::vector<SE::CGameObject>& aVectorToFill, const std::string_view& aFilePath)
{
    // Load ENTIRE file into RAM before parsing
    std::ifstream file{ std::string(aFilePath) };
    std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };

    rapidjson::StringStream stream(content.c_str());
    rapidjson::Document document;
    if (document.ParseStream(stream).HasParseError())
    {
        perr("<%s> Parse error at offset %u: \"%s\", when loading file \"%s\"",
            "ScenePopulator::PopulateVectorFromJson",
            (unsigned)document.GetErrorOffset(),
            rapidjson::GetParseError_En(document.GetParseError()),
            aFilePath.data()
        );
        return false;
    }

    auto jsonObjects = document["GameObjects"].GetArray();
    aVectorToFill.reserve(jsonObjects.Size());
    for (rapidjson::Value& jsonObject : jsonObjects)
    {
        if (jsonObject.HasMember("DummyObject"))
            continue;

        std::string tag = jsonObject["Tag"].GetString();
        std::string name = jsonObject["Name"].GetString();
        Vector3f position(
            jsonObject["Position"]["x"].GetFloat(),
            jsonObject["Position"]["y"].GetFloat(),
            jsonObject["Position"]["z"].GetFloat()
        );
        Vector3f rotation(
            jsonObject["Rotation"]["x"].GetFloat(),
            jsonObject["Rotation"]["y"].GetFloat() + 180.0f,
            jsonObject["Rotation"]["z"].GetFloat()
        );
        Vector3f scale(
            jsonObject["Scale"]["x"].GetFloat(),
            jsonObject["Scale"]["y"].GetFloat(),
            jsonObject["Scale"]["z"].GetFloat()
        );

        SE::CGameObject gameObject;

        // Todo: Fix Unity Scale to our Scale

        ModelComponent* model = gameObject.AddComponent<ModelComponent>();
        model->SetModelToInit("Models/" + name + "/" + name + ".fbx");
        model->SetRelativePosition(position * 100.f);
        model->SetRelativeRotation(rotation);
        model->SetScale(scale);

        aVectorToFill.push_back(gameObject);
    }

    return true;
}
