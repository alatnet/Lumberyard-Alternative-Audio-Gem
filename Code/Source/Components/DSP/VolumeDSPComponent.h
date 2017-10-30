#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio\Components\DSP\VolumeDSPComponentBus.h>
#include <AlternativeAudio\Components\DSPEffectComponentBus.h>

#include <AlternativeAudio\Components\AAComponentTypes.h>

using namespace AlternativeAudio::Components;
using namespace AlternativeAudio::Components::DSP;

namespace AlternativeAudio {
	namespace DSP {
		class VolumeDSPComponent
			: public AZ::Component
			, public VolumeComponentBus::Handler
			, public DSPEffectComponentNotificationBus::Handler
			, public DSPEffectCustomSlotComponentBus::Handler
		{
		public:
			VolumeDSPComponent();
			~VolumeDSPComponent();
		public:
			//AZ_COMPONENT(VolumeDSPComponent, "{73717A4F-CE6E-4917-811B-9350216F483E}");
			AZ_COMPONENT(VolumeDSPComponent, VolumeComponentUUID, AZ::Component);

			static void Reflect(AZ::ReflectContext* context);

			static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
			static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
			static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
			static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);
		protected:
			////////////////////////////////////////////////////////////////////////
			// AudioSourceComponentBus interface implementation
			void SetVol(float vol);
			float GetVol();
			////////////////////////////////////////////////////////////////////////
		protected:
			////////////////////////////////////////////////////////////////////////
			// DSPEffectCustomSlotComponentBus interface implementation
			void UseCustomSlot(bool x);
			bool IsUsingCustomSlot();
			void SetSlot(unsigned long long slot);
			unsigned long long GetSlot();
			////////////////////////////////////////////////////////////////////////
		protected:
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
			unsigned long long m_DSPSlot;
			bool m_shared;
			AZStd::string m_tag;
			float m_vol;

			long long m_slot;
			bool m_customSlot;
		private:
			bool SharedVisibility() { return this->m_shared; }
			bool CustomSlotVisibilty() { return this->m_customSlot; }
			void resetCustomSlot();
			void reloadDSP();
		};
	}
}