#pragma once
#include <map>
#include <string>
#include <vector>
#include "JsonExposedTemplate.h"
#include "JsonExposedVariable.h"

class JsonDocument;
class JsonManager
{
public:
    JsonManager();
    ~JsonManager();
    void InitDocument(const std::string& aConfigFilePath);
    void OnImGui();

    template <class T>
    void ExposeVariableMacro(T* anAddress, const std::string& aLabel, const std::string& anOverrideLabel, const std::string& aCategory, const JsonExposedImguiTag& anImGuiTag = JsonExposedImguiTag::Default);

    void Unexpose(void* anAddres);

private:
    void ExposeVariable(const std::string& aSection, JsonExposedVariable& aVariable);
    void ExposeVariable(void* anAddress, JsonExposedType aType, const std::string& aLabel, const std::string& anOverrideLabel, const std::string& aCategory, const JsonExposedImguiTag& anImGuiTag);
    std::string GetLabelId(const std::string& aLabel);

    void RestoreValue(JsonExposedVariable& aVariable);
    void AlterJsonValue(const std::string& aCategory, JsonExposedVariable& aVariable);
    void LoadJsonValue(const std::string& aCategory, JsonExposedVariable& aVariable);

private:
    std::string SanitizeFileString(const std::string& aFilePath);

private:
    std::map<std::string, std::vector<JsonExposedVariable>> myExposedVariables;

    std::map<     std::string, // [Category]
        std::map< std::string, // [Name]
            size_t // index in myExposedVariables[Category]
        >
    > mySyncedVariableExposeIndexMapper;

    JsonDocument* myJsonDocument;
    std::string myConfigFile;
};

template<class T>
inline void JsonManager::ExposeVariableMacro(T* anAddress, const std::string& aLabel, const std::string& anOverrideLabel, const std::string& aCategory, const JsonExposedImguiTag& anImGuiTag)
{
    JsonExposedTemplate eTemplate = JsonExposedTemplate(anAddress);
    ExposeVariable(reinterpret_cast<void*>(anAddress), eTemplate.GetType(), aLabel, anOverrideLabel, SanitizeFileString(aCategory), anImGuiTag);
}
