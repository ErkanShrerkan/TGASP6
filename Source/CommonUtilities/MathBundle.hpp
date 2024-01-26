#pragma once
#pragma message("Loading bundle \"CommonUtilities/MathBundle.hpp\". You should only see this once building your project, otherwise something is wrong")
#include "Math/Vector.hpp"
#include "Math/Matrix.hpp"
#include "Math.hpp"

typedef unsigned int uint;

typedef CommonUtilities::Vector2<float>	Vector2f;
typedef CommonUtilities::Vector3<float>	Vector3f;
typedef CommonUtilities::Vector4<float>	Vector4f;
typedef CommonUtilities::Vector2<int>	Vector2i;
typedef CommonUtilities::Vector3<int>	Vector3i;
typedef CommonUtilities::Vector4<int>	Vector4i;
typedef CommonUtilities::Vector2<uint>  Vector2ui;
typedef CommonUtilities::Vector3<uint>  Vector3ui;
typedef CommonUtilities::Vector4<uint>  Vector4ui;

typedef CommonUtilities::Matrix3x3<float> Matrix3x3f;
typedef CommonUtilities::Matrix4x4<float> Matrix4x4f;

typedef float                             float1;
typedef CommonUtilities::Vector2<float>   float2;
typedef CommonUtilities::Vector3<float>   float3;
typedef CommonUtilities::Vector4<float>   float4;
typedef CommonUtilities::Matrix3x3<float> float3x3;
typedef CommonUtilities::Matrix4x4<float> float4x4;

typedef int                           int1;
typedef CommonUtilities::Vector2<int> int2;
typedef CommonUtilities::Vector3<int> int3;
typedef CommonUtilities::Vector4<int> int4;

typedef uint                           uint1;
typedef CommonUtilities::Vector2<uint> uint2;
typedef CommonUtilities::Vector3<uint> uint3;
typedef CommonUtilities::Vector4<uint> uint4;