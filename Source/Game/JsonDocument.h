#pragma once
#include <rapidjson\document.h>
#include <string>

class JsonDocument
{
public:
    JsonDocument(const std::string& aJsonFile);
    void SaveToFile(const std::string& aJsonFile, bool aSavePretty = false);

    void SetFloat(const std::string& aSource, float aValue);
    float GetFloat(const std::string& aSource, float aDefaultValue = 0.0f);

    void SetBool(const std::string& aSource, bool aValue);
    bool GetBool(const std::string& aSource, bool aDefaultValue = false);

    inline rapidjson::Document& GetDocument()
    {
        return myDocument;
    }
private:
    rapidjson::Document myDocument;
};
