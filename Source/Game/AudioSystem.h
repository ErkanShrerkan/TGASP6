#pragma once
#include "Audio.h"
#include "Transform.h"

class AudioSystem 
{
public:

	static AudioSystem* GetInstance()
	{
		static AudioSystem ourAudioSystem;

		return &ourAudioSystem;
	}
	
	AudioSystem(AudioSystem&) = delete;
	void operator=(AudioSystem&) = delete;
	~AudioSystem() = default;

	void Update(Transform& aListenerTransform, const bool& aShouldUpdateListener = true);
	void Init();

	void ChangeMusic(const FMOD::Studio::ID& anAudioClip);

	void ChangeAmbience(const FMOD::Studio::ID& anAudioClip);

	void UpdateMusicParameter(const char* aParameterName, const float& aValue);

	void StartGameMusic();

	void StartStartGameMusic();

	void StartGameAmbience();

	void StopGameMusic();
	void StopGameAmbience();

	void StartEventOneShot(const FMOD_GUID& anEventID, const Vector3f& anObjectPosition = { 0,0,0 }, const bool& aHasWorldPosition = false);

	void SetSnapShot(const FMOD_GUID& aBusID, const bool& shouldStart = true);

private:
	AudioSystem();
	AudioComponent myGameAmbience;
	AudioComponent myGameMusic;
	AudioComponent myStartGameMusic;

};

