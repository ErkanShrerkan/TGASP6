#include "pch.h"
#include "CameraFactory.h"
#include "Camera.h"
namespace SE
{
    CCamera* CCameraFactory::CreateCamera()
    {
        return new CCamera();
    }
}
