#ifndef ALTERNATIVE_AUDIO_ATTRIBUTES_H
#define ALTERNATIVE_AUDIO_ATTRIBUTES_H

#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore/Math/Crc.h>

namespace AlternativeAudio {
	namespace Attributes {
		namespace Source {
			const static AZ::Crc32 Loop = AZ_CRC("Loop", 0xa15f1dee); //boolean
			const static AZ::Crc32 PausedOnStart = AZ_CRC("PausedOnStart", 0xc0724055); //boolean
			const static AZ::Crc32 Deinterlaced = AZ_CRC("Deinterlaced", 0xb8b3cbfc); //boolean
		}
	}
}

#endif