#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio/AlternativeAudioBus.h>
#include "Built-in DSP\InterlaceDSP.h"

namespace AlternativeAudio {
	class AlternativeAudioSystemComponent
		: public AZ::Component
		, protected AlternativeAudioRequestBus::Handler {
	public:
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
	protected: //IAudioSource
		void RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr);
		IAudioSource * NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata);
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetAudioLibraryNames() { return *(this->m_sourceLibNames); }
	protected: //IDSPEffect
		void RegisterDSPEffect(AZStd::string libname, AZ::Crc32 crc, NewDSPEffectFunc ptr);
		IDSPEffect * NewDSPEffect(AZ::Crc32 crc, void* userdata);
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetDSPEffectNames() { return *(this->m_dspLibNames); }
	protected: //basic DSP system
		bool AddDSPEffect(DSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot);
		unsigned long long AddDSPEffectFreeSlot(DSPSection section, AZ::Crc32 crc, void* userdata);
		IDSPEffect * GetDSPEffect(DSPSection section, unsigned long long slot);
		bool RemoveDSPEffect(DSPSection section, unsigned long long slot);
		void ProcessDSPEffects(DSPSection section, AudioFrame::Type format, float* buffer, long long len);
	protected: //audio utilities
		void ConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len); //change this to a customizable function?
		AudioFrame::Type GetAudioFormat(int numberOfChannels);
		int GetNumberOfChannels(AudioFrame::Type type);
		void MixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len);
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////
	private: //IAudioSource
		AZStd::unordered_map<AZ::Crc32, NewAudioSourceFunc> *m_sourceLibFuncs;
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> *m_sourceLibNames;
	private: //IDSPEffect
		AZStd::unordered_map<AZ::Crc32, NewDSPEffectFunc> *m_dspLibFuncs;
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> *m_dspLibNames;
	private: //basic DSP system
		//AZStd::unordered_map<unsigned long long, IDSPEffect *> * m_dspEffects;
		std::map<unsigned long long, IDSPEffect*> * m_dspEffects;

	private:
		DSP::InterleaveDSPEffect *interlaceDSP;
		DSP::DeinterleaveDSPEffect *deinterlaceDSP;
	};
}
