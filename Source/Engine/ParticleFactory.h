#pragma once
#include <CommonUtilities/Container/KeyedPool.hpp>
#include <string>

struct ID3D11VertexShader;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
namespace SE
{
    class CParticleEmitter;
    class CParticleFactory
    {
    private:
        CommonUtilities::KeyedPool<std::string, CParticleEmitter> myPool;
        ID3D11VertexShader* myVertexShader;
        ID3D11GeometryShader* myGeometryShader;
        ID3D11PixelShader* myPixelShader;
        ID3D11InputLayout* myInputLayout;
        bool myHasInited = false;

    private:
        void InitFactory();

    public:
        CParticleEmitter* GetParticleEmitter(const std::string& aPath);

    private:
        CParticleEmitter* CreateParticleEmitter(const std::string& aPath);
    };
}
