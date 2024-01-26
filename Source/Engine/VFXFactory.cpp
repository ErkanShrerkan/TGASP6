#include "pch.h"
#include "VFXFactory.h"
#include "VFX.h"
#include <fstream>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/pointer.h>
#include <Engine/DX11.h>
#include <Engine/ContentLoader.h>
#include <Engine\TextureFactory.h>
#include <Engine\TextureHelper.h>
#include <Engine\Texture.h>

namespace SE
{

	CVFX* CVFXFactory::GetVFX(std::string aVFX)
	{
		std::string path = "VFX/";
		path += aVFX;
		path += ".txt";
		return (myVFXs.find(aVFX) == myVFXs.end()) ? CreateVFX(path) : LoadVFX(aVFX);
	}

	CVFX* CVFXFactory::CreateVFX(std::string aVFX)
	{
		CVFX* vfx = new CVFX();
		std::ifstream t(aVFX, std::ios::in | std::ios::binary);
		std::stringstream buffer;
		buffer << t.rdbuf();
		t.close();
		std::string jsonData = buffer.str();
		rapidjson::StringStream stream(jsonData.c_str());
		rapidjson::Document doc;
		doc.ParseStream(stream).HasParseError();

		if (doc.HasMember("Loop"))
		{
			vfx->myLooping = doc["Loop"].GetInt();
		}

		for (auto& json : doc["Components"].GetArray())
		{
			CVFX::SData data;
			data.model = DX11::Content->GetModelFactory().GetModel(json["Model"].GetString());
			data.texture = DX11::Content->GetTextureFactory().LoadTexture(json["Texture"].GetString());
			data.fps = json["FPS"].GetFloat();
			data.frames = json["Frames"].GetFloat();
			data.time = data.frames / data.fps;
			if (json.HasMember("Blend"))
			{
				data.blend = json["Blend"].GetInt();
			}
			for (auto& colorKey : json["ColorKeys"].GetArray())
			{
				CVFX::SColorKey ck;
				ck.frame = colorKey["Frame"].GetFloat();
				ck.r = colorKey["Value"].GetArray()[0].GetFloat();
				ck.g = colorKey["Value"].GetArray()[1].GetFloat();
				ck.b = colorKey["Value"].GetArray()[2].GetFloat();
				ck.a = colorKey["Value"].GetArray()[3].GetFloat();
				data.colorKeys.push_back(ck);
			}
			for (auto& scaleKey : json["ScaleKeys"].GetArray())
			{
				CVFX::SScaleKey sk;
				sk.frame = scaleKey["Frame"].GetFloat();
				sk.x = scaleKey["Value"].GetArray()[0].GetFloat();
				sk.y = scaleKey["Value"].GetArray()[1].GetFloat();
				sk.z = scaleKey["Value"].GetArray()[2].GetFloat();
				data.scaleKeys.push_back(sk);
			}
			for (auto& colorKey : json["UVKeys"].GetArray())
			{
				CVFX::SUVKey uvk;
				uvk.frame = colorKey["Frame"].GetFloat();
				uvk.u = colorKey["Value"].GetArray()[0].GetFloat();
				uvk.v = colorKey["Value"].GetArray()[1].GetFloat();
				data.uvKeys.push_back(uvk);
			}

			vfx->myData.push_back(data);
		}

		//myVFXs[doc["Name"].GetString()] = vfx;
		return vfx;
	}

	CVFX* CVFXFactory::LoadVFX(std::string aVFX)
	{
		return myVFXs[aVFX];
	}

}