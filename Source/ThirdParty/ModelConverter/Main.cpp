#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <Windows.h>
#include "Input.h"
#include "ModelLoader.h"

int consoleIndex = 0;
int consoleWidth = 120;
int consoleHeight = 30;
wchar_t* screenBuffer = nullptr;
DWORD bytesWritten = 0;
HANDLE consoleHandle;

void ConsoleRender()
{
	screenBuffer[consoleWidth * consoleHeight - 1] = '\0';
	WriteConsoleOutputCharacter(consoleHandle, screenBuffer, consoleWidth * consoleHeight, { 0, 0 }, &bytesWritten);
	consoleIndex = 0;
}

void ClearConsole()
{
	consoleIndex = 0;
	for (int i = 0; i < consoleWidth * consoleHeight; i++)
	{
		screenBuffer[i] = ' ';
	}
}

void WriteLine(const char* someText)
{
	int i = 0;
	while (someText[i] != '\0')
	{
		screenBuffer[consoleIndex] = someText[i];
		consoleIndex++;
		i++;
	}
	for (int j = 0; j < consoleWidth - (i % consoleWidth); j++)
	{
		screenBuffer[consoleIndex] = ' ';
		consoleIndex++;
	}
}

int main()
{
	screenBuffer = new wchar_t[static_cast<double>(consoleWidth) * static_cast<double>(consoleHeight)];
	consoleHandle = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleActiveScreenBuffer(consoleHandle);
	SetConsoleCursorInfo(consoleHandle, &info);

	ModelLoader loader;
	Input::Init();

	bool exit = false;
	bool success = true;
	bool first = true;
	bool valid = false;

	int option = 0;
	int total = 0;
	int successes = 0;

	while (!exit)
	{
		while (!valid)
		{
			Input::Update();
			ClearConsole();
			WriteLine("Choose Option:");
			WriteLine("");
			WriteLine(option == 0 ? "->Purge all .erc files" : "Purge all .erc files");
			WriteLine(option == 1 ? "->Purge all .erc files and make new ones" : "Purge all .erc files and make new ones");
			WriteLine(option == 2 ? "->Only convert .fbx files with missing .erc files" : "Only convert .fbx files with missing .erc files");
			WriteLine(option == 3 ? "->Exit" : "Exit");
			WriteLine("");

			if (!first)
			{
				std::string successMessage;
				successMessage += std::to_string(successes);
				successMessage += " / ";
				successMessage += std::to_string(total - 1);
				successMessage += " succeeded";
				WriteLine(successMessage.c_str());
			}

			if (Input::GetInputPressed(eButtonInput::Up))
			{
				option--;
			}
			else if (Input::GetInputPressed(eButtonInput::Down))
			{
				option++;
			}
			else if (Input::GetInputPressed(eButtonInput::Enter))
			{
				valid = true;
				if (option == 3)
				{
					delete[] screenBuffer;
					return 0;
				}
			}

			option = std::clamp(option, 0, 3);
			ConsoleRender();
		}
		valid = false;
		total = 0;
		successes = 0;

		int current = 0;
		int all = 0;

		if (option < 2)
		{
			auto it = std::filesystem::recursive_directory_iterator{ "Models" };
			for (const auto& dir : it)
			{
				std::string entry(dir.path().string());
				std::replace(entry.begin(), entry.end(), '\\', '/');
				std::string ext = std::string(entry.end() - 4, entry.end());
				if (ext == ".erc" || ext == ".myr")
				{
					all++;
				}
			}
			std::string currentFbx;
			for (auto const& dir : std::filesystem::recursive_directory_iterator{ "Models" })
			{
				ClearConsole();

				std::string entry(dir.path().string());
				std::replace(entry.begin(), entry.end(), '\\', '/');

				std::string ext = std::string(entry.end() - 4, entry.end());
				if (ext == ".erc" || ext == ".myr")
				{
					currentFbx = entry;
					WriteLine(currentFbx.c_str());
					std::string progressBar;
					progressBar += std::to_string(int((float(current) / float(all)) * 100));
					progressBar += "% done...";
					WriteLine(progressBar.c_str());
					ConsoleRender();

					success = loader.DeleteFileML(entry.c_str());
					successes += success;
					total++;
					current++;
				}
			}
		}
		if (option > 0)
		{
			total = 0;
			successes = 0;
			current = 0;
			all = 0;
			auto it = std::filesystem::recursive_directory_iterator{ "models" };
			for (const auto& dir : it)
			{
				std::string entry(dir.path().string());
				std::replace(entry.begin(), entry.end(), '\\', '/');
				if (std::string(entry.end() - 4, entry.end()) == ".fbx")
				{
					all++;
				}
			}

			std::string currentFbx;
			for (auto const& dir : std::filesystem::recursive_directory_iterator{ "models" })
			{
				ClearConsole();

				std::string entry(dir.path().string());
				std::replace(entry.begin(), entry.end(), '\\', '/');

				if (std::string(entry.end() - 4, entry.end()) == ".fbx")
				{
					currentFbx = entry;
					WriteLine(currentFbx.c_str());
					std::string progressBar;
					progressBar += std::to_string(int((float(current) / float(all)) * 100));
					progressBar += "% done...";
					WriteLine(progressBar.c_str());
					ConsoleRender();

					success = loader.LoadFBX(entry, option);
					successes += success;
					total++;
					current++;
				}
			}
		}

		first = false;
	}
}
