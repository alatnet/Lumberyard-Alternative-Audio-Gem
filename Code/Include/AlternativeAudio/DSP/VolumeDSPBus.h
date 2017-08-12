#pragma once

#include <AzCore/EBus/EBus.h>
#include <AlternativeAudio\AudioFrame.h>
#include <AlternativeAudio\IDSPEffect.h>

namespace AlternativeAudio {
	namespace DSP {
		class VolumeDSPRequest
			: public AZ::EBusTraits {
		public:
			static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
			static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;

			using BusIdType = IDSPEffect *;
		// Public functions                                         
		public:
			virtual void SetVol(float vol) = 0;
			virtual float GetVol() = 0;
		};
		using VolumeDSPBus = AZ::EBus<VolumeDSPRequest>;
	}
} // namespace AlternativeAudio