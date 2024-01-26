#pragma once
#include <FMOD/fmod.h>
#include <FMOD/fmod.hpp>
#include <FMOD/common.h>
#include <FMOD/fmod_errors.h>
#include <FMOD/fmod_studio.h>
#include <FMOD/fmod_studio.hpp>
#include <FMOD/fmod_studio_common.h>
#include "FMOD/fmod_studio_guids.hpp"
#include <map>
#include <vector>

#include "Transform.h"
namespace AudioClip = FSPRO::Event;
namespace AudioBus = FSPRO::Bus;
namespace AudioSnapShot = FSPRO::Snapshot;

namespace SE
{
	class CAudioEngine
	{
	public:
		static CAudioEngine* GetInstance()
		{
			static CAudioEngine ourAudioEngine = CAudioEngine();

			return &ourAudioEngine;
		}
		CAudioEngine(CAudioEngine&) = delete;
		void operator=(CAudioEngine&) = delete;
		~CAudioEngine() = default;

		void Update(SE::CTransform aListenerTransform, const bool& aShouldUpdateListener);


		FMOD::Studio::EventInstance* CreateEventInstance(const FMOD::Studio::ID& anID);

		void StartEventOneShot(const FMOD_GUID& anEventID, const Vector3f& anObjectPosition = { 0,0,0 }, const bool& aHasWorldPosition = false, const char* aEventParameterName = nullptr, float aParameterValue = 0);

		void SetBusVolume(const FMOD_GUID& aBusID, const float& aVolume);
		void SetMuteBus(const FMOD_GUID& aBusID, const bool& aBool);

		float GetBusVolume(const FMOD_GUID& aBusID);
		bool GetMuteBus(const FMOD_GUID& aBusID);

		float dBToVolume(float adB);
		float VolumeTodB(float aVolume);


		Vector3f GetListenerPos();

		void SetSnapShot(const FMOD_GUID& aBusID, const bool& shouldStart = true);

	private:

		CAudioEngine();

		void InitSystem();
		void LoadBanks();
		void LoadSingleEvent(const FMOD_GUID& aEventID);
		void LoadAllEvents();
		void LoadSingleBus(const FMOD_GUID& aBusID);
		void LoadAllBuses();


		void UpdateListener(SE::CTransform aListenerTransform);
		void SetPosition(FMOD::Studio::EventInstance* anInstance, const Vector3f& aWorldPosition);
		void SetUIPosition(FMOD::Studio::EventInstance* anInstance);
		void ShouldBeVirtual();

		Vector3f GetInstancePos(FMOD::Studio::EventInstance* instance);

		FMOD::Studio::System* mySystem;
		FMOD::Studio::Bank* myMasterBank;

		std::map<FMOD::Studio::ID, FMOD::Studio::EventDescription*> myEventDescriptions;

		std::map<FMOD::Studio::ID, std::vector<FMOD::Studio::EventInstance*>> myEventInstances;

		std::map<FMOD::Studio::ID, FMOD::Studio::Bus*> myBuses;


	};
}

