
#pragma once

#include <AzCore/EBus/EBus.h>

#include <AlternativeAudio\IAudioSource.h>

namespace AlternativeAudio
{
	typedef AZStd::function<IAudioSource* (const char *, void*)> NewAudioSourceFunc;

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
	public: //audio utilities
		virtual void ConvertAudioFrame(AudioFrame::Frame* from, AudioFrame::Frame* to) = 0;
    };
    using AlternativeAudioRequestBus = AZ::EBus<AlternativeAudioRequests>;
} // namespace AlternativeAudio
