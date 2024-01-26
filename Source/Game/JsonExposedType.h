#pragma once
enum class JsonExposedType : unsigned char
{
    None,

    Float1,
    Float2,
    Float3,
    Float4,
    Bool,

    // Keep last, don't delete
    Count
};
