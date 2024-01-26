#pragma once
#include <map>
#include <vector>
#include <memory>
#include "eStateID.h"
#include "Observer.h"
class StateCutscene;
class State;
class UIElement;
class AudioSystem;

class StateStack : public Observer
{
	

public:
	StateStack();
	~StateStack();
	void RecieveMessage(eMessage aMsg) override;
	void PushState(eStateID aState);
	void PushStateAndPop(eStateID aState);
	void Pop();
	bool Update();
	void Render();
	void RenderStateAtIndex(int anIndex);
	State& GetCurrentState();
	State& GetStateFromId(eStateID aStateID);
	State& GetPreviousState();
	const int GetSize();
	StateCutscene* GetCutscene();

protected:
	std::map<eStateID, std::shared_ptr<State>> myCachedStates;
	std::vector<std::shared_ptr<State>> myStateStack;

private:

	AudioSystem* myAudioSystem;

};