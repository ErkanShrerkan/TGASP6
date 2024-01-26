#include "pch.h"
#include "DX11.h"
#include "GraphicsEngine.h"

namespace SE
{
    CContentLoader* DX11::Content = nullptr;
    CGraphicsEngine* DX11::Graphics = nullptr;

    Vector2ui DX11::GetResolution()
    {
        return Vector2ui(Graphics->GetWindowHandler().GetWidth(), Graphics->GetWindowHandler().GetHeight());
    }
}