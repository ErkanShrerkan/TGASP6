#include "pch.h"
#include "Texture.h"
#include "TextureHelper.h"
#include <d3d11.h>

SE::CTexture::CTexture()
	: myShaderResourceView(nullptr)
	, myWidth(0)
	, myHeight(0)
	, myMipLevels(0)
	, myFormat(0u)
{}

SE::CTexture::CTexture(const std::string_view& aFilePath)
	: myShaderResourceView(nullptr)
	, myWidth(0)
	, myHeight(0)
	, myMipLevels(0)
	, myFormat(0u)
{
	Helper::TextureHelper::LoadShaderResourceView(&myShaderResourceView, aFilePath);
	
	if (myShaderResourceView == nullptr)
	{
		/* Error Message */
		perr("Error loading \"%s\" as a CTexture", std::string(aFilePath).c_str());
	}
	else
	{
		// Get size of image
		ID3D11Resource* resource = nullptr;
		myShaderResourceView->GetResource(&resource);
		
		D3D11_TEXTURE2D_DESC description;
		reinterpret_cast<ID3D11Texture2D*>(resource)->GetDesc(&description);

		myWidth = static_cast<float>(description.Width);
		myHeight = static_cast<float>(description.Height);
		myMipLevels = static_cast<float>(description.MipLevels);
		myFormat = static_cast<unsigned>(description.Format);

		if (!(Math::IsPowerOfTwo(description.Width) && Math::IsPowerOfTwo(description.Height)))
		{
			pwarn("Texture is not power of two (%.fx%.f) \"%s\"", myWidth, myHeight, std::string(aFilePath).c_str());
		}
	}
}

SE::CTexture::~CTexture()
{
	if (myShaderResourceView)
		myShaderResourceView->Release();
	myShaderResourceView = nullptr;
}

ID3D11ShaderResourceView* const SE::CTexture::GetShaderResourceView() const noexcept
{
	return myShaderResourceView;// .Raw();
}

ID3D11ShaderResourceView* const* SE::CTexture::GetPointerToShaderResourceView() const
{
	return &myShaderResourceView;
}

void SE::CTexture::SetShaderResourceView(ID3D11ShaderResourceView* const aShaderResourceView)
{
	myShaderResourceView = aShaderResourceView;
}

ID3D11ShaderResourceView* SE::CTexture::StealShaderResource()
{
	ID3D11ShaderResourceView* oldPointer = myShaderResourceView;
	myShaderResourceView = nullptr;

	oldPointer->AddRef();
	return oldPointer;
}
