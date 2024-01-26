
static const float GaussianKernel5[5] =
{
    0.06136, 0.24477, 0.38774, 0.24477, 0.06136
};

struct VertexInput_Fullscreen
{
    unsigned int myIndex : SV_VertexID;
    unsigned int myID : SV_InstanceID;
    float4 myPosition : POSITION;
    float2 myUV : UV;
};

struct VertexToPixel_Fullscreen
{
    float4 myPosition : SV_POSITION;
    float2 myUV : UV;
};

struct PixelOutput_Fullscreen
{
    float4 myColor : SV_TARGET;
};

struct VertexInput_GBuffer
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float3 myNormal : NORMAL;
    float3 myTangent : TANGENT;
    float3 myBinormal : BINORMAL;
    uint4 myBoneIDs : BONEIDS;
    float4 myBoneWeights : BONEWEIGHTS;
};

struct VertexToPixel_GBuffer
{
    float4 myPosition : SV_POSITION;
    float4 myWorldPosition : WORLD_POSITION;
    float4 myColor : COLOR;
    float3 myNormal : NORMAL;
    float3 myTangent : TANGENT;
    float3 myBinormal : BINORMAL;
    float2 myUV : UV;
    float myDepth : DEPTH;
};

struct PixelOutput_GBuffer
{
    float4 myColor : SV_TARGET;
};

struct VertexInput_Sprite
{
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
};

struct VertexToPixel_Sprite
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float2 myRectUV : RECTUV;
};

struct PixelOutput_Sprite
{
    float4 myColor : SV_TARGET;
};

struct VertexInput_Particle
{
    float4 myPosition : POSITION;
    float4 myVeocity : VELOCITY;
    float4 myColor : COLOR;
    float2 mySize : SIZE;
    float myHorizontalOffset : OFFSET;
    float myDistanceToCamera : DISTANCE;
    float myEmissiveStrength : EMISSIVE;
};

struct VertexToGeometry_Particle
{
    float4x4 myProjection : CAMERA;
    float4 myPosition : POSITION;
    float4 myColor : COLOR;
    float2 mySize : SIZE;
    float myEmissiveStrength : EMISSIVE;
    float myHorizontalOffset : OFFSET;
};

struct GeometryToPixel_Particle
{
    float4 myPosition : SV_POSITION;
    float4 myColor : COLOR;
    float2 myUV : UV;
    float myEmissiveStrength : EMISSIVE;
    // float2 myRectUV : RECTUV;
};

cbuffer FrameBuffer_Deferred : register(b0)
{
    float4x4 FBD_cameraTransform;
    float4x4 FBD_toCamera;
    float4x4 FBD_toProjection;
    float4 FBD_cameraPosition;
}

cbuffer ObjectBuffer_Deferred : register(b1)
{
    float4x4 OBD_toWorld;
    float4 OBD_scale;
    uint2 OBD_UVScale;
    unsigned int OBD_HasBones;
    unsigned int OBD_NumBones;
    float4x4 OBD_Bones[128];
    float4 OBD_color;
}

cbuffer ObjectBuffer_Effect : register(b1)
{
    float4x4 OBE_toWorld;
    float4 OBE_color;
    float3 OBE_scale;
    int OBE_blend;
    float2 OBE_uv;
    float2 OBE_trash2;
}

cbuffer LightBuffer_Deferred : register(b2)
{
    float4 LBD_myPosition;
    float4 LBD_myColor;
    float4 LBD_myDirection;
    float LBD_myRange;
    float LBD_myInnerAngle;
    float LBD_myOuterAngle;
    float LBD_mySign;
}

cbuffer PostProcessingBuffer_Fullscreen : register(b3)
{
    float3 PPBD_shallowWaterColor;
    float PPBD_waterLevel;
    float3 PPBD_deepWaterColor;
    float PPBD_depthMult;
    float3 PPBD_waterBorderColor;
    float PPBD_alphaMult;
    uint2 PPBD_fullScreenTextureSize;
    uint2 PPBD_resolution;
    float PPBD_borderThreshhold;
    float PPBD_TimePI;
    float PPBD_DeltaTime;
    float PPBD_fogDensity;
    float3 PPBD_fogColor;
    float PPBD_fogMult;
    float PPBD_fogOffset;
    float PPBD_godRaySampleDistance;
    float PPBD_godRayFalloff;
    float PPBD_alphaThreshold;
}

cbuffer SpriteObjectBuffer_Sprite : register(b0)
{
    float4 mySpriteColor;
    float4 mySpriteRect;
    float2 mySpritePosition;
    float2 mySpriteSize;
    float2 mySpritePivot;
    float mySpriteRotation;
    float mySpriteData;
    int mySpriteIsGamma;
    int3 mySpriteTrash;
}

cbuffer DebugSphereBuffer_Debug : register(b4)
{
    float4 DSBD_myPositionsAndRadii[128];
    float4 DSBD_myColors[128];
    int DSBD_myNumSpheres;
    float3 DSBD_myTrash;
}

cbuffer FrameBuffer_Shadow : register(b5)
{
    float4x4 FBS_cameraTransform;
    float4x4 FBS_toCamera;
    float4x4 FBS_toProjection;
    float4 FBS_cameraPosition;
}

// effect textures
Texture2D FullscreenTexture : register(t0);
Texture2D FullscreenTexture2 : register(t1);

// pbr textures deferred
Texture2D positionTexture : register(t2);
Texture2D albedoTexture : register(t3);
Texture2D normalTexture : register(t4);
Texture2D vertexNormalTexture : register(t5);
Texture2D materialTexture : register(t6);
Texture2D ambientOcclusionTexture : register(t7);
Texture2D depthTexture : register(t8);

// gbuffer textures
Texture2D objAlbedoTexture : register(t9);
Texture2D objNormalTexture : register(t10);
Texture2D objMaterialTexture : register(t11);

// cubemap
TextureCube environmentTexture : register(t12);

// second set of pbr textures
Texture2D positionTexture2 : register(t13);
Texture2D albedoTexture2 : register(t14);
Texture2D normalTexture2 : register(t15);
Texture2D vertexNormalTexture2 : register(t16);
Texture2D materialTexture2 : register(t17);
Texture2D ambientOcclusionTexture2 : register(t18);
Texture2D depthTexture2 : register(t19);

// sprite textures
Texture2D spriteTexture : register(t0);
Texture2D maskTexture : register(t1);

// vfx texture
Texture2D vfxTexture : register(t0);

// shadow textures
Texture2D shadowTexture0 : register(t20);
Texture2D shadowTexture1 : register(t21);
Texture2D shadowTexture2 : register(t22);
Texture2D shadowTexture3 : register(t23);
Texture2D shadowTexture4 : register(t24);
Texture2D shadowTexture5 : register(t25);
Texture2D shadowTexture6 : register(t26);
Texture2D shadowTexture7 : register(t27);
Texture2D shadowTexture8 : register(t28);

// particle textures
Texture2D particleTexture : register(t29);

// samplers
SamplerState defaultSampler : register(s0);
SamplerState wrapSampler : register(s1);