#pragma once
#include <string>
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;
namespace SE
{
    namespace Helper
    {
        namespace ShaderHelper
        {
            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath);
            bool CreateVertexShader(ID3D11VertexShader** aVertexShader, const std::string& aFilePath, std::string* const& anOutData);
            bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath);
            bool CreatePixelShader(ID3D11PixelShader** aPixelShader, const std::string& aFilePath, std::string* const& anOutData);
            bool CreateGeometryShader(ID3D11GeometryShader** aGeometryShader, const std::string& aFilePath);
            bool CreateGeometryShader(ID3D11GeometryShader** aGeometryShader, const std::string& aFilePath, std::string* const& anOutData);
        }
    }
}
