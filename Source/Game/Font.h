#pragma once
#include <unordered_map>
#include <d3d11.h>
#include <CommonUtilities\Math\Vector4.hpp>
struct Font
{
    struct Atlas
    {
        float mySize;
        int myWidth;
        int myHeight;
        float myEmSize;
        float myLineHeight;
        float myAscender;
        float myDescender;
        float myUnderlineY;
        float myUnderlineThickness;
    }myAtlas;

    struct Glyph
    {
        char myCharacter;
        float myAdvance;
        CommonUtilities::Vector4<float> myPlaneBounds;
        CommonUtilities::Vector4<float> myUVBounds;

    };

    std::unordered_map<unsigned int, Glyph> myGlyphs;
    
    ID3D11ShaderResourceView* mySRV;

    Glyph operator[](char aC)
    {
        for (auto& c : myGlyphs)
        {
            if (c.second.myCharacter == aC)
            {
                return c.second;
            }
        }
        return {};
    }
};