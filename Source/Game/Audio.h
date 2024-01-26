#pragma once
#include <Engine/AudioEngine.h>
#include "Engine/Component.h"

namespace SE
{
	class CGameObject;
}

class AudioComponent
{
public:
	AudioComponent();
	~AudioComponent();
	void SetAudioClip(const FMOD::Studio::ID& anAudioClip);

	void Update(const Vector3f& aWorldPosition = {0, 0, 0}, const bool& aHasWorldPosition = false);
	void Init();

	void Start();
	void Stop(const bool& aStopImmediate = true);

	void SetUIScreenPos();

	void SetScreenPosition();
	void SetWorldPosition(const Vector3f& aWorldPosition);
	void ChangeAudioClip(FMOD::Studio::ID anAudioClip);
	void SetEventParameterByName(const char* aParameter, const float& aValue);

	bool GetIsPlayingOrStarting();

private:
	Vector3f myWorldPosition;
	FMOD::Studio::EventInstance* myEventInstance = nullptr;
};

