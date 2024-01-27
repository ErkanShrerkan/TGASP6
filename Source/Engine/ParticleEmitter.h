#pragma once
#include <string>
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>

//spritesheet data
//storlek på sprite
//hur många frames
//
//additive / alpha(kolla med Eric)
//
//gravity force
//
//spawnsphere radius

#include <CommonUtilities/Math.hpp>

struct ID3D11ShaderResourceView;
namespace SE
{
    class CParticleEmitter
    {
        friend class CParticleFactory;
    private:
        std::string myPath;

    public:
        template <class T>
        struct SMinMaxCurrent
        {
            T minimum, maximum;
            SMinMaxCurrent() = default;
            SMinMaxCurrent(const T& aValue)
                : minimum(aValue)
                , maximum(aValue)
            {}
            T Random() const
            {
                float rng = static_cast<float>(rand() % 10000) * 0.0001f;
                return Math::Lerp(minimum, maximum, rng);
            }
        };
        struct SParticleSettings
        {
            float spawnRate            =   5.0f;
            float spawnRadius          =   0.0f;
            struct
            {
                float lifeTime                         =   1.0f;
                SMinMaxCurrent<float> speed            =   5.0f;
                SMinMaxCurrent<float> startSize        =   100.0f;
                SMinMaxCurrent<float> endSize          =   500.0f;
                SMinMaxCurrent<float> emissiveStrength =   10.0f;
                SMinMaxCurrent<float> gravity          =   0.0f;
                SMinMaxCurrent<float3> direction       = float3{ 0.0f, 1.0f, 0.0f };
                SMinMaxCurrent<float4> startColor      = float4{ 1.0f, 1.0f, 1.0f, 1.0f };
                SMinMaxCurrent<float4> endColor        = float4{ 0.9f, 1.0f, 1.0f, 0.0f };
            }
            particle;
            struct
            {
                float size = 1.0f;
                float amount = 0.0f;
                float interval = 0.0f;
                bool isAnimated = false;
                bool startWithRandomIndex = false;
            }
            spriteSheet;
        };

        struct SParticleData
        {
            uint numberOfParticles = 0;
            uint stride = 0;
            uint offset = 0;
            ID3D11Buffer* particleVertexBuffer = nullptr;
            ID3D11VertexShader* vertexShader = nullptr;
            ID3D11GeometryShader* geometryShader = nullptr;
            ID3D11PixelShader* pixelShader = nullptr;
            D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = {};
            ID3D11InputLayout* inputLayout = nullptr;
            ID3D11ShaderResourceView* shaderResourveView = nullptr;
        };

        struct SParticleVertex
        {
            // == On GPU side
            float4 position;
            float4 velocity;
            float4 color;
            float2 size; // x = size | y = spritesheetsize in normalized value
            float horizontalOffset = 0.0f;
            float distanceToCamera = 0.0f;
            float emissiveStrength = 1.0f;
            // === Only on CPU side
            float4 startColor;
            float4 endColor;
            float4 offset;
            float endSize;
            float startSize;
            float gravity;
            float lifeTime = -1.0f;
            float spriteSheetTime = 0.0f;
        };

    private:
        SParticleData myData;
        SParticleSettings mySettings;

    public:
        ~CParticleEmitter();
        __forceinline const SParticleSettings& GetSettings() const
        {
            return mySettings;
        }
        __forceinline const SParticleData& GetData() const
        {
            return myData;
        }
        __forceinline void SetSettings(const SParticleSettings& someSettings)
        {
            mySettings = someSettings;
        }
        __forceinline void SwitchShaderResourceView(ID3D11ShaderResourceView* const aShaderResourceView)
        {
            myData.shaderResourveView = aShaderResourceView;
        }
    };
}
