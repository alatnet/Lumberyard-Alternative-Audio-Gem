#pragma once

#include <AzCore\Component\ComponentBus.h>

#include <AlternativeAudio\DSP\AADSPEffect.h>

namespace AlternativeAudio {
	namespace Components {
		class DSPEffectComponentEvents
			: public AZ::EntityComponentBus
		{
		public:
			virtual void SetDSPEffectCrc(AZ::Crc32 dsp, void* userdata) = 0;
			virtual void SetDSPEffect(AZStd::string dsp, void* userdata) = 0;
			virtual AADSPEffect* GetDSPEffect() = 0;
			virtual AZ::Crc32 GetDSPEffectCrc() = 0;
			virtual AZStd::string GetDSPEffectName() = 0;

			virtual void SetShared(bool shared) = 0;
			virtual void SetSharedTag(AZStd::string tag) = 0;
			virtual bool GetShared() = 0;
			virtual AZStd::string GetSharedTag() = 0;
		};

		using DSPEffectComponentBus = AZ::EBus<DSPEffectComponentEvents>;

		class DSPEffectCustomSlotComponentEvents
			: public AZ::EntityComponentBus {
		public:
			virtual void UseCustomSlot(bool x) = 0;
			virtual bool IsUsingCustomSlot() = 0;
			virtual void SetSlot(unsigned long long slot) = 0;
			virtual unsigned long long GetSlot() = 0;
		};

		using DSPEffectCustomSlotComponentBus = AZ::EBus<DSPEffectCustomSlotComponentEvents>;

		class DSPEffectComponentNotifications
			: public AZ::ComponentBus
		{
		public:
			virtual void SourceReloaded() = 0;
		};

		using DSPEffectComponentNotificationBus = AZ::EBus<DSPEffectComponentNotifications>;
	}
}