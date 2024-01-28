#include "pch.h"
#include "StateStack.h"
#include "State.h"
#include "StateMainMenu.h"
#include "StateGameplay.h"
#include "StateSettings.h"
#include "StateLevelSelect.h"
#include "StatePause.h"
#include "StateCutscene.h"
#include "StateDeath.h"
#include "StateCredits.h"
#include "Postmaster.h"
#include "PlayerSystem.h"
#include "AudioSystem.h"
#include "UIManager.h"
#include "GameWorld.h"
#include "AudioSystem.h"

StateStack::StateStack()
{
	myCachedStates.emplace(eStateID::MainMenu, std::make_shared<StateMainMenu>());
	myCachedStates.emplace(eStateID::Settings, std::make_shared<StateSettings>());
	myCachedStates.emplace(eStateID::Playing, std::make_shared<StateGameplay>());
	myCachedStates.emplace(eStateID::Pause, std::make_shared<StatePause>());
	myCachedStates.emplace(eStateID::LevelSelect, std::make_shared<StateLevelSelect>());
	myCachedStates.emplace(eStateID::Cutscene, std::make_shared<StateCutscene>());
	myCachedStates.emplace(eStateID::Death, std::make_shared<StateDeath>());
	myCachedStates.emplace(eStateID::Credits, std::make_shared<StateCredits>());

	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartGame);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eOpenSettings);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eQuitGame);
	Postmaster::GetInstance()->Subscribe(this, eMessage::ePopStack);
	Postmaster::GetInstance()->Subscribe(this, eMessage::ePauseGame);
	Postmaster::GetInstance()->Subscribe(this, eMessage::ePopStackToMainMenu);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eOpenLevelSelect);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eOpenCredits);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartLevel1);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartLevel2);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartLevel3);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartLevel4);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartLevel5);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eStartCutscene);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eRespawn);
	Postmaster::GetInstance()->Subscribe(this, eMessage::ePlayerDied);

	myAudioSystem = AudioSystem::GetInstance();
	myAudioSystem->Init();
}

StateStack::~StateStack()
{

}

void StateStack::RecieveMessage(eMessage aMsg)
{
	bool fromLevelSelect = false;
	if (GetCurrentState().GetStateID() == eStateID::LevelSelect)
	{
		fromLevelSelect = true;
	}
	switch (aMsg)
	{
	case eMessage::eStartGame:
		myAudioSystem->SetSnapShot(AudioSnapShot::PauseGame,false);
		myAudioSystem->ChangeMusic(AudioClip::Music_SwampTest);
		myAudioSystem->StartStartGameMusic();
		PushState(eStateID::Playing);
		if (GameWorld::GetInstance()->GetLevelHandler().GetActiveLevelIndex() == 0)
		{
			PushState(eStateID::Cutscene);
			GetCutscene()->ForceStopCutscene();
			GetCutscene()->PlayCutscene(eCutscenes::Intro);
		}
		break;
	case eMessage::eQuitGame:
		exit(0);
		break;
	case eMessage::eOpenSettings:
		PushState(eStateID::Settings);
		break;
	case eMessage::eOpenCredits:
		PushState(eStateID::Credits);
		break;
	case eMessage::ePopStack:
		Pop();
		if (GetCurrentState() == GetStateFromId(eStateID::Playing))
		{
			myAudioSystem->SetSnapShot(AudioSnapShot::PauseGame, false);
		}
		
		break;
	case eMessage::ePauseGame:
		PushState(eStateID::Pause);
		myAudioSystem->SetSnapShot(AudioSnapShot::PauseGame, true);
		break;
	case eMessage::ePopStackToMainMenu:
		myAudioSystem->ChangeMusic(AudioClip::Music_Menu);
		myAudioSystem->SetSnapShot(AudioSnapShot::PauseGame, false);
		while (myStateStack[myStateStack.size() - 1]->GetStateID() != eStateID::MainMenu)
		{
			Pop();
		}
		break;
	case eMessage::eOpenLevelSelect:
		PushState(eStateID::LevelSelect);
		break;
	case eMessage::eStartLevel1:
	{
		Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(0);

		myAudioSystem->ChangeMusic(AudioClip::Music_SwampTest);
		myAudioSystem->StartGameMusic(); 
		GameWorld::GetInstance()->GetLevelHandler().SetLevelByIndex(0, fromLevelSelect);
		if (myStateStack[myStateStack.size() - 1]->GetStateID() != eStateID::Playing)
		{

			PushState(eStateID::Playing);
			PushState(eStateID::Cutscene);
			GetCutscene()->ForceStopCutscene();
			GetCutscene()->PlayCutscene(eCutscenes::Intro);
		}
		//load level
	}
		
		break;
	case eMessage::eStartLevel2:
	{
		Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(2);
		myAudioSystem->ChangeMusic(AudioClip::Music_Level2);
		myAudioSystem->StartGameMusic();
		if (myStateStack[myStateStack.size() - 1]->GetStateID() != eStateID::Playing)
		{
			PushState(eStateID::Playing);
		}
		//load level
		GameWorld::GetInstance()->GetLevelHandler().SetLevelByIndex(1, fromLevelSelect);
	}
		
		break;
	case eMessage::eStartLevel3:
	{
		Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(3);
		//play music
		myAudioSystem->ChangeMusic(AudioClip::Music_MiniBoss);
		//push state if not already playing
		if (myStateStack[myStateStack.size() - 1]->GetStateID() != eStateID::Playing)
		{
			PushState(eStateID::Playing);
		}
		//load level
		GameWorld::GetInstance()->GetLevelHandler().SetLevelByIndex(2, fromLevelSelect);
	}
		break;

	case eMessage::eStartLevel4:
		Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(6);
		myAudioSystem->StartGameMusic();
		myAudioSystem->ChangeMusic(AudioClip::Music_Level4);
		myAudioSystem->ChangeAmbience(AudioClip::Ambience_Level4);
		if (myStateStack[myStateStack.size() - 1]->GetStateID() != eStateID::Playing)
		{
			PushState(eStateID::Playing);
		}
		//load level
		GameWorld::GetInstance()->GetLevelHandler().SetLevelByIndex(3, fromLevelSelect);
		break;
	case eMessage::eStartLevel5:
	{
		Singleton<UIManager>().GetStateGameplay()->SetObjectiveIndex(7);
		myAudioSystem->StopGameMusic();
		myAudioSystem->ChangeAmbience(AudioClip::Ambience_Boss);
		if (myStateStack[myStateStack.size() - 1]->GetStateID() != eStateID::Playing)
		{
			PushState(eStateID::Playing);
		}
		//load level
		//ändra till 5 när den är inne
		GameWorld::GetInstance()->GetLevelHandler().SetLevelByIndex(4, fromLevelSelect);

		//Set references
		//auto& playerSys = GameWorld::GetInstance()->GetLevelHandler().GetActiveLevel().GetPlayerSystem();
		//Singleton<UIManager>().SetNewPlayerReferences(&playerSys.GetHealthComponent()->GetHealth(), &playerSys.GetAPComponent()->GetAttackPower());
	}		
		break;
	case eMessage::eStartCutscene:
		PushState(eStateID::Cutscene);
		break;
	case eMessage::eRespawn:
		//Simon
		GameWorld::GetInstance()->GetLevelHandler().GetActiveLevel().GetPlayerSystem().RevivePlayer();
		Pop();
		break;
	case eMessage::ePlayerDied:
		PushState(eStateID::Death);
		break;
	default:
		break;
	}
}

void StateStack::PushState(eStateID aState)
{
	myStateStack.push_back(myCachedStates.at(aState));
}

void StateStack::PushStateAndPop(eStateID aState)
{
	Pop();
	PushState(aState);
}

void StateStack::Pop()
{
	myStateStack.pop_back();
}

bool StateStack::Update()
{
	return myStateStack[myStateStack.size() - 1]->Update();
}

void StateStack::Render()
{
	RenderStateAtIndex(static_cast<int>(myStateStack.size()) - 1);
}

void StateStack::RenderStateAtIndex(int anIndex)
{
	if (anIndex < 0)
		return;

	if (myStateStack[anIndex]->GetLetThroughRender() == true)
	{
		RenderStateAtIndex(anIndex - 1);
	}
	myStateStack[anIndex]->Render();
}

State& StateStack::GetCurrentState()
{
	return *myStateStack[myStateStack.size() - 1];
}

State& StateStack::GetStateFromId(eStateID aStateID)
{
	return *myCachedStates.at(aStateID);
}

const int StateStack::GetSize()
{
	return static_cast<int>(myStateStack.size());
}

StateCutscene* StateStack::GetCutscene()
{
	return reinterpret_cast<StateCutscene*>(&(GetStateFromId(eStateID::Cutscene)));
	
}

State& StateStack::GetPreviousState()
{
	return *myStateStack[myStateStack.size() - 2];
}
