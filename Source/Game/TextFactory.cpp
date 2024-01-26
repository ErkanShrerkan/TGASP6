#include "pch.h"
#include "TextFactory.h"
#include <fstream>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <Engine\TextureHelper.h>
#include "Text.h"
#include <Engine\Engine.h>
TextFactory::TextFactory()
{
    Init();
}

std::shared_ptr<Text> TextFactory::CreateText(const std::string someText, const int someWorldSize)
{
    const std::string asciiString = std::string(someText.begin(), someText.end());
    //const size_t strLen = strlen(asciiString.c_str());

    std::wstring fontName = L"msdf";
    auto it = myFonts.find(fontName);
    Font& font = it->second;

    std::vector<Vertex> vertices;
    std::vector<UINT> indices;

    float x = 0;
    float actualWorldSize = static_cast<float>(someWorldSize) / 1.f;
    //float actualWorldSize = static_cast<float>(font.myAtlas.mySize);

    for (auto c : asciiString)
    {
        const float charAdvance = font[c].myAdvance * actualWorldSize;
        const unsigned int currentVertexCount = static_cast<unsigned>(vertices.size());

        const Vector4f bounds = font[c].myUVBounds;
        Vector4f offsets = font[c].myPlaneBounds;

        offsets.x = font[c].myPlaneBounds.x * charAdvance;
        offsets.y = font[c].myPlaneBounds.y * charAdvance;
        offsets.z = font[c].myPlaneBounds.z * actualWorldSize * 0.65f;
        offsets.w = font[c].myPlaneBounds.w * actualWorldSize;

        if (abs(offsets.w) > 0.005f)
        {
            offsets.w += font.myAtlas.myDescender * actualWorldSize;
        }

        //Bottomleft
        vertices.push_back({
            x + offsets.x, offsets.y, 0,1,
            1, 1, 1, 1,
            bounds.x, 1 - bounds.w
            });
        //Topleft
        vertices.push_back({
            x + offsets.x, offsets.w, 0,1,
            1, 1, 1, 1,
            bounds.x, 1 - bounds.y
            });
        //Bottomright
        vertices.push_back({
            x + offsets.z, offsets.y, 0, 1,
            1, 1, 1, 1,
            bounds.z, 1 - bounds.w
            });
        //Topright
        vertices.push_back({
            x + offsets.z, offsets.w, 0, 1,
            1, 1, 1, 1,
            bounds.z, 1 - bounds.y
            });

        x += charAdvance * 0.65f;

        indices.push_back(currentVertexCount);
        indices.push_back(currentVertexCount + 1);
        indices.push_back(currentVertexCount + 2);
        indices.push_back(currentVertexCount + 1);
        indices.push_back(currentVertexCount + 3);
        indices.push_back(currentVertexCount + 2);
    }

    HRESULT result;
    D3D11_BUFFER_DESC vertexBufferDescription = { 0 };
    //vertexBufferDescription.ByteWidth = sizeof(vertices);
    vertexBufferDescription.ByteWidth = sizeof(Vertex) * static_cast<UINT>(vertices.size());
    vertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA vertexSubresourceData = { 0 };
    vertexSubresourceData.pSysMem = vertices.data();

    auto* device = SE::CEngine::GetInstance()->GetDXDevice();
    ID3D11Buffer* vertexBuffer;
    result = device->CreateBuffer(&vertexBufferDescription, &vertexSubresourceData, &vertexBuffer);
    if (FAILED(result))
    {
        /* Error Message */
        assert(!"<TextFactriru::Textttxtttty::TEXt> Error creating VertexBuffer");
    }

    D3D11_BUFFER_DESC indexBufferDescription = { 0 };
    indexBufferDescription.ByteWidth = sizeof(UINT) * static_cast<UINT>(indices.size());
    indexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA indexSubresourceData = { 0 };
    indexSubresourceData.pSysMem = indices.data();

    ID3D11Buffer* indexBuffer;
    result = device->CreateBuffer(&indexBufferDescription, &indexSubresourceData, &indexBuffer);
    if (FAILED(result))
    {
        assert(!"<SE::TextFactriru::Textttxtttt> Error creating IndexBuffer");

    }
    // End Vertex

    Text::TextData textData;
    textData.myNumberOfVertices = static_cast<UINT>(vertices.size());
    textData.myNumberOfIndices = static_cast<UINT>(indices.size());
    textData.myStride = sizeof(Vertex);
    textData.myOffset = 0;
    textData.myVertexBuffer = vertexBuffer;
    textData.myIndexBuffer = indexBuffer;
    textData.myPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    std::shared_ptr<Text> text = std::make_shared<Text>();
    //font borde tappa referensen när den går out of scope???
    std::wstring wtext = std::wstring(someText.begin(), someText.end());
    text->Init(textData, wtext, &font);
    return text;
}

bool TextFactory::Init()
{
    const std::string fontFileName = "Textures/Fonts/msdf";
    const std::string atlasFileName = fontFileName + ".dds";
    const std::string fontDefinition = fontFileName + ".json";
    
    //Json
    // Load ENTIRE file into RAM before parsing
    std::ifstream file{ std::string(fontDefinition) };
    std::string content{ std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
    file.close();
    
    rapidjson::StringStream stream(content.c_str());
    rapidjson::Document document;
    document.ParseStream(stream);
    
    Font font;
    
    font.myAtlas.mySize = document["atlas"]["size"].GetFloat();
    font.myAtlas.myWidth = document["atlas"]["width"].GetInt();
    font.myAtlas.myHeight = document["atlas"]["height"].GetInt();
    font.myAtlas.myEmSize = document["metrics"]["emSize"].GetFloat();
    font.myAtlas.myLineHeight = document["metrics"]["lineHeight"].GetFloat();
    font.myAtlas.myAscender = document["metrics"]["ascender"].GetFloat();
    font.myAtlas.myDescender = document["metrics"]["descender"].GetFloat();

    size_t glyphCount = static_cast<size_t>(document["glyphs"].GetArray().Size());
    for (unsigned int g = 0; g < glyphCount; g++)
    {
        unsigned int unicode = document["glyphs"][g]["unicode"].GetUint();
        float advance = document["glyphs"][g]["advance"].GetFloat();
    
        Vector4f planeBounds = Vector4f(0.f, 0.f, 0.f, 0.f);
        if (document["glyphs"][g].HasMember("planeBounds"))
        {
            planeBounds.x = document["glyphs"][g]["planeBounds"]["left"].GetFloat();
            planeBounds.y = document["glyphs"][g]["planeBounds"]["bottom"].GetFloat();
            planeBounds.z = document["glyphs"][g]["planeBounds"]["right"].GetFloat();
            planeBounds.w = document["glyphs"][g]["planeBounds"]["top"].GetFloat();
        }

        Vector4f uvBounds = Vector4f(0, 0, 0, 0);
        if (document["glyphs"][g].HasMember("atlasBounds"))
        {
            const float UVStartX = static_cast<float>(document["glyphs"][g]["atlasBounds"]["left"].GetFloat() / static_cast<float>(font.myAtlas.myWidth));
            const float UVStartY = static_cast<float>(document["glyphs"][g]["atlasBounds"]["top"].GetFloat() / static_cast<float>(font.myAtlas.myHeight));
            const float UVEndX = static_cast<float>(document["glyphs"][g]["atlasBounds"]["right"].GetFloat() / static_cast<float>(font.myAtlas.myWidth));
            const float UVEndY = static_cast<float>(document["glyphs"][g]["atlasBounds"]["bottom"].GetFloat() / static_cast<float>(font.myAtlas.myHeight));

            uvBounds = { UVStartX, UVStartY, UVEndX, UVEndY };
        }

        font.myGlyphs.insert(
            {
                unicode,
            {
                static_cast<char>(unicode),
                advance,
                planeBounds,
                uvBounds
}
            }
        );
    }

    ID3D11ShaderResourceView* fontSRV;
    if (SE::Helper::TextureHelper::LoadShaderResourceView(&fontSRV, "Textures/Fonts/msdf.dds"))
    {
        font.mySRV = fontSRV;
        myFonts.insert({ L"msdf", font });

        return true;
    }
    return false;  
}
