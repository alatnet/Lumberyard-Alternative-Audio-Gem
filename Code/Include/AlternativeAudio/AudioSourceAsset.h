#pragma once

#include <AzCore/RTTI/TypeInfo.h>
#include <AzFramework/Asset/SimpleAsset.h>

namespace AlternativeAudio {
	class AltAudioAsset {
	public:
		AZ_TYPE_INFO(AudioAsset, "{E27971A5-FE48-4201-9AD6-924C73488BE5}");
		static const char* GetFileFilter() {
			return "";
		}
	};

	using AltAudioAssetRef = AzFramework::SimpleAssetReference<AltAudioAsset>;
}

namespace AZ {
	AZ_TYPE_INFO_SPECIALIZE(AzFramework::SimpleAssetReference<AlternativeAudio::AltAudioAsset>, "{456156D2-B8DC-4E9A-BDBC-3DFC633FB8B2}");
}