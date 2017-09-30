#pragma once

#include <AzCore/EBus/EBus.h>
#include <AlternativeAudio\AAAudioFrame.h>
#include <AlternativeAudio\AudioSource\IAudioSource.h>
#include <AlternativeAudio\DSP\AADSPEffect.h>
#include <AlternativeAudio\Device\OAudioDevice.h>

#define ALTERNATIVE_AUDIO_VER_HIGH 3
#define ALTERNATIVE_AUDIO_VER_LOW 1
#define ALTERNATIVE_AUDIO_VER_REV 0
#define ALTERNATIVE_AUDIO_VER_STR "3.1.0"

namespace AlternativeAudio {
	//function definitions
	typedef AZStd::function<IAudioSource* (const char *, void*)> NewAudioSourceFunc;
	typedef AZStd::function<AADSPEffect* (void*)> NewDSPEffectFunc;

	typedef AZStd::function<void(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len)> ConvertAudioFrameFunc;
	typedef AZStd::function<void(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len)> MixAudioFramesFunc;

	//general functions bus
	class AlternativeAudioRequests
		: public AZ::EBusTraits {
	public:
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
		// Public functions
	public: //frame conversion
		virtual void ConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) = 0;
		virtual void DefaultConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) = 0;
		virtual void SetConvertFunction(ConvertAudioFrameFunc convertFunc) = 0;
		virtual void ResetConvertFunction() = 0; //reset to default Alternative Audio frame conversion function.
	public: //frame mixing
		virtual void MixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) = 0;
		virtual void DefaultMixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) = 0;
		virtual void SetMixFunction(MixAudioFramesFunc mixFunc) = 0;
		virtual void ResetMixFunction() = 0; //reset to default Alternative Audio mixing function.
	public: //audio utilities
		virtual AudioFrame::Type GetAudioFormat(int numberOfChannels) = 0;
		virtual int GetNumberOfChannels(AudioFrame::Type type) = 0;
	};
	using AlternativeAudioRequestBus = AZ::EBus<AlternativeAudioRequests>;

	//audio sources bus
	class AlternativeAudioSourceRequests
		: public AZ::EBusTraits {
	public:
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	public: //basic audio library system
		virtual void RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr) = 0;
		virtual IAudioSource * NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata) = 0;
		virtual AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetAudioLibraryNames() = 0;
	};
	using AlternativeAudioSourceBus = AZ::EBus<AlternativeAudioSourceRequests>;

	//dsp effects bus
	class AlternativeAudioDSPRequests
		: public AZ::EBusTraits {
	public:
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	public: //basic DSP library system
		virtual void RegisterDSPEffect(AZStd::string libname, AZ::Crc32 crc, NewDSPEffectFunc ptr) = 0;
		virtual AADSPEffect * NewDSPEffect(AZ::Crc32 crc, void* userdata) = 0;
		virtual AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetDSPEffectNames() = 0;
	public: //basic global DSP system
		virtual bool AddEffect(AADSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot) = 0;
		virtual unsigned long long AddEffectFreeSlot(AADSPSection section, AZ::Crc32 crc, void* userdata) = 0;
		virtual AADSPEffect * GetEffect(AADSPSection section, unsigned long long slot) = 0;
		virtual bool RemoveEffect(AADSPSection section, unsigned long long slot) = 0;
		virtual void ProcessEffects(AADSPSection section, AudioFrame::Type format, float* buffer, long long len, AAFlagHandler * flags) = 0;
		virtual AADSPDeviceEffectHandler* GetDSPDeviceEffectHandler() = 0;
	};
	using AlternativeAudioDSPBus = AZ::EBus<AlternativeAudioDSPRequests>;

	//audio device bus
	class AlternativeAudioDeviceRequests
		: public AZ::EBusTraits {
	public:
		static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
		static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
	public:
		virtual void RegisterPlaybackLibrary(AZStd::string libname, AZ::Crc32 crc, OAudioDeviceProvider* ptr) = 0;
		virtual OAudioDevice * NewDevice(AZ::Crc32 crc, long long device, double samplerate, AudioFrame::Type audioFormat, void* userdata) = 0;
		virtual AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetPlaybackLibraryNames() = 0;
		virtual AZStd::vector<OAudioDeviceInfo>& GetPlaybackDevices(AZ::Crc32 playbackLib) = 0;
		virtual long long GetDefaultPlaybackDevice(AZ::Crc32 playbackLib) = 0;
		virtual OAudioDeviceProvider* GetDeviceProvider(AZ::Crc32 playbackLib) = 0;
	public:
		virtual void SetMasterDevice(OAudioDevice * device) = 0;
		virtual void SetStream(double samplerate, AudioFrame::Type audioFormat, void * userdata) = 0;
		virtual void SetResampleQuality(AAResampleQuality quality) = 0;
		virtual OAudioDeviceInfo GetDeviceInfo() = 0;
		virtual unsigned long long PlaySource(IAudioSource * source) = 0;
		virtual void PlaySFXSource(IAudioSource * source) = 0; //used to play sfx audio sources that are one hit wonders
		virtual void PauseSource(unsigned long long id) = 0;
		virtual void ResumeSource(unsigned long long id) = 0;
		virtual void StopSource(unsigned long long id) = 0;
		virtual bool IsPlaying(unsigned long long id) = 0;
		virtual AudioSourceTime GetTime(unsigned long long id) = 0;
		virtual void SetTime(unsigned long long id, double time) = 0;
	public:
		//used to control ALL sound source playback.
		virtual void PauseAll() = 0;
		virtual void ResumeAll() = 0;
		virtual void StopAll() = 0;
	public:
		virtual void Queue(bool startstop) = 0; //used to queue up multiple commands for simultanious execution. (applyies to play,pause,resume, and stop).
	};
	using AlternativeAudioDeviceBus = AZ::EBus<AlternativeAudioDeviceRequests>;

} // namespace AlternativeAudio
