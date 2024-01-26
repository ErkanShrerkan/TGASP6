#pragma once
#include <string_view>
struct ID3D11ShaderResourceView;
namespace SE
{
    namespace Helper
    {
        namespace TextureHelper
        {
            bool LoadShaderResourceView(ID3D11ShaderResourceView** aShaderResourceView, const std::string_view& aFilePath);
        }
    }
}
