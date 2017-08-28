#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio/AlternativeAudioBus.h>
#include <AlternativeAudio\DSP\AADSPEffectHandler.h>
#include "Built-in DSP\InterlaceDSP.h"

namespace AlternativeAudio {
	class AlternativeAudioSystemComponent
		: public AZ::Component
		, protected AlternativeAudioRequestBus::Handler
		, protected AlternativeAudioSourceBus::Handler
		, protected AlternativeAudioDSPBus::Handler
		, protected AlternativeAudioDeviceBus::Handler
		, public AADSPDeviceEffectHandler
	{
	public:
		AlternativeAudioSystemComponent();
		~AlternativeAudioSystemComponent();
	public:
		AZ_COMPONENT(AlternativeAudioSystemComponent, "{8C2C3BF1-C34E-4BEC-B299-A9AAC5276511}");

		static void Reflect(AZ::ReflectContext* context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

	protected:
		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioRequestBus interface implementation
	protected: //frame conversion
		void ConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) { this->currentConvert(in, out, inType, outType, len); }
		void DefaultConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) { this->defaultConvert(in, out, inType, outType, len); }
		void SetConvertFunction(ConvertAudioFrameFunc convertFunc);
		void ResetConvertFunction() { this->currentConvert = this->defaultConvert; }
	protected: //frame mixing
		void MixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) { this->currentMix(output, source, frameType, len); }
		void DefaultMixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) { this->defaultMix(output, source, frameType, len); }
		void SetMixFunction(MixAudioFramesFunc mixFunc);
		void ResetMixFunction() { this->currentMix = this->defaultMix; }
	protected: //audio utilities
		AudioFrame::Type GetAudioFormat(int numberOfChannels);
		int GetNumberOfChannels(AudioFrame::Type type);
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioSourceBus interface implementation
	protected: //IAudioSource
		void RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr);
		IAudioSource * NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata);
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetAudioLibraryNames() { return *(this->m_sourceLibNames); }
	private: //IAudioSource
		AZStd::unordered_map<AZ::Crc32, NewAudioSourceFunc> *m_sourceLibFuncs;
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> *m_sourceLibNames;
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioDSPBus interface implementation
	protected: //AADSPEffect
		void RegisterDSPEffect(AZStd::string libname, AZ::Crc32 crc, NewDSPEffectFunc ptr);
		AADSPEffect * NewDSPEffect(AZ::Crc32 crc, void* userdata);
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetDSPEffectNames() { return *(this->m_dspLibNames); }
	protected: //basic DSP system //remove as moving it to AADSPDeviceEffectHandler?
		bool AddEffect(AADSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot);
		unsigned long long AddEffectFreeSlot(AADSPSection section, AZ::Crc32 crc, void* userdata);
		AADSPEffect * GetEffect(AADSPSection section, unsigned long long slot) { return AADSPDeviceEffectHandler::GetEffect(section, slot); }
		bool RemoveEffect(AADSPSection section, unsigned long long slot) { return AADSPDeviceEffectHandler::RemoveEffect(section, slot); }
		void ProcessEffects(AADSPSection section, AudioFrame::Type format, float* buffer, long long len, AAFlagHandler * flags) { AADSPDeviceEffectHandler::ProcessEffects(section, format, buffer, len, flags); }
		AADSPDeviceEffectHandler* GetDSPDeviceEffectHandler() { return (AADSPDeviceEffectHandler*)this; }
	private: //AADSPEffect
		AZStd::unordered_map<AZ::Crc32, NewDSPEffectFunc> *m_dspLibFuncs;
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> *m_dspLibNames;
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioDeviceBus
	protected:
		void RegisterPlaybackLibrary(AZStd::string libname, AZ::Crc32 crc, OAudioDeviceProvider* ptr);
		OAudioDevice * NewDevice(AZ::Crc32 crc, long long device, double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void* userdata);
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetPlaybackLibraryNames() { return *(this->m_deviceLibNames); }
		AZStd::vector<OAudioDeviceInfo>& GetPlaybackDevices(AZ::Crc32 playbackLib);
		long long GetDefaultPlaybackDevice(AZ::Crc32 playbackLib);
		OAudioDeviceProvider* GetDeviceProvider(AZ::Crc32 playbackLib);
	protected:
		void SetMasterDevice(OAudioDevice * device);
		void SetStream(double samplerate, AudioFrame::Type audioFormat, void * userdata);
		void SetResampleQuality(AAResampleQuality quality);
		OAudioDeviceInfo GetDeviceInfo();
		long long PlaySource(AlternativeAudio::IAudioSource * source);
		void PauseSource(long long id);
		void ResumeSource(long long id);
		void StopSource(long long id);
		bool IsPlaying(long long id);
		AlternativeAudio::AudioSourceTime GetTime(long long id);
		void SetTime(long long id, double time);
	private:
		AZStd::unordered_map<AZ::Crc32, OAudioDeviceProvider*> *m_deviceProviders;
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> *m_deviceLibNames;
		AANullProvider m_nullProvider;
		AZStd::vector<OAudioDeviceInfo> m_NullDeviceInfo;
		OAudioDevice * m_MasterDevice;
	////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////
	private:
		DSP::InterleaveDSPEffect *interlaceDSP;
		DSP::DeinterleaveDSPEffect *deinterlaceDSP;
	private:
		ConvertAudioFrameFunc defaultConvert, currentConvert;
		MixAudioFramesFunc defaultMix, currentMix;
	};
}
