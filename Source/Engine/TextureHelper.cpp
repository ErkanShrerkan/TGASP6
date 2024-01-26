#include "pch.h"
#include "TextureHelper.h"
#include <DirectXTK\DDSTextureLoader.h> // .dds
#include <DirectXTK\WICTextureLoader.h> // .png .jpg etc
#include "Engine.h" // device

namespace SE
{
	namespace Helper
	{
		namespace TextureHelper
		{
			bool LoadShaderResourceView(ID3D11ShaderResourceView** aShaderResourceView, const std::string_view& aFilePath)
			{
				// TODO: Maybe use a CTextureFactory instead
				ID3D11Device* device = CEngine::GetInstance()->GetDXDevice();

				size_t dot = aFilePath.find_last_of('.');
				if (dot == std::string::npos)
				{
					*aShaderResourceView = nullptr;
					return false;
				}
				std::string_view extension = aFilePath.substr(dot);
				std::wstring wFilePath(aFilePath.begin(), aFilePath.end());

				if (extension == ".dds")
				{
					DirectX::CreateDDSTextureFromFile(device, wFilePath.c_str(), nullptr, aShaderResourceView);
				}
				else
				{
					/* Warning, texture is not dds */
					pwarn("Texture \"%s\" is not a .dds", std::string(aFilePath).c_str());
					DirectX::CreateWICTextureFromFile(device, wFilePath.c_str(), nullptr, aShaderResourceView);
				}

				// TODO: Warn about error loading shader resource view

				return *aShaderResourceView != nullptr;
			}
		}
	}
}
