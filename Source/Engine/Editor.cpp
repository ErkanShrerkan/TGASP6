#include "pch.h"
#include "Editor.h"
namespace SE
{
	CEditor::CEditor()
	{
		
	}

	void CEditor::Render()
	{
		//ImGui::ShowDemoWindow();
		//ImGui::ShowUserGuide();

		if (ImGui::Begin("Hiarchy"))
		{
			if (ImGui::TreeNodeEx("GameObject##1", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::TreeNodeEx("GameObject##1", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("GameObject##2", ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::TreeNodeEx("GameObject##1", ImGuiTreeNodeFlags_DefaultOpen))
					{
						ImGui::TreePop();
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("GameObject##3", ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}
		}
		ImGui::End();
	}
}
