#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio\Components\DSPEffectComponentBus.h>

namespace AlternativeAudio {
	class DSPEffectComponent
		: public AZ::Component
		, public Components::DSPEffectComponentBus::Handler
		, public Components::DSPEffectComponentNotificationBus::Handler
	{
	public:
		DSPEffectComponent();
		~DSPEffectComponent();
	public:
		AZ_COMPONENT(DSPEffectComponent, "{73803528-69F3-4017-BDE9-47E4CFD1AD6A}");

		static void Reflect(AZ::ReflectContext* context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
	protected:
		////////////////////////////////////////////////////////////////////////
		// DSPEffectComponentBus interface implementation
		void SetDSPEffectCrc(AZ::Crc32 dsp);
		void SetDSPEffect(AZStd::string dsp);
		AADSPEffect* GetDSPEffect();
		AZ::Crc32 GetDSPEffectCrc();
		AZStd::string GetDSPEffectName();
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// DSPEffectComponentNotificationBus interface implementation
		void SourceReloaded();
		////////////////////////////////////////////////////////////////////////
	protected:
		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////
	private:
		AADSPEffect* m_pDSP;
		AZ::Crc32 m_DSPCrc;
		AZStd::string m_DSPlib;
		unsigned long long m_DSPSlot;

		bool m_checkButton;
	private:
		void CheckCrc();
		void reloadDSP();
	};
}