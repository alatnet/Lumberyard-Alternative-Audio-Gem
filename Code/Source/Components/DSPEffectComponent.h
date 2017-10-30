#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio\Components\DSPEffectComponentBus.h>
#include <AlternativeAudio\Components\AAComponentTypes.h>

using namespace AlternativeAudio::Components;

namespace AlternativeAudio {
	class DSPEffectComponent
		: public AZ::Component
		, public DSPEffectComponentBus::Handler
		, public DSPEffectComponentNotificationBus::Handler
		, public DSPEffectCustomSlotComponentBus::Handler
	{
	public:
		DSPEffectComponent();
		~DSPEffectComponent();
	public:
		//AZ_COMPONENT(DSPEffectComponent, "{73803528-69F3-4017-BDE9-47E4CFD1AD6A}");
		AZ_COMPONENT(DSPEffectComponent, DSPEffectComponentUUID, AZ::Component);

		static void Reflect(AZ::ReflectContext* context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
	protected:
		////////////////////////////////////////////////////////////////////////
		// DSPEffectComponentBus interface implementation
		void SetDSPEffectCrc(AZ::Crc32 dsp, void* userdata);
		void SetDSPEffect(AZStd::string dsp, void* userdata);
		AADSPEffect* GetDSPEffect();
		AZ::Crc32 GetDSPEffectCrc();
		AZStd::string GetDSPEffectName();
		void SetShared(bool shared);
		void SetSharedTag(AZStd::string tag);
		bool GetShared();
		AZStd::string GetSharedTag();
		////////////////////////////////////////////////////////////////////////
	protected:
		////////////////////////////////////////////////////////////////////////
		// DSPEffectCustomSlotComponentBus interface implementation
		void UseCustomSlot(bool x);
		bool IsUsingCustomSlot();
		void SetSlot(unsigned long long slot);
		unsigned long long GetSlot();
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
		void * m_pDSPUserdata;

		bool m_shared;
		AZStd::string m_tag;

		bool m_checkButton;

		long long m_slot;
		bool m_customSlot;
	private:
		void CheckCrc();
		void reloadDSP();
		bool SharedVisibility() { return this->m_shared; }
		bool CustomSlotVisibilty() { return this->m_customSlot; }
		void resetCustomSlot();
	};
}