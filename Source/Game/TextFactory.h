#pragma once
#include "Font.h"
#include <unordered_map>
class Text;
class TextFactory
{
public:
    TextFactory();
    
    std::shared_ptr<Text>  CreateText(const std::string someText, const int someWorldSize);

private:
    std::vector<std::shared_ptr<Text>> myTextObjects;
    std::unordered_map<std::wstring, Font> myFonts;

    struct Vertex
    {
        float x, y, z, w;  // Position
        float r, g, b, a;  // Color
        float u, v;		  // UV Mapping
    };
    
    bool Init();

};

