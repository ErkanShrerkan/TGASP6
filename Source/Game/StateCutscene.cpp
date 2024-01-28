#include "pch.h"
#include "StateCutscene.h"
#include "UIElement.h"
#include <Engine/Sprite.h>
#include <Engine/Engine.h>
#include "GameWorld.h"
#include <Engine/Input.h>
#include "Postmaster.h"
#include "LevelHandler.h"
#include "UIManager.h"
#include "UIDialog.h"
#include "StateStack.h"
#include "StateMainMenu.h"
StateCutscene::StateCutscene()
{
	//blackbars for cutscene
	myBlackbarUp = new UIElement("Textures/Sprites/UI/UI_S_BlackPixel.dds");
	myBlackbarDown = new UIElement("Textures/Sprites/UI/UI_S_BlackPixel.dds");
	myBlackscreen = new UIElement("Textures/Sprites/UI/UI_S_BlackPixel.dds");
	myCredits = new UIElement("Textures/Sprites/UI/UI_S_CreditPan.dds");
	myBlackbarUp->GetSprite()->SetSizeRelativeToScreen({ 1.f, 0.16f });
	myBlackbarDown->GetSprite()->SetSizeRelativeToScreen({ 1.f, 0.16f });
	myBlackscreen->GetSprite()->SetSizeRelativeToScreen({ 1.f, 1.f });
	myBlackbarUp->SetPivot({ 0.5f, 0.f });
	myBlackbarDown->SetPivot({ 0.5f, 1.f });
	myBlackscreen->SetPivot({ 0.f, 0.f });
	myCredits->SetPivot({ 0.5f, 0.f });
	myBlackbarUp->SetPosition({ 0.5f, 0.f });
	myBlackbarDown->SetPosition({ 0.5f, 1.f });
	myBlackscreen->SetPosition({ 0.f, 0.f });
	myCredits->SetPosition({ 0.5f, 0.f });
	AddUIElement(myBlackbarUp);
	AddUIElement(myBlackbarDown);
	AddUIElement(myBlackscreen);

	myBlackscreen->GetSprite()->SetColor({ 1.f, 1.f, 1.f, 1.f });

	myBlackbarsStopwatch = Stopwatch(2.f);
	myCutsceneStopwatch = Stopwatch(2.f);
	myBlackscreenStopwatch = Stopwatch(4.f);
	mySecondBlackscreenStopwatch = Stopwatch(4.f);
	myCreditsStopwatch = Stopwatch(40.f);
	myStartCreditsWait = Stopwatch(5.f);
	myCreditsDoneStopwatch = Stopwatch(5.f);

	for (size_t i = 0; i < static_cast<size_t>(eCutscenes::Count); i++)
	{
		eCutscenes current = static_cast<eCutscenes>(i);
		myCutsceneCameraQuaternions.emplace(current, std::vector<Quaternion>());
		myCutsceneCameraPositions.emplace(current, std::vector<Vector3f>());
		myCutsceneDialogs.emplace(current, std::vector<UIDialog*>());

		auto& dialogList = myCutsceneDialogs[current];

		if (i == 5)
		{
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog01.dds", 1));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog01.dds", 2));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog02.dds", 3));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog02.dds", 4));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog03.dds", 5));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog03.dds", 6));

		}
		else if (i == 4)
		{
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog04.dds", 1));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog04.dds", 2));
			dialogList.push_back(new UIDialog("Textures/Sprites/UI/Texts/UI_S_Dialog04.dds", 3));
		}
	}


}

StateCutscene::~StateCutscene()
{
	delete myBlackbarUp;
	delete myBlackbarDown;
	delete myBlackscreen;
	myBlackbarDown = nullptr;
	myBlackbarUp = nullptr;
	myBlackscreen = nullptr;

	for (auto& [scene, dialogue] : myCutsceneDialogs)
	{
		for (auto& line : dialogue)
		{
			delete line;
		}

		dialogue.clear();
	}

	myCutsceneDialogs.clear();
}

bool StateCutscene::Update()
{
	if (myCurrentCutscene != eCutscenes::BossDead)
	{
		myCamera->SetRenderOffset({ 0.f, 0.f, 0.f });
	}

	if (Input::GetInstance().GetInputPressed(eButtonInput::Escape))
	{
		if (myCurrentCutscene == eCutscenes::BossDead)
		{
			Postmaster::GetInstance()->SendMail(eMessage::ePopStackToMainMenu);
			SE::CAudioEngine::GetInstance()->SetMuteBus(AudioBus::SemiMaster_SFX, false);
		}
		else
		{
			DeactivateBlackbars();
		}

		ForceStopCutscene();
	}

	UpdateActiveCutscene();

	UpdateCutscene();

	return true;
}

void StateCutscene::Render()
{
	if (!myActiveCutscene)
	{
		return;
	}

	myBlackbarDown->Render();
	myBlackbarUp->Render();
	if (myActiveBlackscreen || myReverseBlackscreen)
	{
		myBlackscreen->Render();
	}
	if (myShouldUpdateCredits && myBlackscreenStopwatch.IsOver())
	{
		myCredits->Render();
	}

	for (auto& dialog : myCutsceneDialogs.find(myCurrentCutscene)->second)
	{
		if (dialog->GetKeyframe() == myKeyframeIndex)
		{
			dialog->Render();
		}
	}
}

void StateCutscene::ActivateCutscene()
{
	if (myActiveCutscene)
	{
		return;
	}

	myActiveCutscene = true;
	myBlackbarUp->SetPivot({ 0.5f, 1.f });
	myBlackbarDown->SetPivot({ 0.5f, 0.f });
	myBlackbarsStopwatch.Reset();
	auto lvlindex = GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelIndex();
	const auto& levelSettings = GameWorld::GetInstance()->GetLevelHandler().GetLevelSettingsByIndex(lvlindex);
	float3 playerPos;
	if (myCurrentCutscene == eCutscenes::Intro)
	{
		playerPos = reinterpret_cast<StateMainMenu*>(&Singleton<UIManager>().GetStateStack().GetStateFromId(eStateID::MainMenu))->GetPosByIndex(GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelIndex());

	}
	else
	{
		playerPos = GameWorld::GetInstance()->GetLevelHandler().GetActiveLevel().GetPlayerSystem().GetCurrentPosition();

	}
	CAMERA->SetOrbit(levelSettings.cameraRotation, levelSettings.cameraOffset, levelSettings.cameraDistance);
	CAMERA->SetRenderOffset(playerPos);
	mySavedCameraPosition = myCamera->GetRenderOffset() + myCamera->GetPosition();
	Quaternion q = Quaternion(myCamera->GetTransform());
	mySavedCameraQuaternion = q;
	if (myCurrentCutscene == eCutscenes::BossDead)
	{
		PlayOutroFade();
	}
}

void StateCutscene::PlayIntroFade()
{
	myActiveBlackscreen = true;
}

void StateCutscene::PlayOutroFade()
{
	myReverseBlackscreen = true;
}

void StateCutscene::BlackscreenUpdate()
{
	myBlackscreenStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
	myBlackscreen->GetSprite()->SetColor({ 1.f, 1.f, 1.f, 1.f - myBlackscreenStopwatch.GetTime() * 0.25f });
}

void StateCutscene::ReverseBlackscreenUpdate()
{
	myBlackscreenStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
	myBlackscreen->GetSprite()->SetColor({ 1.f, 1.f, 1.f, myBlackscreenStopwatch.GetTime() * 0.25f });
}

void StateCutscene::BeginEnding()
{
	myShouldUpdateCredits = false;
	myIsEnding = true;
}

void StateCutscene::UpdateActiveCutscene()
{
	if (!myActiveCutscene)
	{
		return;
	}

	myBlackbarsStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
	myCutsceneStopwatch.Update(SE::CEngine::GetInstance()->GetDeltaTime(), false);
	myBlackbarUp->SetPivot({ 0.5f, 1.f - (myBlackbarsStopwatch.GetTime() * 0.5f) });
	myBlackbarDown->SetPivot({ 0.5f, myBlackbarsStopwatch.GetTime() * 0.5f });

	if (myActiveBlackscreen)
	{
		BlackscreenUpdate();
	}
	else if (myReverseBlackscreen)
	{
		ReverseBlackscreenUpdate();
	}

	if (myShouldUpdateCredits && myBlackscreenStopwatch.IsOver())
	{
		myStartCreditsWait.Update(ENGINE->GetDeltaTime(), false);

		myCredits->GetSprite()->SetColor({ 1.f, 1.f, 1.f, myStartCreditsWait.GetTime() * 0.33f });

		if (myStartCreditsWait.IsOver())
		{

			myCreditsStopwatch.Update(ENGINE->GetDeltaTime(), false);

			auto val = myCreditsStopwatch.GetTime() * (0.025f - 0.0066f);
			myCredits->SetPivot({ 0.5f, val });

			if (myCreditsStopwatch.IsOver())
			{
				BeginEnding();
			}

		}
	}

	if (myIsEnding)
	{
		myCreditsDoneStopwatch.Update(ENGINE->GetDeltaTime(), false);
		myCredits->GetSprite()->SetColor({ 1.f, 1.f, 1.f, 1.f - myCreditsDoneStopwatch.GetTime() * 0.2f });
		if (myCreditsDoneStopwatch.IsOver())
		{
			Postmaster::GetInstance()->SendMail(eMessage::ePopStackToMainMenu);
			SE::CAudioEngine::GetInstance()->SetMuteBus(AudioBus::SemiMaster_SFX, false);
		}
	}
}

void StateCutscene::DeactivateBlackbars()
{
	myCurrentCutscene = eCutscenes::None;
	myActiveCutscene = false;
	myKeyframeIndex = 0;
	myBlackbarUp->SetPivot({ 0.5f, 1.f });
	myBlackbarDown->SetPivot({ 0.5f, 0.f });
	myActiveBlackscreen = false;
	myBlackscreen->GetSprite()->SetColor({ 1.f, 1.f, 1.f, 1.f });
	myCutsceneStopwatch.Reset();
	myBlackbarsStopwatch.Reset();
	myBlackscreenStopwatch.Reset();
	mySecondBlackscreenStopwatch.Reset();
	myCreditsStopwatch.Reset();
	myStartCreditsWait.Reset();
	myCreditsDoneStopwatch.Reset();
	myActiveCutscene = false;
	myActiveBlackscreen = false;
	myReverseBlackscreen = false;
	myCreditsStandstill = true;
	myShouldUpdateCredits = false;
	myIsEnding = false;

	Postmaster::GetInstance()->SendMail(eMessage::ePopStack);
}

void StateCutscene::UpdateCutscene()
{
	if (myCurrentCutscene == eCutscenes::BossDead)
	{

		PlayOutroFade();
		myShouldUpdateCredits = true;

		return;
	}
	auto vec = myCutsceneCameraQuaternions.find(myCurrentCutscene)->second;
	auto pos = myCutsceneCameraPositions.find(myCurrentCutscene)->second;



	if (vec.size() == myKeyframeIndex)
	{
	}
	else if (vec.size() + 1 == myKeyframeIndex)
	{
		myCurrentCameraQuaternion.slerp(vec[myKeyframeIndex - 2], mySavedCameraQuaternion, myCutsceneStopwatch.GetTime() * 0.5f);
		myCurrentCameraPosition.x = Math::Lerp(pos[myKeyframeIndex - 2].x, mySavedCameraPosition.x, myCutsceneStopwatch.GetTime() * 0.5f);
		myCurrentCameraPosition.y = Math::Lerp(pos[myKeyframeIndex - 2].y, mySavedCameraPosition.y, myCutsceneStopwatch.GetTime() * 0.5f);
		myCurrentCameraPosition.z = Math::Lerp(pos[myKeyframeIndex - 2].z, mySavedCameraPosition.z, myCutsceneStopwatch.GetTime() * 0.5f);

	}
	else if (myKeyframeIndex == 0)
	{
		if (myCurrentCutscene == eCutscenes::Intro)
		{
			myKeyframeIndex++;
			PlayIntroFade();
		}

		else
		{
			myCurrentCameraQuaternion.slerp(mySavedCameraQuaternion, vec[myKeyframeIndex], myCutsceneStopwatch.GetTime() * 0.5f);
			myCurrentCameraPosition.x = Math::Lerp(mySavedCameraPosition.x, pos[myKeyframeIndex].x, myCutsceneStopwatch.GetTime() * 0.5f);
			myCurrentCameraPosition.y = Math::Lerp(mySavedCameraPosition.y, pos[myKeyframeIndex].y, myCutsceneStopwatch.GetTime() * 0.5f);
			myCurrentCameraPosition.z = Math::Lerp(mySavedCameraPosition.z, pos[myKeyframeIndex].z, myCutsceneStopwatch.GetTime() * 0.5f);
		}
	}
	else if (vec.size() + 2 == myKeyframeIndex)
	{
		DeactivateBlackbars();
	}
	else if (myKeyframeIndex > 0 && vec.size() > myKeyframeIndex)
	{
		myCurrentCameraQuaternion.slerp(vec[myKeyframeIndex - 1], vec[myKeyframeIndex], myCutsceneStopwatch.GetTime() * 0.5f);
		myCurrentCameraPosition.x = Math::Lerp(pos[myKeyframeIndex - 1].x, pos[myKeyframeIndex].x, myCutsceneStopwatch.GetTime() * 0.5f);
		myCurrentCameraPosition.y = Math::Lerp(pos[myKeyframeIndex - 1].y, pos[myKeyframeIndex].y, myCutsceneStopwatch.GetTime() * 0.5f);
		myCurrentCameraPosition.z = Math::Lerp(pos[myKeyframeIndex - 1].z, pos[myKeyframeIndex].z, myCutsceneStopwatch.GetTime() * 0.5f);
	}

	if (myCutsceneStopwatch.IsOver())
	{
		myKeyframeIndex++;
		myCutsceneStopwatch.Reset();
	}

	Matrix4x4f mat = Matrix4x4f();
	mat *= myCurrentCameraQuaternion.getMatrix();
	myCamera->SetTransform(mat);
	myCamera->SetRenderOffset(myCurrentCameraPosition);
	myCamera->SetPosition({ 0, 0, 0 });
}


void StateCutscene::PlayCutscene(eCutscenes aCutscene)
{
	myCamera = GameWorld::GetInstance()->GetCamera();

	myCurrentCutscene = aCutscene;
	ActivateCutscene();
}

void StateCutscene::AddCutsceneKeyframe(eCutscenes aCutscene, Transform aTransform)
{
	const Vector3f& pos(aTransform.GetPosition());

	Quaternion q = Quaternion(aTransform.GetTransform());

	myCutsceneCameraQuaternions.find(aCutscene)->second.push_back(q);
	myCutsceneCameraPositions.find(aCutscene)->second.push_back(pos);


}

void StateCutscene::Reset()
{
	for (auto& cutscene : myCutsceneCameraPositions)
	{
		cutscene.second.clear();
	}
	for (auto& cutscene : myCutsceneCameraQuaternions)
	{
		cutscene.second.clear();
	}
}

void StateCutscene::ForceStopCutscene()
{
	myActiveCutscene = false;
	myShouldUpdateCredits = false;
}

