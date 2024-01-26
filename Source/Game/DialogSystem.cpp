#include "pch.h"
#include "DialogSystem.h"
#include "JsonDocument.h"
#include "Text.h"
#include "TextFactory.h"

#include <Engine/Engine.h>
#include <Engine/Scene.h>

#include <Engine/Input.h>

void Dialog::DisplayTextAt(const size_t& anIndex, const Vector3f& aPosition)
{
	myText.at(anIndex)->SetPosition(aPosition);
}

void Dialog::HideText(const size_t& anIndex)
{
	myText.at(anIndex)->SetPosition({ 9999, 9999, 0 });
}

void Dialog::AddText(const std::string& someText)
{
	std::shared_ptr<Text> text = Singleton<TextFactory>().CreateText(someText, 1);
	SE::CEngine::GetInstance()->GetActiveScene()->AddInstance(text);
	
	text->SetScale({ 0.0625f, 0.0875f, 1.0f });
	myText.push_back(text);
	HideText(myText.size() - 1);
}

void DialogSystem::Init(const std::string& aDialogFile)
{
    JsonDocument dialogFile(aDialogFile);

	for (auto& json : dialogFile.GetDocument()["Dialogs"].GetArray())
	{
		Dialog& dialog = myDialogs[json["ID"].GetString()];
		for (auto& text : json["Text"].GetArray())
		{
			dialog.AddText(text.GetString());
		}
	}
}

void DialogSystem::SetActiveDialog(const std::string& anID)
{
	myActiveDialog = &myDialogs.at(anID);
}

void DialogSystem::Update()
{
	if (!myActiveDialog) return;

	if (Input::GetInputPressed(eButtonInput::Up))
	{
		auto& index = myActiveDialog->myCurrentTextIndex;
		
		if (index < myActiveDialog->Count() - 1)
		{
			myActiveDialog->HideText(index);
			myActiveDialog->DisplayTextAt(++index, { .0f, -.1f, 100 });
		}
	}
}
