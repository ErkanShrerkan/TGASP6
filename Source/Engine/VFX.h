#pragma once

namespace SE
{
	class CModel;
	class CTexture;
	class CVFX
	{
		friend class CVFXFactory;
		friend class CVFXInstance;
		friend class CVFXRenderer;
	public:
		struct SColorKey
		{
			float r, g, b, a;
			float frame;
		};

		struct SScaleKey
		{
			float x, y, z;
			float frame;
		};

		struct SUVKey
		{
			float u, v;
			float frame;
		};

		struct SData
		{
			CModel* model = nullptr;
			CTexture* texture = nullptr;
			std::vector<SColorKey> colorKeys;
			std::vector<SScaleKey> scaleKeys;
			std::vector<SUVKey> uvKeys;
			int blend = 0;
			float fps;
			float frames;
			float time;
		};

	private:
		CVFX() {};
		std::vector<SData> myData;
		bool myLooping = false;
	};
}
