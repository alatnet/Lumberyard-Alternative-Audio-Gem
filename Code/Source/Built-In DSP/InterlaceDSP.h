#pragma once

#include "AlternativeAudio\DSP\AADSPEffect.h"

namespace AlternativeAudio{
    namespace DSP {
		class InterleaveDSPEffect
			: public AADSPEffect {
		public:
			AZ_RTTI(InterleaveDSPEffect, "{F91C23F1-D0FA-4CDA-9B02-4F3C19B8AFBE}");
		public:
			InterleaveDSPEffect() {}
			InterleaveDSPEffect(void* userdata) {}
		public:
			AZStd::string GetName() { return "AAInterlace"; }
		public:
			void Process(AudioFrame::Type format, float * buffer, long long len, AAFlagHandler * flags);
			void ProcessFrame(AudioFrame::Type format, float * frame, AAFlagHandler * flags) {}
		public:
			int GetDSPSection() { return eDS_PerSource_BC | eDS_PerSource_AC | eDS_PerSource_ARS | eDS_Output; }
			AADSPProcessType GetProcessType() { return eDPT_Buffer; }
		public:
			static void Reflect(AZ::SerializeContext* serialize) {
				serialize->Class<InterleaveDSPEffect, AADSPEffect>()
					->Version(0)
					->SerializerForEmptyClass();
			}
		};

		class DeinterleaveDSPEffect
			: public AADSPEffect {
		public:
			AZ_RTTI(DeinterleaveDSPEffect, "{42EAC2FA-70EF-4B08-8DDC-1F8244F6AD16}");
		public:
			DeinterleaveDSPEffect() {}
			DeinterleaveDSPEffect(void* userdata) {}
		public:
			AZStd::string GetName() { return "AADeinterlace"; }
		public:
			void Process(AudioFrame::Type format, float * buffer, long long len, AAFlagHandler * flags);
			void ProcessFrame(AudioFrame::Type format, float * frame, AAFlagHandler * flags) {}
		public:
			int GetDSPSection() { return eDS_PerSource_BC | eDS_PerSource_AC | eDS_PerSource_ARS | eDS_Output; }
			AADSPProcessType GetProcessType() { return eDPT_Buffer; }
		public:
			static void Reflect(AZ::SerializeContext* serialize) {
				serialize->Class<DeinterleaveDSPEffect, AADSPEffect>()
					->Version(0)
					->SerializerForEmptyClass();
			}
		};
	}
}