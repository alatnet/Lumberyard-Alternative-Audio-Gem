#pragma once

#include "AlternativeAudio\IDSPEffect.h"
#include "AlternativeAudio\DSP\VolumeDSPBus.h"

namespace AlternativeAudio{
    namespace DSP {
        class VolumeDSPEffect
			: public IDSPEffect
			, public VolumeDSPBus::Handler {
        public:
            AZ_RTTI(VolumeDSP, "{8ECA20AB-1A49-4572-A119-26FE5904E87A}");
        public:
			VolumeDSPEffect();
			VolumeDSPEffect(void* userdata);
			~VolumeDSPEffect();
		public:
			AZStd::string GetName() { return "Default Volume Control"; }
        public:
            void SetVol(float vol);
            float GetVol() { return this->m_vol; }
        public:
            void Process(AudioFrame::Type format, float * buffer, long long len);
            void ProcessFrame(AudioFrame::Type format, float * frame);
		public:
			int GetDSPSection() { return eDS_PerSource_BC | eDS_PerSource_AC | eDS_PerSource_ARS | eDS_Output; }
			DSP_ProcessType GetProcessType() { return eDPT_Buffer; }
		public:
			static void Reflect(AZ::SerializeContext* serialize) {
				serialize->Class<VolumeDSPEffect, IDSPEffect>()
					->Version(1)
					->Field("vol",&VolumeDSPEffect::m_vol);
			}
        private:
            float m_vol;
			//__m128 m_simd_vol;
            AZStd::mutex m_mutex;
		};
	}
}