#include "pch.h"
#include "AudioSystem.h"
//#include "Coordinator.h"

void AudioSystem::Update(Transform& aListenerTransform, const bool& aShouldUpdateListener)
{

	SE::CAudioEngine::GetInstance()->Update(aListenerTransform, aShouldUpdateListener);
	myGameMusic.Update(aListenerTransform.GetPosition());
	myStartGameMusic.Update(aListenerTransform.GetPosition());
	myGameAmbience.Update(aListenerTransform.GetPosition());
	//myGameWater.SetWorldPosition(aListenerTransform.GetPosition());
	//myGameWater.Update({ 0,0,0 });
}

void AudioSystem::Init()
{
	myGameMusic.SetAudioClip(AudioClip::Music_Menu);
	myStartGameMusic.SetAudioClip(AudioClip::Music_StartGame);
	myGameAmbience.SetAudioClip(AudioClip::Ambience_Swamp);
	myGameMusic.Start();
	myGameAmbience.Start();
}

void AudioSystem::ChangeMusic(const FMOD::Studio::ID& anAudioClip)
{
	myGameMusic.Stop(false);
	myGameMusic.SetAudioClip(anAudioClip);
	myGameMusic.Start();
}

void AudioSystem::ChangeAmbience(const FMOD::Studio::ID& anAudioClip)
{
	myGameAmbience.Stop(false);
	myGameAmbience.SetAudioClip(anAudioClip);
	myGameAmbience.Start();
}

void AudioSystem::UpdateMusicParameter(const char* aParameterName, const float& aValue)
{
	myGameMusic.SetEventParameterByName(aParameterName, aValue);
}
void AudioSystem::StartGameMusic()
{
	if (!myGameMusic.GetIsPlayingOrStarting())
	{
		myGameMusic.Start();
	}
}
void AudioSystem::StartStartGameMusic()
{
	if (!myStartGameMusic.GetIsPlayingOrStarting())
	{
		myStartGameMusic.Start();
	}
}
void AudioSystem::StartGameAmbience()
{
	if (!myGameAmbience.GetIsPlayingOrStarting())
	{
		myGameAmbience.Start();
	}
}

void AudioSystem::StopGameMusic()
{
	myGameMusic.Stop(false);
}

void AudioSystem::StopGameAmbience()
{
	myGameAmbience.Stop(false);
}

void AudioSystem::StartEventOneShot(const FMOD_GUID& anEventID, const Vector3f& anObjectPosition, const bool& aHasWorldPosition)
{
	SE::CAudioEngine::GetInstance()->StartEventOneShot(anEventID, anObjectPosition, aHasWorldPosition);
}

void AudioSystem::SetSnapShot(const FMOD_GUID& aBusID, const bool& shouldStart)
{
	SE::CAudioEngine::GetInstance()->SetSnapShot(aBusID, shouldStart);
}

AudioSystem::AudioSystem()
{
}
