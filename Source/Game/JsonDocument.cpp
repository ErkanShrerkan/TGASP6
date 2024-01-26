#include "pch.h"
#include "JsonDocument.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/writer.h>
#include <rapidjson/pointer.h>
#include <rapidjson/prettywriter.h>

JsonDocument::JsonDocument(const std::string& aJsonFile)
{
    // Load ENTIRE file into RAM before parsing
    std::ifstream file{ std::string(aJsonFile) };
    std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    file.close();

    rapidjson::StringStream stream(content.c_str());
    if (myDocument.ParseStream(stream).HasParseError())
    {
        perr("<%s> Parse error at offset %u: \"%s\", when loading file \"%s\"",
            "JsonDocument::JsonDocument",
            (unsigned)myDocument.GetErrorOffset(),
            rapidjson::GetParseError_En(myDocument.GetParseError()),
            aJsonFile.data()
        );
        assert(!"JSON Parse error");
    }
}

void JsonDocument::SaveToFile(const std::string& aJsonFile, bool aSavePretty)
{
    rapidjson::StringBuffer buffer;
    if (aSavePretty)
    {
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        myDocument.Accept(writer);
    }
    else
    {
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        myDocument.Accept(writer);
    }
   
    std::ofstream file{ std::string(aJsonFile) };
    file << buffer.GetString();
    file.close();
}

void JsonDocument::SetFloat(const std::string& aSource, float aValue)
{
    rapidjson::Pointer(aSource.c_str()).Set(myDocument, aValue);
}

float JsonDocument::GetFloat(const std::string& aSource, float aDefaultValue)
{
    auto jfloat = rapidjson::Pointer(aSource.c_str()).Get(myDocument);
    return jfloat ? jfloat->GetFloat() : aDefaultValue;
}

void JsonDocument::SetBool(const std::string& aSource, bool aValue)
{
    rapidjson::Pointer(aSource.c_str()).Set(myDocument, aValue);
}

bool JsonDocument::GetBool(const std::string& aSource, bool aDefaultValue)
{
    auto jbool = rapidjson::Pointer(aSource.c_str()).Get(myDocument);
    return jbool ? jbool->GetBool() : aDefaultValue;
}
