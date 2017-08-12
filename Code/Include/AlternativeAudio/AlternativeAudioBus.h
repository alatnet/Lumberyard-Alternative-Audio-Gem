#pragma once

#include <AzCore/EBus/EBus.h>
#include <AlternativeAudio\AudioFrame.h>
#include <AlternativeAudio\IAudioSource.h>
#include <AlternativeAudio\IDSPEffect.h>

namespace AlternativeAudio
{
	typedef AZStd::function<IAudioSource* (const char *, void*)> NewAudioSourceFunc;
	typedef AZStd::function<IDSPEffect* (void*)> NewDSPEffectFunc;

	class AlternativeAudioRequests
        : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;
        // Public functions                                         
	public: //basic audio library system
		virtual void RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr) = 0;
		virtual IAudioSource * NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata) = 0;
		virtual AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetAudioLibraryNames() = 0;
	public: //basic DSP library system
		virtual void RegisterDSPEffect(AZStd::string libname, AZ::Crc32 crc, NewDSPEffectFunc ptr) = 0;
		virtual IDSPEffect * NewDSPEffect(AZ::Crc32 crc, void* userdata) = 0;
		virtual AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetDSPEffectNames() = 0;
	public: //basic DSP system
		virtual bool AddDSPEffect(DSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot) = 0;
		virtual unsigned long long AddDSPEffectFreeSlot(DSPSection section, AZ::Crc32 crc, void* userdata) = 0;
		virtual IDSPEffect * GetDSPEffect(DSPSection section, unsigned long long slot) = 0;
		virtual bool RemoveDSPEffect(DSPSection section, unsigned long long slot) = 0;
		virtual void ProcessDSPEffects(DSPSection section, AudioFrame::Type format, float* buffer, long long len) = 0;
	public: //audio utilities
		virtual void ConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) = 0;
		virtual AudioFrame::Type GetAudioFormat(int numberOfChannels) = 0;
		virtual int GetNumberOfChannels(AudioFrame::Type type) = 0;
		virtual void MixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) = 0;
    };
    using AlternativeAudioRequestBus = AZ::EBus<AlternativeAudioRequests>;
} // namespace AlternativeAudio
