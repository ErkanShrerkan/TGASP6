#pragma once
#include <vector>
#include <Engine\ShaderDefines.h>
#include <Engine\ShaderHelper.h>
#include <Engine\BufferHelper.h>

namespace Debug
{
	constexpr int maxSpheres = 128;
}

class CDebugDrawer
{
	struct SSphere
	{
		float3 position;
		float radius;
		float4 color;
	};

public:
	void Init();
	void DrawSphere(float3 aPos, float aRadius, float4 aColor);
	void Render();
	//static int GetNumSpheres() { return myIndex; }
	//static std::vector<SSphere> GetSpheres() { return myDebugSpheres; }

	static CDebugDrawer& GetInstance()
	{
		static CDebugDrawer instance;
		return instance;
	}

private:

	CDebugDrawer(){}
	struct SphereBufferData
	{
		float4 myPositionsAndRadii[Debug::maxSpheres];
		float4 myColors[Debug::maxSpheres];
		int myNumSpheres;
		float3 trash;
	} mySphereBufferData;

	ID3D11Buffer* mySphereBuffer = nullptr;
	
	int myIndex = 0;
	bool myShouldRender = false;

	std::vector<SSphere> myDebugSpheres;
};

