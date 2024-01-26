#include "pch.h"
#include "ModelViewer.h"
#include "ModelInstance.h"

#include <filesystem>

namespace SE
{
    void CModelViewer::Update(CModelInstance* aModelInstance)
    {
        (void)aModelInstance;

        if (ImGui::Button("Load"))
        {
            for (const auto& entry : std::filesystem::directory_iterator("Models"))
            {
                //entry.is_directory();

                printf("Entry \"%s\"\n", entry.path().string().c_str());
            }
        }
    }
}
