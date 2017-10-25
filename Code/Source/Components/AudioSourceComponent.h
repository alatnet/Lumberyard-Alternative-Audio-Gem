#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio\Components\AudioSourceComponentBus.h>

namespace AlternativeAudio {
	class AudioSourceComponent
		: public AZ::Component
		, public Components::AudioSourceComponentBus::Handler
	{
	public:
		AudioSourceComponent();
		~AudioSourceComponent();
	public:
		AZ_COMPONENT(AudioSourceComponent, "{57C8594B-0D13-4555-AED1-769D59A55A09}");

		static void Reflect(AZ::ReflectContext* context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

	protected:
		////////////////////////////////////////////////////////////////////////
		// AudioSourceComponentBus interface implementation
		void Play();
		void PlaySFX();
		void Pause();
		void Resume();
		void Stop();
		bool IsPlaying();
		void SetTime(double time);
		AudioSourceTime GetTime();

		void SetPath(AZStd::string path);
		AZStd::string GetPath();

		void SetLibraryCrc(AZ::Crc32 lib);
		AZ::Crc32 GetLibraryCrc();
		void SetLibrary(AZStd::string lib);
		AZStd::string GetLibrary();

		IAudioSource* GetSource();

		void SetDevice(OAudioDevice* device);
		////////////////////////////////////////////////////////////////////////
	protected:
		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////
	private:
		IAudioSource * m_pSrc;
		OAudioDevice * m_pDevice;
		AZStd::string m_path;
		AZ::Crc32 m_libCrc;
		AZStd::string m_lib;

		unsigned long long m_srcID;

		bool m_checkButton;
	private:
		void CheckCrc();
		void reloadSource();
	};
}