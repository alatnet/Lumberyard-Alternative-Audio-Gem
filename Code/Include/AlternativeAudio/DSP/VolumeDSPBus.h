#pragma once

#include <AzCore/EBus/EBus.h>
#include <AlternativeAudio\AAAudioFrame.h>
#include <AlternativeAudio\DSP\AADSPEffect.h>

namespace AlternativeAudio {
	namespace DSP {
		class VolumeDSPRequest
			: public AZ::EBusTraits {
		public:
			static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;
			static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::ById;

			using BusIdType = AADSPEffect *;
		// Public functions                                         
		public:
			virtual void SetVol(float vol) = 0;
			virtual float GetVol() = 0;
		};
		using VolumeDSPBus = AZ::EBus<VolumeDSPRequest>;
	}
} // namespace AlternativeAudio