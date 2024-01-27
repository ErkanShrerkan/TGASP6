#include "pch.h"
#include "TextRenderer.h"
#include <fstream>
#include <Engine\Engine.h>
#include <Engine\Camera.h>
#include "Text.h"
#include "Font.h"
#include <Engine/ShaderHelper.h>

Matrix4x4f dinMamma;

bool TextRenderer::Init()
{
    bool yes = true;

    if (yes)
    {
        return true;
    }
    dinMamma(1, 1) = 2.f;
    dinMamma(2, 2) = 2.f;
    dinMamma(3, 3) = 1.f / (10000 - 1);
    dinMamma(4, 3) = 1.f / (1 - 10000);
    dinMamma(4, 4) = 1.0f;

    HRESULT result;
    auto* device = SE::CEngine::GetInstance()->GetDXDevice();

    std::string vsData;
    ID3D11VertexShader* vertexShader;
    if (!SE::Helper::ShaderHelper::CreateVertexShader(&vertexShader, "Shaders/TextVertexShader", &vsData))
    {
        return false;
    }

    ID3D11PixelShader* pixelShader;
    if (!SE::Helper::ShaderHelper::CreatePixelShader(&pixelShader, "Shaders/TextPixelShader"))
    {
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"COLOR", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"COLOR", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"UV", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"UV", 2, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"UV", 3, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        //{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    ID3D11InputLayout* inputLayout;
    result = device->CreateInputLayout(layout, sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC), vsData.data(), vsData.size(), &inputLayout);
    if (FAILED(result))
    {
        return false;
    }

    D3D11_BUFFER_DESC bufferDesc = { 0 };
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    bufferDesc.ByteWidth = sizeof(FrameBufferData);
    result = device->CreateBuffer(&bufferDesc, nullptr, &myFrameBuffer);
    if (FAILED(result))
    {
        return false;
    }

    bufferDesc.ByteWidth = sizeof(ObjectBufferData);
    result = device->CreateBuffer(&bufferDesc, nullptr, &myObjectBuffer);

    if (FAILED(result))
    {
        return false;
    }

    myVertexShader = vertexShader;
    myPixelShader = pixelShader;
    myInputLayout = inputLayout;

    return true;
}

void TextRenderer::Render(SE::CCamera* aCamera, const std::vector<std::shared_ptr<Text>>& someText)
{
    bool yes = true;

    if (yes)
    {
        return;
    }
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE bufferData;

    myFrameBufferData.myToCamera = Matrix4x4f();// Matrix4x4f::GetFastInverse(aCamera->GetTransform());
    myFrameBufferData.myToProjection = dinMamma;

    float camNearPlane = 0;
    float camFarPlane = 0;
    aCamera->GetProjectionPlanes(camNearPlane, camFarPlane);
    myFrameBufferData.myNearPlane = camNearPlane;
    myFrameBufferData.myFarPlane = camFarPlane;
    myFrameBufferData.myCameraPosition = { 0.f, 0.f, 0.f, 1.f };
    myFrameBufferData.myRenderMode = 0;

    ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));

    auto* context = SE::CEngine::GetInstance()->GetDXDeviceContext();
    result = context->Map(myFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
    if (FAILED(result))
    {
        return;
    }
    memcpy(bufferData.pData, &myFrameBufferData, sizeof(FrameBufferData));
    context->Unmap(myFrameBuffer, 0);

    context->PSSetConstantBuffers(0, 1, &myFrameBuffer);
    context->VSSetConstantBuffers(0, 1, &myFrameBuffer);

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(myInputLayout);

    context->GSSetShader(nullptr, nullptr, 0);
    context->VSSetShader(myVertexShader, nullptr, 0);
    context->PSSetShader(myPixelShader, nullptr, 0);

    for (auto& text : someText)
    {
        Text::TextData textData = text->GetTextData();

        myObjectBufferData.myToWorld = text->GetTransform();

        ZeroMemory(&bufferData, sizeof(D3D11_MAPPED_SUBRESOURCE));
        result = context->Map(myObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
        if (FAILED(result))
        {
            return;
        }

        memcpy(bufferData.pData, &myObjectBufferData, sizeof(ObjectBufferData));
        context->Unmap(myObjectBuffer, 0);
        context->VSSetConstantBuffers(1, 1, &myObjectBuffer);
        context->PSSetConstantBuffers(1, 1, &myObjectBuffer);

        context->IASetVertexBuffers(0, 1, &textData.myVertexBuffer, &textData.myStride, &textData.myOffset);
        context->IASetIndexBuffer(textData.myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

        //context->PSSetConstantBuffers(1, 1, &myObjectBuffer);
        context->PSSetShaderResources(0, 1, &text->GetFont()->mySRV);

        context->DrawIndexed(textData.myNumberOfIndices, 0, 0);
    }
}
