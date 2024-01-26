#include "pch.h"
#include "ParticleFactory.h"
#include "ParticleEmitter.h"
#include "ShaderHelper.h"
#include "BufferHelper.h"
#include "Engine.h"
#include "ContentLoader.h"
#include "Texture.h"
#include <rapidjson/document.h>
#include <rapidjson/stream.h>
#include <fstream>
#include <rapidjson/pointer.h>

namespace SE
{
    void CParticleFactory::InitFactory()
    {
        HRESULT result;
        auto& device = CEngine::GetInstance()->GetDXDevice();

        std::string vsData;
        Helper::ShaderHelper::CreateVertexShader(&myVertexShader, "Shaders/ParticleVertexShader", &vsData);
        Helper::ShaderHelper::CreateGeometryShader(&myGeometryShader, "Shaders/ParticleGeometryShader");
        Helper::ShaderHelper::CreatePixelShader(&myPixelShader, "Shaders/ParticlePixelShader");

        D3D11_INPUT_ELEMENT_DESC layout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "VELOCITY", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "SIZE",     0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "OFFSET",   0, DXGI_FORMAT_R32_FLOAT,          0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "DISTANCE", 0, DXGI_FORMAT_R32_FLOAT,          0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "EMISSIVE", 0, DXGI_FORMAT_R32_FLOAT,          0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &myInputLayout);
        if (FAILED(result))
        {
            /* Error Message */
            assert(false && "Error creating input layout");
        }

        myHasInited = true;
    }

    CParticleEmitter* CParticleFactory::GetParticleEmitter(const std::string& aPath)
    {
        return myPool.Get(aPath, [this, aPath](const std::string&) -> CParticleEmitter* {
            return CreateParticleEmitter(aPath);
        });
    }

#define JSON_POINTER_GET(source) \
rapidjson::Pointer((source).c_str()).Get(document)
#define JSON_POINTER_GET_FLOAT(source) \
( JSON_POINTER_GET(source) ? JSON_POINTER_GET(source)->GetFloat() : 0.0f )
#define JSON_POINTER_GET_BOOL(source) \
( JSON_POINTER_GET(source) ? JSON_POINTER_GET(source)->GetBool() : false )
#define JSON_POINTER_SET_FLOAT(source, var) \
var = JSON_POINTER_GET_FLOAT(source)
#define JSON_POINTER_SET_BOOL(source, var) \
var = JSON_POINTER_GET_BOOL(source)

    CParticleEmitter* CParticleFactory::CreateParticleEmitter(const std::string& aName)
    {
        if (!myHasInited)
            InitFactory();

        CParticleEmitter* emitter = new CParticleEmitter();

        // Load ENTIRE file into RAM before parsing
        std::ifstream file{ std::string("Data/Particles.json") };
        std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
        file.close();
        rapidjson::StringStream stream(content.c_str());
        rapidjson::Document document;
        if (document.ParseStream(stream).HasParseError())
        {
            assert(!"JSON Parse error");
        }
        std::string source = "/Particles/";
        source += aName;
        source += "/";
        auto& settings = emitter->mySettings;
        JSON_POINTER_SET_FLOAT(source + "SpawnRate",                  settings.spawnRate);
        JSON_POINTER_SET_FLOAT(source + "SpawnRadius",                settings.spawnRadius);
        JSON_POINTER_SET_FLOAT(source + "Lifetime",                   settings.particle.lifeTime);
        JSON_POINTER_SET_FLOAT(source + "Speed/Min",                  settings.particle.speed.minimum);
        JSON_POINTER_SET_FLOAT(source + "Speed/Max",                  settings.particle.speed.maximum);
        JSON_POINTER_SET_FLOAT(source + "Size/Start/Min",             settings.particle.startSize.minimum);
        JSON_POINTER_SET_FLOAT(source + "Size/Start/Max",             settings.particle.startSize.maximum);
        JSON_POINTER_SET_FLOAT(source + "Size/End/Min",               settings.particle.endSize.minimum);
        JSON_POINTER_SET_FLOAT(source + "Size/End/Max",               settings.particle.endSize.maximum);
        JSON_POINTER_SET_FLOAT(source + "Emissive/Min",               settings.particle.emissiveStrength.minimum);
        JSON_POINTER_SET_FLOAT(source + "Emissive/Max",               settings.particle.emissiveStrength.maximum);
        JSON_POINTER_SET_FLOAT(source + "Gravity/Min",                settings.particle.gravity.minimum);
        JSON_POINTER_SET_FLOAT(source + "Gravity/Max",                settings.particle.gravity.maximum);
        JSON_POINTER_SET_FLOAT(source + "Direction/Min/x",            settings.particle.direction.minimum.x);
        JSON_POINTER_SET_FLOAT(source + "Direction/Max/x",            settings.particle.direction.maximum.x);
        JSON_POINTER_SET_FLOAT(source + "Direction/Min/y",            settings.particle.direction.minimum.y);
        JSON_POINTER_SET_FLOAT(source + "Direction/Max/y",            settings.particle.direction.maximum.y);
        JSON_POINTER_SET_FLOAT(source + "Direction/Min/z",            settings.particle.direction.minimum.z);
        JSON_POINTER_SET_FLOAT(source + "Direction/Max/z",            settings.particle.direction.maximum.z);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Min/x",          settings.particle.startColor.minimum.x);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Max/x",          settings.particle.startColor.maximum.x);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Min/y",          settings.particle.startColor.minimum.y);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Max/y",          settings.particle.startColor.maximum.y);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Min/z",          settings.particle.startColor.minimum.z);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Max/z",          settings.particle.startColor.maximum.z);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Min/w",          settings.particle.startColor.minimum.w);
        JSON_POINTER_SET_FLOAT(source + "Color/Start/Max/w",          settings.particle.startColor.maximum.w);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Min/x",            settings.particle.endColor.minimum.x);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Max/x",            settings.particle.endColor.maximum.x);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Min/y",            settings.particle.endColor.minimum.y);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Max/y",            settings.particle.endColor.maximum.y);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Min/z",            settings.particle.endColor.minimum.z);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Max/z",            settings.particle.endColor.maximum.z);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Min/w",            settings.particle.endColor.minimum.w);
        JSON_POINTER_SET_FLOAT(source + "Color/End/Max/w",            settings.particle.endColor.maximum.w);
        // Sprite
        JSON_POINTER_SET_FLOAT(source + "Sprite/Size",                settings.spriteSheet.size);
        JSON_POINTER_SET_FLOAT(source + "Sprite/Amount",              settings.spriteSheet.amount);
        JSON_POINTER_SET_FLOAT(source + "Sprite/Interval",            settings.spriteSheet.interval);
        JSON_POINTER_SET_BOOL(source +  "Sprite/IsAnimated",           settings.spriteSheet.isAnimated);
        JSON_POINTER_SET_BOOL(source +  "Sprite/StartWithRandomIndex", settings.spriteSheet.startWithRandomIndex);
        if (settings.spriteSheet.size < 0.000001f)
            settings.spriteSheet.size = 1.0f;

        ID3D11Buffer* vertexBuffer;
        float maximumAmountOfParticles = max(1.0f, emitter->mySettings.particle.lifeTime) * max(1.0f, emitter->mySettings.spawnRate);
        Helper::BufferHelper::CreateBuffer(
            &vertexBuffer,
            static_cast<size_t>(maximumAmountOfParticles) * sizeof(CParticleEmitter::SParticleVertex),
            D3D11_BIND_VERTEX_BUFFER
        );

        auto* jsonPath = rapidjson::Pointer((source + "Sprite/Path").c_str()).Get(document);
        std::string spritePath = jsonPath ? jsonPath->GetString() : "";
        if (spritePath.empty())
            spritePath = "Textures/Error/Albedo_c.dds";

        CTexture* texture = SE::CEngine::GetInstance()->GetContentLoader()->GetTextureFactory().LoadTexture(spritePath);
        CParticleEmitter::SParticleData data;
        data.geometryShader = myGeometryShader;
        data.vertexShader = myVertexShader;
        data.pixelShader = myPixelShader;
        data.inputLayout = myInputLayout;
        data.shaderResourveView = texture->GetShaderResourceView();
        data.numberOfParticles = static_cast<uint>(maximumAmountOfParticles);
        data.particleVertexBuffer = vertexBuffer;
        data.offset = 0;
        data.primitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        data.stride = sizeof(CParticleEmitter::SParticleVertex);

        emitter->myData = data;
        emitter->myPath = aName;

        return emitter;
    }
}
