#pragma once

#include "AlternativeAudio\IDSPEffect.h"

namespace AlternativeAudio{
    namespace DSP {
		class InterleaveDSPEffect
			: public IDSPEffect {
		public:
			AZ_RTTI(InterleaveDSPEffect, "{F91C23F1-D0FA-4CDA-9B02-4F3C19B8AFBE}");
		public:
			InterleaveDSPEffect(void* userdata) {}
		public:
			AZStd::string GetName() { return "Interlace"; }
		public:
			void Process(AudioFrame::Type format, float * buffer, long long len);
			void ProcessFrame(AudioFrame::Type format, float * frame) {}
		public:
			int GetDSPSection() { return eDS_PerSource_BC | eDS_PerSource_AC | eDS_PerSource_ARS | eDS_Output; }
			DSP_ProcessType GetProcessType() { return eDPT_Buffer; }
		};

		class DeinterleaveDSPEffect
			: public IDSPEffect {
		public:
			AZ_RTTI(DeinterleaveDSPEffect, "{42EAC2FA-70EF-4B08-8DDC-1F8244F6AD16}");
		public:
			DeinterleaveDSPEffect(void* userdata) {}
		public:
			AZStd::string GetName() { return "Deinterlace"; }
		public:
			void Process(AudioFrame::Type format, float * buffer, long long len);
			void ProcessFrame(AudioFrame::Type format, float * frame) {}
		public:
			int GetDSPSection() { return eDS_PerSource_BC | eDS_PerSource_AC | eDS_PerSource_ARS | eDS_Output; }
			DSP_ProcessType GetProcessType() { return eDPT_Buffer; }
		};
	}
}