#pragma once

#include <AzCore\Component\ComponentBus.h>

#include <AlternativeAudio\Device\OAudioDevice.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>

namespace AlternativeAudio {
	namespace Components {
		//const char * AudioSourceComponentUUID = "{57C8594B-0D13-4555-AED1-769D59A55A09}";

		class AudioSourceComponentEvents
			: public AZ::ComponentBus
		{
		public:
			virtual void Play() = 0;
			virtual void PlaySFX() = 0;
			virtual void Pause() = 0;
			virtual void Resume() = 0;
			virtual void Stop() = 0;
			virtual bool IsPlaying() = 0;
			virtual void SetTime(double time) = 0;
			virtual AudioSourceTime GetTime() = 0;

			virtual void SetPath(AZStd::string path) = 0;
			virtual AZStd::string GetPath() = 0;


			virtual void SetLibraryCrc(AZ::Crc32 lib) = 0;
			virtual AZ::Crc32 GetLibraryCrc() = 0;
			virtual void SetLibrary(AZStd::string lib) = 0;
			virtual AZStd::string GetLibrary() = 0;

			virtual IAudioSource* GetSource() = 0;

			virtual void SetDevice(OAudioDevice* device) = 0;
		};

		using AudioSourceComponentBus = AZ::EBus<AudioSourceComponentEvents>;
	}
}