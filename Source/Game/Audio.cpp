#include "pch.h"
#include "Audio.h"
#include "Engine/GameObject.h"
#include "Engine/ModelInstance.h"
#include "Engine\Camera.h"
#include "Engine/CDebugDrawer.h"

AudioComponent::AudioComponent()
{
}

AudioComponent::~AudioComponent()
{
	if (myEventInstance)
	{
		Stop(true);
		myEventInstance->release();
		myEventInstance = nullptr;
	}
}

void AudioComponent::SetAudioClip(const FMOD::Studio::ID& anAudioClip)
{
	myEventInstance = SE::CAudioEngine::GetInstance()->CreateEventInstance(anAudioClip);
}

void AudioComponent::Init()
{
}

void AudioComponent::Update(const Vector3f& aWorldPosition, const bool& aHasWorldPosition)
{
	assert(myEventInstance->isValid() && "FMOD eventinstance should always be valid!");
	//CDebugDrawer::GetInstance().DrawSphere(myWorldPosition, 2000, { 1,1,0,1 });
	if (aHasWorldPosition)
	{
		SetWorldPosition(aWorldPosition);
		SetScreenPosition();
	}
	else
	{
		SetUIScreenPos();
	}
}

void AudioComponent::Start()
{
	FMOD_STUDIO_PLAYBACK_STATE state;
	myEventInstance->getPlaybackState(&state);
	if (state != FMOD_STUDIO_PLAYBACK_PLAYING && state != FMOD_STUDIO_PLAYBACK_STARTING)
	{
		assert(myEventInstance->isValid() && "FMOD eventinstance should always be valid!");
		myEventInstance->start();
	}
}

void AudioComponent::Stop(const bool& aStopImmediate)
{
	FMOD_STUDIO_STOP_MODE mode;
	assert(myEventInstance->isValid() && "FMOD eventinstance should always be valid!");
	if (aStopImmediate)
	{
		mode = FMOD_STUDIO_STOP_IMMEDIATE;
	}
	else
	{
		mode = FMOD_STUDIO_STOP_ALLOWFADEOUT;
	}
	myEventInstance->stop(mode);
}

void AudioComponent::SetUIScreenPos()
{
	Vector3f listenerPos = SE::CAudioEngine::GetInstance()->GetListenerPos();
	FMOD_3D_ATTRIBUTES vec = {};
	vec.forward.z = 1.0f;
	vec.up.y = 1.0f;
	vec.position.x = listenerPos.x; //TODO fixa en annan lösning så att avstånden blir rimliga
	vec.position.y = listenerPos.y;
	vec.position.z = listenerPos.z;

	myEventInstance->set3DAttributes(&vec);
}

void AudioComponent::SetScreenPosition()
{
	FMOD_3D_ATTRIBUTES vec = {};
	vec.forward.z = 1.0f;
	vec.up.y = 1.0f;
	vec.position.x = myWorldPosition.x * 0.01f; //TODO fixa en annan lösning så att avstånden blir rimliga
	vec.position.y = myWorldPosition.y * 0.01f;
	vec.position.z = myWorldPosition.z * 0.01f;

	myEventInstance->set3DAttributes(&vec);
	
}

void AudioComponent::SetWorldPosition(const Vector3f& aWorldPosition)
{
	myWorldPosition = aWorldPosition;
}

void AudioComponent::SetEventParameterByName(const char* aParameter, const float& aValue)
{
	myEventInstance->setParameterByName(aParameter, aValue);
}

bool AudioComponent::GetIsPlayingOrStarting()
{
	FMOD_STUDIO_PLAYBACK_STATE state;
	myEventInstance->getPlaybackState(&state);
	if (state == FMOD_STUDIO_PLAYBACK_PLAYING || state == FMOD_STUDIO_PLAYBACK_STARTING)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AudioComponent::ChangeAudioClip(FMOD::Studio::ID anAudioClip)
{
	myEventInstance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
	myEventInstance->release();
	myEventInstance = SE::CAudioEngine::GetInstance()->CreateEventInstance(anAudioClip);
}
