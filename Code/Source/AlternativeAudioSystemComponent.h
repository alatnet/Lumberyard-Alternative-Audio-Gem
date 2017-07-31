
#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio/AlternativeAudioBus.h>

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
	protected:
		void RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr);
		IAudioSource * NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata);
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>& GetAudioLibraryNames() { return *(this->m_libnames); }
	protected:
		void ConvertAudioFrame(AudioFrame::Frame* from, AudioFrame::Frame* to);
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////
	private:
		AZStd::unordered_map<AZ::Crc32, NewAudioSourceFunc> *m_libfuncs;
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> *m_libnames;
	};
}
