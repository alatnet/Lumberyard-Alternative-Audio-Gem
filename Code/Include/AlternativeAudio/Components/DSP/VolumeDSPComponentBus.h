#pragma once

#include <AzCore\Component\ComponentBus.h>

#include <AlternativeAudio\Device\OAudioDevice.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>

namespace AlternativeAudio {
	namespace Components {
		namespace DSP {
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