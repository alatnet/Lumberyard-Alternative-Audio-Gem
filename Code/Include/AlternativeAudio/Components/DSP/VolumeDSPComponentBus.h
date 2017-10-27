#pragma once

#include <AzCore\Component\ComponentBus.h>

#include <AlternativeAudio\Device\OAudioDevice.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>

namespace AlternativeAudio {
	namespace Components {
		namespace DSP {
			//const char * VolumeComponentUUID = "{73717A4F-CE6E-4917-811B-9350216F483E}";

			class VolumeComponentEvents
				: public AZ::EntityComponentBus {
			public:
				virtual void SetVol(float vol) = 0;
				virtual float GetVol() = 0;
			};

			using VolumeComponentBus = AZ::EBus<VolumeComponentEvents>;
		}
	}
}