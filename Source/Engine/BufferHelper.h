#pragma once

struct ID3D11Buffer;
namespace SE
{
    namespace Helper
    {
        namespace BufferHelper
        {
            void MapBuffer(ID3D11Buffer* aBuffer, void* aBufferData, size_t aSizeOfBufferData);
            template <class IBufferData>
            void MapBuffer(ID3D11Buffer* aBuffer, IBufferData* aBufferData)
            {
                MapBuffer(aBuffer, aBufferData, sizeof(IBufferData));
            }

            /// <summary>
            /// </summary>
            /// <param name="aBindFlag">Default is D3D11_BIND_CONSTANT_BUFFER</param>
            void CreateBuffer(ID3D11Buffer** aBuffer, size_t aSizeOfBufferData, int aBindFlag = -1);
            /// <summary>
            /// </summary>
            /// <param name="aBindFlag">If omitted, D3D11_BIND_CONSTANT_BUFFER will be used</param>
            template <class IBufferData>
            void CreateBuffer(ID3D11Buffer** aBuffer, int aBindFlag = -1)
            {
                CreateBuffer(aBuffer, sizeof(IBufferData), aBindFlag);
            }
        }
    }
}
