#pragma once
#include <memory>
#include <vector>
#include "StateGameplay.h"
class StateStack;
class UIElement;
class UISlider;
class UIManager
{
public:
	UIManager();
	~UIManager();
	void Init();
	bool Update();
	void Render();
	void AddUIElement(UIElement* aUIElement);
	void AddUISlider(UISlider* aUISlider);
	void RemoveUIElement(UIElement* aUIElement);
	void SetPlayerVariables(float* someHp, float* someAp);
	void SetNewPlayerReferences(float* someHp, float* someAp);
	void SetPlayerAbilityLockedStatus(bool aState, int anIndex);
	StateStack& GetStateStack();
	StateGameplay* GetStateGameplay();
	UIElement* GetCursor() { return myCursor;}
private:
	std::shared_ptr<StateStack> myStateStack;
	std::vector<UIElement*> myUIElements;
	UIElement* myCursor = nullptr;
	StateGameplay* myGameplayState;
};

