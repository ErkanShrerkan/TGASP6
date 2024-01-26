#include "pch.h"
#include "BufferHelper.h"

#include <Engine/Engine.h>
#pragma comment(lib, "d3d11.lib")
#include <d3d11.h>

namespace SE
{
    namespace Helper
    {
        namespace BufferHelper
        {
            void MapBuffer(ID3D11Buffer* aBuffer, void* aBufferData, size_t aSizeOfBufferData)
            {
                HRESULT result;
                D3D11_MAPPED_SUBRESOURCE bufferData;
                ID3D11DeviceContext* const& context = CEngine::GetInstance()->GetDXDeviceContext();

                ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
                result = context->Map(aBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
                if (FAILED(result))
                {
                    /* Error Message */
                    assert(!"<SE::BufferHelper::MapBuffer> Error");
                    return;
                }
                memcpy(bufferData.pData, aBufferData, aSizeOfBufferData);
                context->Unmap(aBuffer, 0);
            }

            void CreateBuffer(ID3D11Buffer** aBuffer, size_t aSizeOfBufferData, int aBindFlag)
            {
                HRESULT result;
                D3D11_BUFFER_DESC bufferDescription = { 0 };
                bufferDescription.Usage = D3D11_USAGE_DYNAMIC;
                bufferDescription.BindFlags = (aBindFlag == -1 ? D3D11_BIND_CONSTANT_BUFFER : static_cast<D3D11_BIND_FLAG>(aBindFlag));
                bufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                bufferDescription.ByteWidth = static_cast<uint>(aSizeOfBufferData);

                ID3D11Device* const& device = CEngine::GetInstance()->GetDXDevice();
                result = device->CreateBuffer(&bufferDescription, nullptr, aBuffer);
                if (FAILED(result))
                {
                    /* Error message */
                    assert(!"<SE::BufferHelper::CreateBuffer> Error");
                    return;
                }
            }
        }
    }
}