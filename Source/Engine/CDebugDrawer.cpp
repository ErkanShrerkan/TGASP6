#include "pch.h"
#include "CDebugDrawer.h"
#include <Engine\Engine.h>
#include <d3d11.h>
#include "Input.h"

void CDebugDrawer::Init()
{
	SE::Helper::BufferHelper::CreateBuffer<SphereBufferData>(&mySphereBuffer);
}

void CDebugDrawer::DrawSphere([[maybe_unused]]float3 aPos, [[maybe_unused]] float aRadius, [[maybe_unused]] float4 aColor)
{
#ifdef DEBUG
	if (myIndex < Debug::maxSpheres)
	{
		float4 posAndRadius({ aPos.x, aPos.y, aPos.z, aRadius});
		mySphereBufferData.myPositionsAndRadii[myIndex] = posAndRadius;
		mySphereBufferData.myColors[myIndex] = aColor;
		myIndex++;
	}
#endif // DEBUG
}

void CDebugDrawer::Render()
{
#ifdef DEBUG
	if (Input::GetInputPressed(eButtonInput::Up))
	{
		myShouldRender = !myShouldRender;
	}

	if (!myShouldRender)
	{
		myIndex = 0;
	}

	mySphereBufferData.myNumSpheres = myIndex;
	SE::Helper::BufferHelper::MapBuffer(mySphereBuffer, &mySphereBufferData);
	ID3D11DeviceContext* context = SE::CEngine::GetInstance()->GetDXDeviceContext();
	context->PSSetConstantBuffers(4, 1, &mySphereBuffer);

	myIndex = 0;
#endif // DEBUG
}
