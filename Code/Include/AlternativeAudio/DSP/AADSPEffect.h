#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/RTTI/TypeInfo.h>
#include <AzCore\RTTI\RTTI.h>
#include <AlternativeAudio\AAErrorHandler.h>
#include <AlternativeAudio\AAAttributeHandler.h>
#include <AlternativeAudio\AAAudioFrame.h>
#include <AlternativeAudio\AASmartRef.h>
#include <AzCore\std\string\string.h>
#include <AzCore\std\sort.h>

namespace AlternativeAudio {

	//How the dsp sections work.
	/*
	for each source
	-Get raw source frames
	-Apply PerSource_BC DSP (Before Convert)
	-Convert source frames to playback audio format. (i.e. 2ch to 4ch)
	-Apply PerSource_AC DSP (After Convert)
	-Resample if needed
	-Apply PerSource_ARS DSP (After ReSample)
	-Mix frames with output buffer.
	Apply Output DSP
	*/
	enum AADSPSection {
		eDS_PerSource_BC = 1 << 0, //before converting audio frame
		eDS_PerSource_AC = 1 << 1, //after converting audio frame
		eDS_PerSource_ARS = 1 << 2, //after resampling
		eDS_Output = 1 << 3, //output buffer
		eDS_Count = 4
		//eDS_PerSource = 1 << 4, //in IAudioSource GetFrames/GetFrame
	};

	//How process types work
	/*
	==eDPT_Buffer
	for (AADSPEffect effect : effects){
	  effect.Process(srcFormat, srcBuffer, srcLen);
	  --for (long long i=0; i<srcLen; i++){
	  --  dsp effect code
	  --}
	}

	OR

	==eDPT_Frame
	for (long long i = 0; i < len; i++){ //can be parallised
	  for (AADSPEffect effect : effects){
	    effect.ProcessFrame(srcFormat, frame[i]);
	    -- dsp effect code
	  }
	}
	*/
	enum AADSPProcessType {
		eDPT_Buffer, //entire buffer //allows for using SSE or other types of buffer processing
		eDPT_Frame //single frame //allows for stacking dsp processing calls and multi-threading
	};

	//effect returned by alternative audio
	class AADSPEffect : public AAErrorHandler, public AASmartRef {
	public:
		AZ_RTTI(AADSPEffect, "{056BD074-4330-42DB-A073-824EE637BD94}", AAErrorHandler, AASmartRef);
	public:
		virtual AZStd::string GetName() = 0;
	public:
		//can implement either or both.
		//best to implement both and toggle GetProcessType based on platform features.
		virtual void Process(AudioFrame::Type format, float * buffer, long long len, AAAttributeHandler * attr) {} //processes a range of frames (has a for loop)
		virtual void ProcessFrame(AudioFrame::Type format, float * frame, AAAttributeHandler * attr) {} //processes a single frame (used in a for loop)
	public:
		virtual int GetDSPSection() = 0;
		virtual AADSPProcessType GetProcessType() = 0;
	public:
		virtual operator AADSPEffect*() { return this; }
		virtual AADSPEffect* get() { return this; }
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AADSPEffect, AAErrorHandler, AASmartRef>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AADSPEffect>("AADSPEffect")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("GetName", &AADSPEffect::GetName)
				->Method("GetDSPSection", &AADSPEffect::GetDSPSection)
				->Method("GetProcessType", &AADSPEffect::GetProcessType)
				->Method("get", &AADSPEffect::get);
		}
	};
}

namespace AZ {
	AZ_TYPE_INFO_SPECIALIZE(AlternativeAudio::AADSPSection, "{95E86FD2-44FB-413B-A25D-9CE9CC1B5204}");
	AZ_TYPE_INFO_SPECIALIZE(AlternativeAudio::AADSPProcessType, "{DC351EF8-609A-4C7A-8AB0-01C37A8077D1}");
}