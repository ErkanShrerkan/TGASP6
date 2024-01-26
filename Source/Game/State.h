#pragma once
#include "eStateID.h"

class StateStack;
class UIElement;
class UISlider;
class State
{
public:
	virtual bool Update() = 0;
	virtual void Render() = 0;
	eStateID GetStateID() const;

	bool GetLetThroughRender() { return myLetThroughRender; }
	bool operator== (const State& aStateID);

protected:
	virtual void AddUIElement(UIElement* aUIElement);
	virtual void AddUISlider(UISlider* aUISlider);
	virtual void UpdateUIElements();
protected:
	std::vector<UIElement*> myUIElements;
	eStateID myStateID = eStateID::Count;
	bool myLetThroughRender = false;
};
