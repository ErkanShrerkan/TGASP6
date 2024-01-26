#pragma once
namespace SE
{
    class CContentLoader;
    class CGraphicsEngine;
    struct DX11
    {
        static CContentLoader* Content;
        static CGraphicsEngine* Graphics;

        static Vector2ui GetResolution();
    };
}
