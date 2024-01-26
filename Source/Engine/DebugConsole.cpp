#include "pch.h"
#include "DebugConsole.h"
#include <stdarg.h>
#include <stdio.h>
#include <ctime> // std::strftime
#include <ImGui\imgui.h>

namespace SE
{
	namespace Debug
	{
		void CDebugConsole::Render()
		{
			ImGui::SetNextWindowSize(ImVec2(520, 600), ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Console Window##cmd"))
			{
				ImGui::BulletText("Console Header Here");

				ImGui::Separator();

				const float reservedFooterHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
				ImGui::BeginChild("ScrollingRegion", ImVec2(0, -reservedFooterHeight), false, ImGuiWindowFlags_HorizontalScrollbar);
				
				if (ImGui::BeginPopupContextWindow())
				{
					if (ImGui::Selectable("Clear"))
					{
						Clear();
					}
					ImGui::EndPopup();
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
				ImGuiListClipper clipper;
				clipper.Begin(static_cast<int>(myStoredText.size()));
				while (clipper.Step())
				{
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.5f, .5f, .5f, 1.f));
						// Make text selectable so one can copy it
						ImGui::TextUnformatted(myStoredText[i]);
						ImGui::PopStyleColor();
					}
				}
				ImGui::PopStyleVar();

				if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				{
					ImGui::SetScrollHereY(1.0f);
				}

				ImGui::EndChild();

				ImGui::Separator();
				ImGui::TextUnformatted("Insert some magic here");
			}
			ImGui::End();
		}

		void CDebugConsole::Clear()
		{
			/*for (auto& text : myStoredText)
			{
				delete text;
			}*/
			myStoredText.clear();
		}

		// Print
		CDebugConsole& CDebugConsole::operator()(const char* aFormat, ...)
		{
			// TODO: Add [Time]
			// pout [Time] aFormat \n
			std::string concatFormat("pout [XX:XX:XX] ");
			concatFormat.append(aFormat);
			concatFormat.append("\n");

			char buf[1024];
			va_list args;
			va_start(args, aFormat);
			vsnprintf(buf, C_ARRAY_SIZE(buf), aFormat, args);
			/* printf */ vprintf(concatFormat.c_str(), args);
			buf[C_ARRAY_SIZE(buf) - 1] = 0;
			va_end(args);
			myStoredText.push_back(_strdup(buf));
			return *this;
		}

		// std::endl
		CDebugConsole& CDebugConsole::operator<<(const EConsoleStream& aConsoleStreamEnum)
		{
			if (aConsoleStreamEnum == EConsoleStream::EndStream)
			{
				(*this)("%s", myStream.str().c_str());
				std::stringstream empty;
				myStream.swap(empty);
				myStream.clear();
			}
			return *this;
		}
		CDebugConsole& CDebugConsole::operator<<(CDebugConsole& aDebugConsole)
		{
			if (this == &aDebugConsole)
			{
				*this << EConsoleStream::EndStream;
			}
			return *this;
		}
	}
}