#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore\RTTI\RTTI.h>
#include <AlternativeAudio\AAErrorHandler.h>
#include <AlternativeAudio\AAAudioFrame.h>
#include <AlternativeAudio\DSP\AADSPEffectHandler.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>

namespace AlternativeAudio {
	enum AAResampleQuality {
		eAARQ_Best,
		eAARQ_Medium,
		eAARQ_Fastest,
		eAARQ_Zero_Order_Hold,
		eAARQ_Linear,
		//misc resample qualities
		eAARQ_Misc0,
		eAARQ_Misc1,
		eAARQ_Misc2,
		eAARQ_Misc3,
		eAARQ_Misc4
	};

	struct OAudioDeviceInfo {
		AZ_RTTI(OAudioDeviceInfo, "{B8161836-72D7-4FCC-AA51-C5809DB994C9}");
		AZStd::string name{ "" };
		int maxChannels{ 0 };
		double defaultSampleRate{ 0.0 };

		AudioFrame::Type currentFormat{ AudioFrame::Type::eT_af1 };
		double currentSampleRate{ 0.0 };
		AAResampleQuality currentResampleQuality{ eAARQ_Linear };

		AZStd::string getName() { return this->name; }
		int getMaxChannels() { return this->maxChannels; }
		double getDefaultSampleRate() { return this->defaultSampleRate; }

		AudioFrame::Type getFormat() { return this->currentFormat; }
		double getSampleRate() { return this->currentSampleRate; }
		AAResampleQuality getResampleQuality() { return this->currentResampleQuality; }

		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<OAudioDeviceInfo>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<OAudioDeviceInfo>("OAudioDeviceInfo")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("getName", &OAudioDeviceInfo::getName)
				->Method("getMaxChannels", &OAudioDeviceInfo::getMaxChannels)
				->Method("getDefaultSampleRate", &OAudioDeviceInfo::getDefaultSampleRate)
				->Method("getFormat", &OAudioDeviceInfo::getFormat)
				->Method("getSampleRate", &OAudioDeviceInfo::getSampleRate)
				->Method("getResampleQuality", &OAudioDeviceInfo::getResampleQuality)
				->Constant("name", &OAudioDeviceInfo::getName)
				->Constant("maxChannels", &OAudioDeviceInfo::getMaxChannels)
				->Constant("defaultSampleRate", &OAudioDeviceInfo::getDefaultSampleRate)
				->Constant("currentFormat", &OAudioDeviceInfo::getFormat)
				->Constant("currentSampleRate", &OAudioDeviceInfo::getSampleRate)
				->Constant("currentResampleQuality", &OAudioDeviceInfo::getResampleQuality);
		}
	};

	//output device
	class OAudioDevice
		: public AADSPDeviceEffectHandler
		, public AASmartRef
		, public AAErrorHandler
	{
	public:
		//virtual ~OAudioDevice() {}
	public:
		AZ_RTTI(OAudioDevice, "{1F39A7FD-B0B3-4CEC-B36C-AA05D20044C2}", AADSPDeviceEffectHandler, AASmartRef, AAErrorHandler);
	public:
		virtual void SetStream(double samplerate, AudioFrame::Type audioFormat, void * userdata) = 0;
		virtual void SetResampleQuality(AAResampleQuality quality) = 0;
		virtual OAudioDeviceInfo GetDeviceInfo() = 0;
	public:
		virtual unsigned long long PlaySource(IAudioSource * source) = 0;
		virtual void PlaySFXSource(IAudioSource * source) {}; //used to play sfx audio sources that are one hit wonders
		virtual void PauseSource(unsigned long long id) = 0;
		virtual void ResumeSource(unsigned long long id) = 0;
		virtual void StopSource(unsigned long long id) = 0;
		virtual bool IsPlaying(unsigned long long id) = 0;
		virtual AudioSourceTime GetTime(unsigned long long id) = 0;
		virtual void SetTime(unsigned long long id, double time) = 0;
	public: //attribute control for running audio sources
		virtual void UpdateAttribute(unsigned long long id, AZ::Crc32 idCrc, AAAttribute* attr) = 0;
		virtual void ClearAttribute(unsigned long long id, AZ::Crc32 idCrc) = 0;
	public: //used to control ALL sound source playback.
		virtual void PauseAll() {};
		virtual void ResumeAll() {};
		virtual void StopAll() {};
	public:
		virtual void Queue(bool startstop) = 0; //used to queue up multiple commands for simultanious execution.
	public:
		virtual void SetMaster(bool onoff) = 0; //mainly for when assigning a device as a master device
	protected:
		AAAttributeHandler m_attributes;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<OAudioDevice, AADSPDeviceEffectHandler, AASmartRef, AAErrorHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<OAudioDevice>("OAudioDevice")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("SetStream", &OAudioDevice::SetStream)
				->Method("SetResampleQuality", &OAudioDevice::SetResampleQuality)
				->Method("GetDeviceInfo", &OAudioDevice::GetDeviceInfo)
				->Method("PlaySource", &OAudioDevice::PlaySource)
				->Method("PauseSource", &OAudioDevice::PauseSource)
				->Method("ResumeSource", &OAudioDevice::ResumeSource)
				->Method("StopSource", &OAudioDevice::StopSource)
				->Method("IsPlaying", &OAudioDevice::IsPlaying)
				->Method("GetTime", &OAudioDevice::GetTime)
				->Method("SetTime", &OAudioDevice::SetTime)
				->Method("Queue", &OAudioDevice::Queue);
		}
	};

	class OAudioDeviceProvider : public AAErrorHandler {
	public:
		AZ_RTTI(OAudioDeviceProvider, "{693CCBBC-8BE9-44F1-B71C-FE76314F21D8}", AAErrorHandler);
	public:
		virtual OAudioDevice* NewDevice(long long device, double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void* userdata) = 0;
		virtual AZStd::vector<OAudioDeviceInfo>& GetDevices() = 0;
		virtual long long GetDefaultDevice() = 0;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<OAudioDeviceProvider, AAErrorHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<OAudioDeviceProvider>("OAudioDeviceProvider")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("NewDevice", &OAudioDeviceProvider::NewDevice)
				->Method("GetDevices", &OAudioDeviceProvider::GetDevices)
				->Method("GetDefaultDevice", &OAudioDeviceProvider::GetDefaultDevice);
		}
	};

	class AANullProvider : public OAudioDeviceProvider{
	public:
		AZ_RTTI(AANullProvider, "{1B0D387F-968A-4B6E-8668-545BC239DED9}", OAudioDeviceProvider);
	public:
		AANullProvider() {
			this->m_NullDeviceInfo = AZStd::vector<OAudioDeviceInfo>(0);
			this->m_NullDeviceInfo.shrink_to_fit();
		}
	public:
		OAudioDevice* NewDevice(long long device, double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void* userdata) { return nullptr; }
		AZStd::vector<OAudioDeviceInfo>& GetDevices() { return this->m_NullDeviceInfo; }
		long long GetDefaultDevice() { return -1; }
	private:
		AZStd::vector<OAudioDeviceInfo> m_NullDeviceInfo;
	};
}

namespace AZ {
	AZ_TYPE_INFO_SPECIALIZE(AlternativeAudio::AAResampleQuality, "{FA9A856C-AE6B-4758-8C5E-469FB82CF29A}");
}