#include "pch.h"
#include "AutoReleaser.h"
#include <Unknwnbase.h>

void SE::Impl::AutoReleaser::Release(HiddenIUnknown* aResource)
{
    reinterpret_cast<IUnknown*>(aResource)->Release();
}
