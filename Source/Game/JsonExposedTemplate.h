#pragma once
#include <CommonUtilities\MathBundle.hpp>
#include "JsonExposedType.h"

template <class T>
struct JsonExposedTemplate
{
    T* myVariable;
    JsonExposedTemplate(T* aVariable)
        : myVariable(aVariable)
    {};
    JsonExposedType GetType()
    {
        return JsonExposedType::None;
    }
};

#define JXT_LINK_TYPE_WITH_ENUM(jxt_type, jxt_enum) template <> \
inline JsonExposedType JsonExposedTemplate<jxt_type>::GetType() { return jxt_enum; }

JXT_LINK_TYPE_WITH_ENUM(float1, JsonExposedType::Float1)
JXT_LINK_TYPE_WITH_ENUM(float2, JsonExposedType::Float2)
JXT_LINK_TYPE_WITH_ENUM(float3, JsonExposedType::Float3)
JXT_LINK_TYPE_WITH_ENUM(float4, JsonExposedType::Float4)
JXT_LINK_TYPE_WITH_ENUM(bool,   JsonExposedType::Bool)
