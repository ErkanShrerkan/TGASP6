#include "pch.h"
#include "UIManager.h"
#include "StateStack.h"
#include "UIElement.h"
#include "UISlider.h"
#include <Engine/Engine.h>
#include <Engine/DX11.h>
#include <Engine/Scene.h>
#include "StateGameplay.h"
#include "UIGauge.h"
#include <Engine/Input.h>
#include "StateCutscene.h"
#include <Engine/Sprite.h>
UIManager::UIManager()
{
}

UIManager::~UIManager()
{
	delete myCursor;
	myCursor = nullptr;
}

void UIManager::Init()
{
	myStateStack = std::make_shared<StateStack>();
	myCursor = new UIElement("Textures/Sprites/UI/UI_S_Cursor.dds");
	myCursor->GetSprite()->SetSizeRelativeToImage({ 0.75f, 0.75f});
	myGameplayState = reinterpret_cast<StateGameplay*>(&(myStateStack->GetStateFromId(eStateID::Playing)));
}

bool UIManager::Update()
{
	myCursor->SetPosition(Input::GetInstance().GetMousePos());
	return myStateStack->Update();
}

void UIManager::Render()
{
	myStateStack->Render();
	myCursor->Render();
}

void UIManager::AddUIElement(UIElement* aUIElement)
{
	myUIElements.push_back(aUIElement);
	SE::CEngine* const& engine = SE::CEngine::GetInstance();
	SE::CScene* const& scene = engine->GetActiveScene();
	scene->AddInstance(aUIElement->GetSprite());
}

void UIManager::AddUISlider(UISlider* aUISlider)
{
	myUIElements.push_back(aUISlider);
	SE::CEngine* const& engine = SE::CEngine::GetInstance();
	SE::CScene* const& scene = engine->GetActiveScene();
	scene->AddInstance(aUISlider->GetSprite());
	scene->AddInstance(aUISlider->GetIcon());
}

void UIManager::RemoveUIElement(UIElement* aUIElement)
{
	myUIElements.erase(
		std::remove_if(myUIElements.begin(), myUIElements.end(),
			[&](UIElement* o) { return o == aUIElement; }),
		myUIElements.end());
}

void UIManager::SetPlayerVariables(float* someHp, float* someAp)
{
	StateGameplay* myGameplay = reinterpret_cast<StateGameplay*>(&(myStateStack->GetStateFromId(eStateID::Playing)));
	myGameplay->SetHpVariable(someHp);
	myGameplay->SetApVariable(someAp);
}

void UIManager::SetNewPlayerReferences(float* someHp, float* someAp)
{
	StateGameplay* myGameplay = reinterpret_cast<StateGameplay*>(&(myStateStack->GetStateFromId(eStateID::Playing)));
	myGameplay->GetHPGauge()->SetNewVariableReference(someHp);

	myGameplay->GetAPGauge()->SetNewVariableReference(someAp);
}

void UIManager::SetPlayerAbilityLockedStatus(bool aState, int anIndex)
{
	StateGameplay* myGameplay = reinterpret_cast<StateGameplay*>(&(myStateStack->GetStateFromId(eStateID::Playing)));
	myGameplay->SetPlayerAbilityLockedStatus(aState, anIndex);
}

StateStack& UIManager::GetStateStack()
{
	return *myStateStack;
}

StateGameplay* UIManager::GetStateGameplay()
{
	return myGameplayState;
}
