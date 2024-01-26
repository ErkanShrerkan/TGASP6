#include "pch.h"
#include "StateGameplay.h"
#include "Engine/Scene.h"
#include <Engine/Engine.h>
#include "eMessageTypes.h"
#include "UIElement.h"
#include "UIGauge.h"
#include "Postmaster.h"
#include <Engine\Sprite.h>
#include <Engine\Input.h>
#include <iostream>
#include <Engine/Stopwatch.h>
#include "GameWorld.h"
#include <Engine/Camera.h>
#include "Health.h"
#include "Expose.h"
#include <string>
#include "UIManager.h"
extern SE::CCamera* _locCamera;

StateGameplay::StateGameplay()
{
	CreateHPBars();
	myApGauge = nullptr;
	myHpGauge = nullptr;
	myBossGauge = nullptr;
	myStateID = eStateID::Playing;
	myAbilityBar = new UIElement("Textures/Sprites/UI/UI_S_HUD_nofill.dds");
	myAbilityBar->SetPosition({ 0.5f, 1.f });
	myAbilityBar->SetPivot({ 0.5f, 1.f });
	myMaxAbilityBar = new UIElement("Textures/Sprites/UI/UI_S_HUD_nofillMax.dds");
	myMaxAbilityBar->SetPivot({ 0.5f, 1.f });
	myMaxAbilityBar->SetPosition({ 0.5f, 1.f });
	UIElement* resourceFill = new UIElement("Textures/Sprites/UI/UI_S_HUD_resourceEmpty.dds");
	resourceFill->SetPosition({ 0.5f, 1.f });
	resourceFill->SetPivot({ 0.5f, 1.f });
	myResourceOverlay = new UIElement("Textures/Sprites/UI/UI_S_HUD_resourceOverlay.dds");
	myResourceOverlay->SetPosition({ 0.5f, 1.f });
	myResourceOverlay->SetPivot({ 0.5f, 1.f });

	myPlayerXP.Gauge = std::make_shared<UIGauge>("Textures/Sprites/UI/UI_S_ExpBar.dds", nullptr, 0.f, true, false);
	myPlayerXP.MaxGauge = std::make_shared<UIGauge>("Textures/Sprites/UI/UI_S_ExpBarMax.dds", nullptr, 0.f, true, false);
	myPlayerXP.MaxGauge->GetSprite()->SetPosition({ -1.0f,-1.0f });
	myPlayerXP.currentLevelSprite = std::make_shared<UIElement>("Textures/Sprites/UI/Texts/1.dds");
	myPlayerXP.currentLevelSprite->SetPivot({ 0.5f,0.5f });
	myPlayerXP.currentLevelSpriteOutline = std::make_shared<UIElement>("Textures/Sprites/UI/Texts/1.dds");
	myPlayerXP.currentLevelSpriteOutline->SetPivot({ 0.5f,0.5f });
	myPlayerXP.currentLevel = 1;
	EXPOSE(myXPPosX);
	EXPOSE(myXPPosY);
	
	myPlayerXP.Gauge->SetPivot({ 0.0f, 0.5f });
	myPlayerXP.MaxGauge->SetPivot({ 0.0f, 0.5f });

	myLevelUpGlow = new UIElement("Textures/Sprites/UI/UI_S_LevelUpGlow.dds");
	myLevelUpGlow->SetPivot({ 0.5f,0.5f });
	myLevelUpGlow->SetPosition({ 0.5f,0.8f });
	myLevelUpText = new UIElement("Textures/Sprites/UI/Texts/UI_S_UnlockAOE.dds");
	myLevelUpText->SetPivot({ 0.5f,0.5f });
	myLevelUpText->SetPosition({ 0.5f,0.5f });
	//myLevelUpGlow->SetSize({ 1.0f,1.0f });
	myLevelUpTimer = new Stopwatch(7);
	myLevelUpTimer->SetIsOver();
	//Postmaster::GetInstance()->Subscribe(this, eMessage::eUnlockRMB);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eUnlockAOE);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eUnlockTeleport);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eUnlockHook);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eUnlockUltimate);
	Postmaster::GetInstance()->Subscribe(this, eMessage::eGeneralLevelUp);

	for (size_t i = 0; i < myAbilityIcons.size(); i++)
	{
		std::string string = "Textures/Sprites/UI/UI_S_A0";
		string = string + std::to_string(i + 1) + ".dds";

		myAbilityIcons[i] = new UIElement(string.c_str());
		myAbilityIcons[i]->myAbilityIcon = true;
		myAbilityIcons[i]->SetPivot({ 0.5f, 0.f });
		myAbilityIcons[i]->SetPosition({ 0.3802f + (i * 0.0396f), 0.9255f });
		myAbilityIcons[i]->GetSprite()->SetShaderType(SE::SpriteShaderType::eCircularFill);
		myAbilityIcons[i]->GetSprite()->SetShaderData(1);
	}
	for (size_t i = 0; i < myAbilityLocks.size(); i++)
	{
		std::string string = "Textures/Sprites/UI/UI_S_Lock.dds";

		myAbilityLocks[i] = new UIElement(string.c_str());
		myAbilityLocks[i]->myAbilityIcon = true;
		myAbilityLocks[i]->SetPivot({ 0.5f, 0.f });
		myAbilityLocks[i]->SetPosition({ 0.3802f + (i * 0.0396f), 0.9255f });
		myAbilityLocks[i]->myShouldRenderLock = true;
		myAbilityLocks[i]->myIsLock = true;

		std::string path = "Textures/Sprites/UI/Texts/";
		if (i == 0)
		{
			path += std::to_string(3);
			path += ".dds";
		}
		else if (i == 1)
		{
			path += std::to_string(6);
			path += ".dds";
		}
		else if (i == 2)
		{
			path += std::to_string(9);
			path += ".dds";
		}
		else if (i == 3)
		{
			path  = "Textures/Sprites/UI/UI_S_UnlockGem.dds"; 
		}

		if (i < 3)
		{
			myAbilityLockLevels[i] = new UIElement(path.c_str());
			myAbilityLockLevels[i]->SetPivot({ 0.5f, 0.f });
			myAbilityLockLevels[i]->SetPosition({ 0.38502f + (i * 0.0396f), 0.9555f });
			myAbilityLockLevels[i]->myShouldRenderLock = true;
			myAbilityLockLevels[i]->myIsLock = true;
		}
		else if (i == 3)
		{
			myAbilityLockLevels[i] = new UIElement(path.c_str());
			myAbilityLockLevels[i]->SetPivot({ 0.5f, 0.f });
			myAbilityLockLevels[i]->SetPosition({ 0.38012f + (i * 0.0396f), 0.9505f });
			myAbilityLockLevels[i]->myShouldRenderLock = true;
			myAbilityLockLevels[i]->myIsLock = true;
		}
		//myAbilityIcons[i]->GetSprite()->SetShaderType(SE::SpriteShaderType::eCircularFill);
		//myAbilityIcons[i]->GetSprite()->SetShaderData(1);
	}
	for (size_t i = 0; i < myAbilityTooltips.size(); i++)
	{
		std::string string = "Textures/Sprites/UI/Texts/UI_S_Tooltip0";
		string = string + std::to_string(i + 1) + ".dds";

		myAbilityTooltips[i] = new UIElement(string.c_str());
		myAbilityTooltips[i]->myAbilityTooltip = true;
		myAbilityTooltips[i]->SetPivot({ 0.5f, 0.f });
		myAbilityTooltips[i]->SetPosition({ 0.5f, 0.68f});
	}
	for (size_t i = 0; i < myObjectives.size(); i++)
	{
		std::string string = "Textures/Sprites/UI/Texts/UI_S_Objective0";
		string = string + std::to_string(i + 1) + ".dds";

		myObjectives[i] = new UIElement(string.c_str());
		myObjectives[i]->myObjective = true;
		AddUIElement(myObjectives[i]);

	}
	for (auto& tooltip : myAbilityTooltips)
	{
		AddUIElement(tooltip);
	}
	for (auto& icon : myAbilityIcons)
	{
		AddUIElement(icon);
	}
	for (auto& icon : myAbilityLocks)
	{
		AddUIElement(icon);
	}
	for (auto& icon : myAbilityLockLevels)
	{
		AddUIElement(icon);
	}
	AddUIElement(resourceFill);

}

StateGameplay::~StateGameplay()
{
	Postmaster::GetInstance()->UnSubscribe(this, eMessage::eUnlockAOE);
	Postmaster::GetInstance()->UnSubscribe(this, eMessage::eUnlockTeleport);
	Postmaster::GetInstance()->UnSubscribe(this, eMessage::eUnlockHook);
	Postmaster::GetInstance()->UnSubscribe(this, eMessage::eUnlockUltimate);
}

bool StateGameplay::Update()
{
	//0.505
	//	0.904
	
	myPlayerXP.currentLevelSprite->SetPosition({ 0.505f, 0.904f });
	myPlayerXP.currentLevelSpriteOutline->SetPosition({ 0.505f, 0.904f });
	myLevelUpTimer->Update(ENGINE->GetDeltaTime());
	if (myLevelUpTimer->IsOver())
	{
		myShouldRenderLevelUpText = false;
		myShouldRenderLevelUpGlow = false;
	}
	if (Input::GetInstance().GetInputPressed(eButtonInput::Escape))
	{
		Postmaster::GetInstance()->SendMail(eMessage::ePauseGame);
	}
	for (auto& element : myUIElements)
	{
		if (element->myIsBossHp)
			continue;
		element->Update();
	}
	if (myShouldRenderBossHp)
	{
		myBossGaugeBackground->Update();
		myBossGauge->Update();
		myBossGaugeOverlay->Update();
		myBossName->Update();
	}
	return true;
}

void StateGameplay::Render()
{
	LevelUpRender();
	myPlayerXP.Gauge->Render();
	myPlayerXP.MaxGauge->Render();
	for (size_t i = 0; i < myAbilityIcons.size(); i++)
	{
		if (myAbilityIcons[i]->MouseHovering() && !myAbilityIcons[i]->GetGreyedOut())
		{
			myAbilityTooltips[i]->Render();
		}
	}
	for (auto& element : myUIElements)
	{
		if (element == nullptr)
		{
			continue;
		}
		
		if (element->myObjective)
		{
			if (element == myObjectives[myObjectiveIndex])
			{
				element->Render();
			}
			continue;
		}
		if (element->myIsEnemyHp || element->myAbilityTooltip || element->myIsLock || element->myIsBossHp)
		{
			continue;
		}
		element->Render();

	}	
	for (size_t i = 0; i < myHpBarIndex; i++)
	{
		auto& bar = myHPBarPool[i];
		bar.Background->Render();
		bar.Gauge->Render();
	}
	for (size_t i = 0; i < myAbilityLocks.size(); i++)
	{
		if (myAbilityLocks[i]->myShouldRenderLock)
		{
			myAbilityLocks[i]->Render();
		}
	}
	for (size_t i = 0; i < myAbilityLockLevels.size(); i++)
	{
		if (myAbilityLockLevels[i]->myShouldRenderLock)
		{
			myAbilityLockLevels[i]->Render();
		}
	}

	if (myShouldRenderBossHp)
	{
		myBossGaugeBackground->Render();
		myBossGauge->Render();
		myBossGaugeOverlay->Render();
		myBossName->Render();
	}

	
	myHpBarIndex = 0;

	RenderCurrentLevel();
	
	
}

void StateGameplay::LevelUpRender()
{
	if (!myLevelUpTimer->IsOver() && myShouldRenderLevelUpGlow)
	{

		if (myFadeInTimer < myFadeInTimerMax)
		{
			myFadeInTimer += ENGINE->GetDeltaTime();
			myLevelUpText->GetSprite()->SetColor({ myLevelUpText->GetSprite()->GetColor().rgb,myFadeInTimer / myFadeInTimerMax });
			myLevelUpGlow->GetSprite()->SetColor({ myLevelUpGlow->GetSprite()->GetColor().rgb,myFadeInTimer * 2.0f / myFadeInTimerMax });
			myLevelUpGlow->GetSprite()->SetSizeRelativeToImage({1.0f + myFadeInTimer, 1.0f + myFadeInTimer });
			myPlayerXP.currentLevelSpriteOutline->GetSprite()->SetColor({ 1,1,0,myFadeInTimer * 2.0f / myFadeInTimerMax });
		}
		else
		{
			myLevelUpText->GetSprite()->SetColor({ myLevelUpText->GetSprite()->GetColor().rgb,1 });

			myLevelUpGlow->GetSprite()->SetColor({ myLevelUpGlow->GetSprite()->GetColor().rgb, myLevelUpGlow->GetSprite()->GetColor().a - myLevelUpTimer->GetTime() / myLevelUpTimer->GetTimeInterval()});
			myPlayerXP.currentLevelSpriteOutline->GetSprite()->SetColor({ 1,1,0, myLevelUpGlow->GetSprite()->GetColor().a - myLevelUpTimer->GetTime() / myLevelUpTimer->GetTimeInterval() });
			if (myLevelUpTimer->GetTime() > 4)
			{
				myFadeOutTimer += 0.01f;
			}
		}
		if (myFadeOutTimer > 0.0f)
		{
			myFadeOutTimer += ENGINE->GetDeltaTime();
			myLevelUpText->GetSprite()->SetColor({ myLevelUpText->GetSprite()->GetColor().rgb,1 - myFadeOutTimer / myFadeOutTimerMax });
		}
		
		
		myLevelUpGlow->Render();
		if (myShouldRenderLevelUpText)
		{
			myLevelUpText->Render();
		}
	}
}

void StateGameplay::SetHpVariable(float* aVariable)
{
	if (myHpGauge == nullptr)
	{
		myHpGauge = new UIGauge("Textures/Sprites/UI/UI_S_HUD_hpfill.dds", aVariable, *aVariable);
		myHpGauge->SetPosition({ 0.27f, .92f });
		myHpGauge->SetPivot({ 0.f, 0.5f });
		AddUIElement(myHpGauge);
	}
}

void StateGameplay::SetApVariable(float* aVariable)
{
	if (myApGauge == nullptr)
	{
		myApGauge = new UIGauge("Textures/Sprites/UI/UI_S_HUD_apfill.dds", aVariable, *aVariable);
		myApGauge->SetPosition({ 0.73f, .92f });
		myApGauge->SetPivot({ 1.f, 0.5f });
		AddUIElement(myApGauge);
		AddUIOverlays();
	}
}

void StateGameplay::CreateBossGauge(float* aVariable)
{
	if (myBossGauge == nullptr)
	{

		myBossGaugeBackground = new UIElement("Textures/Sprites/UI/UI_S_BossHPBackground.dds");
		myBossGaugeBackground->myIsBossHp = true;
		myBossGaugeBackground->SetPosition({ 0.5f, 0.05f });
		myBossGaugeBackground->SetPivot({ 0.5f, 0.5f });
		myBossGaugeBackground->GetSprite()->SetSizeRelativeToImage({ 0.97f, 1.f });
		myBossGaugeOverlay = new UIElement("Textures/Sprites/UI/UI_S_BossHPOverlay.dds");
		myBossGaugeOverlay->myIsBossHp = true;
		myBossGaugeOverlay->SetPosition({ 0.5f, 0.05f });
		myBossGaugeOverlay->SetPivot({ 0.5f, 0.5f });
		myBossGaugeOverlay->GetSprite()->SetSizeRelativeToImage({ 0.97f, 1.f });
		myBossGauge = new UIGauge("Textures/Sprites/UI/UI_S_BossHP.dds", aVariable, *aVariable, true);
		myBossGauge->myIsBossHp = true;
		myBossGauge->SetPosition({ 0.24f, 0.05f });
		myBossGauge->SetPivot({ 0.f, 0.5f });
		myBossName = new UIElement("Textures/Sprites/UI/UI_S_BossTitle.dds");
		myBossName->myIsBossHp = true;
		myBossName->SetPivot({ 0.5f, 0.0f });
		myBossName->SetPosition({ 0.5f, 0.07f });
		AddUIElement(myBossGaugeBackground);
		AddUIElement(myBossGauge);
		AddUIElement(myBossGaugeOverlay);
		AddUIElement(myBossName);
		myShouldRenderBossHp = true;

	}
}

void StateGameplay::DestroyBossGauge()
{
	Singleton<UIManager>().RemoveUIElement(myBossGauge);
	Singleton<UIManager>().RemoveUIElement(myBossGaugeBackground);
	Singleton<UIManager>().RemoveUIElement(myBossName);
	myBossGauge = nullptr;
	myBossGaugeBackground = nullptr;
	myBossName = nullptr;
	myShouldRenderBossHp = false;
}

void StateGameplay::CreateHPBars()
{
	for (int i = 0; i < myHPBarPool.size(); i++)
	{
		auto& bar = myHPBarPool[i];

		auto bg = std::make_shared<UIElement>("Textures/Sprites/UI/UI_S_EnemyHPBackground.dds");
		bar.Gauge = std::make_shared<UIGauge>("Textures/Sprites/UI/UI_S_HUD_EnemyHP.dds", nullptr, 0.f, true, false);

		bg->SetPivot({ 0.f, 0.5f });
		
		bar.Gauge->SetPivot({ 0.f, 0.5f });
		bar.Background = bg;
		
		bg->myIsEnemyHp = true;
		bar.Gauge->myIsEnemyHp = true;

		AddUIElement(bg.get());
		AddUIElement(bar.Gauge.get());
	}
}


void StateGameplay::AddUIOverlays()
{
	AddUIElement(myResourceOverlay);
	AddUIElement(myPlayerXP.Gauge.get());
	AddUIElement(myPlayerXP.MaxGauge.get());
	AddUIElement(myAbilityBar);
}

UIGauge* StateGameplay::GetHPGauge()
{
	return myHpGauge;
}

UIGauge* StateGameplay::GetAPGauge()
{
	return myApGauge;
}

void StateGameplay::SetPlayerAbilityLockedStatus(bool aState, int anIndex)
{
	myAbilityLocks[anIndex]->myShouldRenderLock = aState;
	if (anIndex <= 3)
	{
	myAbilityLockLevels[anIndex]->myShouldRenderLock = aState;

	}
}

void StateGameplay::RenderHPBarAt(Vector3f aPosition, float aHealth)
{
	Matrix4x4f cameraTransform(_locCamera->GetTransform());

	Vector4f pos = { aPosition - _locCamera->GetRenderOffset(), 1.f };

	//float4 cameraPos(_locCamera->GetPosition(), 1);
	//cameraTransform.SetRow(4, cameraPos);

	float4 viewPos = pos * Matrix4x4f::GetFastInverse(cameraTransform);
	float4 projPos = viewPos * _locCamera->GetProjection();
	projPos /= projPos.w;
	projPos.x = (projPos.x + 1.f) * 0.5f;
	projPos.y = 1 - (projPos.y + 1.f) * 0.5f;

	float2 sp = { projPos.xyz.xy };
	sp.x -= myHPBarPool[0].Background->GetSprite()->GetSize().x * 0.5f;

	myHPBarPool[myHpBarIndex].Gauge->GetSprite()->SetRect({ 0.f, 0.f, aHealth, 1.f });
	myHPBarPool[myHpBarIndex].Gauge->GetSprite()->SetSizeRelativeToImage({ aHealth, 1.f});
	myHPBarPool[myHpBarIndex].Gauge->GetSprite()->SetPosition(sp);
	myHPBarPool[myHpBarIndex].Background->GetSprite()->SetPosition(sp);
	myHpBarIndex++;
	if (myHpBarIndex >= myHPBarPool.size())
	{
		myHpBarIndex = 0;
	}
}

void StateGameplay::RenderXPBar(float anXP)
{
	myPlayerXP.Gauge->GetSprite()->SetRect({ 0.f, 0.f, anXP, 1.f });
	myPlayerXP.Gauge->GetSprite()->SetSizeRelativeToImage({ anXP, 1.f });
	myPlayerXP.Gauge->GetSprite()->SetPosition({0.36f,0.908f });
}

void StateGameplay::RenderCurrentLevel()
{
	myPlayerXP.currentLevelSprite->Render();
	myPlayerXP.currentLevelSpriteOutline->Render();
}

void StateGameplay::SetCurrentLevel(int aLevel)
{
	if (aLevel != myPlayerXP.currentLevel && aLevel <= 9)
	{
		myPlayerXP.currentLevel = aLevel;
		std::string levelString = std::to_string(aLevel);
		std::string path = "Textures/Sprites/UI/Texts/";
		path += levelString;
		path += ".dds";
		myPlayerXP.currentLevelSprite = std::make_shared<UIElement>(path.c_str());
		myPlayerXP.currentLevelSprite->SetPivot({ 0.5f,0.5f });
		myPlayerXP.currentLevelSpriteOutline = std::make_shared<UIElement>(path.c_str());
		myPlayerXP.currentLevelSpriteOutline->SetPivot({ 0.5f,0.5f });
		if (aLevel == 9)
		{
			ChangeToMaxLevelUI();
		}
	}
}

void StateGameplay::RecieveMessage(eMessage aMsg)
{
	myFadeInTimer = 0;
	myFadeOutTimer = 0;
	delete myLevelUpGlow;
	myLevelUpGlow = new UIElement("Textures/Sprites/UI/UI_S_LevelUpGlow.dds");
	myLevelUpGlow->SetPivot({ 0.5f, 0.5f });
	myLevelUpGlow->SetPosition({ 0.5007f, 0.894f });
	switch (aMsg)
	{
	/*case eMessage::eUnlockRMB:
		delete myLevelUpText;
		myLevelUpText = new UIElement("Textures/Sprites/UI/Texts/UI_S_UnlockHook.dds");*/

		//break;
	case eMessage::eUnlockAOE:
		delete myLevelUpText;
		myLevelUpText = new UIElement("Textures/Sprites/UI/Texts/UI_S_UnlockNova.dds");
		myShouldRenderLevelUpText = true;

		break;
	case eMessage::eUnlockHook:
		delete myLevelUpText;
		myLevelUpText = new UIElement("Textures/Sprites/UI/Texts/UI_S_UnlockHook.dds");
		myShouldRenderLevelUpText = true;
		
		break;
	case eMessage::eUnlockTeleport:
		delete myLevelUpText;
		myLevelUpText = new UIElement("Textures/Sprites/UI/Texts/UI_S_UnlockTeleport.dds");
		myShouldRenderLevelUpText = true;

		break;
	case eMessage::eUnlockUltimate:
		delete myLevelUpText;
		myLevelUpText = new UIElement("Textures/Sprites/UI/Texts/UI_S_UnlockUltimate.dds");
		myShouldRenderLevelUpText = true;

		break;

	default:
		break;
	}
	myLevelUpTimer->Reset();
	myShouldRenderLevelUpGlow = true;
}

void StateGameplay::ChangeToMaxLevelUI()
{

	AddUIElement(myMaxAbilityBar);

	myPlayerXP.MaxGauge->GetSprite()->SetRect({ 0.f, 0.f, 1.0f, 1.f });
	myPlayerXP.MaxGauge->GetSprite()->SetSizeRelativeToImage({ 1.0f, 1.f });
	myPlayerXP.MaxGauge->GetSprite()->SetPosition({ 0.36f,0.908f });
	
}
