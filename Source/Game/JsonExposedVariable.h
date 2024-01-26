#pragma once
#include <string>
#include <CommonUtilities\MathBundle.hpp>
#include <set>
#include "JsonExposedType.h"

enum class JsonExposedImguiTag : unsigned char
{
    Default,
    ColorPicker
};

struct JsonExposedVariable
{
    std::set<void*> addresses;
    void* address;
    JsonExposedType type;
    JsonExposedImguiTag imguiTag;
    bool valueHasChanged = false;
    std::string labelId;
    std::string labelText;

    struct
    {
        union
        {
            bool asBool = false;
            float1 asFloat1;
            float2 asFloat2;
            float3 asFloat3;
            float4 asFloat4;
        };
    }
    savedValue;
};