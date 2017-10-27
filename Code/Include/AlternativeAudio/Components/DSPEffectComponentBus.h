#pragma once

#include <AzCore\Component\ComponentBus.h>

#include <AlternativeAudio\DSP\AADSPEffect.h>

namespace AlternativeAudio {
	namespace Components {
		//const char * DSPEffectComponentUUID = "{73803528-69F3-4017-BDE9-47E4CFD1AD6A}";

		class DSPEffectComponentEvents
			: public AZ::EntityComponentBus
			//: public AZ::EBusTraits
		{
		/*public:
			virtual ~DSPEffectComponentEvents() = default;
			static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;
			typedef AZ::u32 BusIdType;*/
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

		class DSPEffectComponentNotifications
			: public AZ::ComponentBus
		{
		public:
			virtual void SourceReloaded() = 0;
		};

		using DSPEffectComponentNotificationBus = AZ::EBus<DSPEffectComponentNotifications>;
	}
}