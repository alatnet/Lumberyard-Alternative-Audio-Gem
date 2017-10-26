#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore\RTTI\RTTI.h>
#include <AlternativeAudio\AAErrorHandler.h>
#include <AlternativeAudio\AAFlagHandler.h>
#include <AlternativeAudio\AAAudioFrame.h>
#include <AlternativeAudio\DSP\AADSPEffect.h>
#include <AlternativeAudio\DSP\AADSPEffectHandler.h>
#include <AlternativeAudio\AASmartRef.h>

namespace AlternativeAudio {

	//class to deal with reading audio source from a library.
	//input source
	class IAudioSourceLib
		: public AAErrorHandlerLib
		, public AAFlagHandler
		, public AASmartRef
	{
	public:
		AZ_RTTI(IAudioSourceLib, "{FC787633-E346-4547-9A30-311C2E5767AF}", AAErrorHandlerLib, AAFlagHandler, AASmartRef);
	public:
		//seek to a position on the audio source (in frames)
		virtual bool Seek(long long position) = 0;
		/*
		get the frames of the audio source
		params:
		- long long framesToRead - number of frames to read
		- float * buff - pointer to a buffer to where the frames are transfered to.
		returns:
		- long long - number of frames read.
		*/
		virtual long long GetFrames(long long framesToRead, float* buff) = 0;
		/*
		get a single frame of the audio source
		params:
		- float * frame - pointer to a bufferr to where the frame is transfered to.
		returns:
		- bool - whether the frame was read or not.
		*/
		virtual bool GetFrame(float* frame) = 0;

		//returns the sample rate of the audio source.  Common sample rates: 44100, 48000, 192000
		virtual double GetSampleRate() = 0;
		//returns the audio frame type (how many channels the audio source has).
		//virtual AudioFrame::Type GetFrameType() = 0;
		//virtual aztypeid GetFrameType() = 0;
		virtual const AlternativeAudio::AudioFrame::Type GetFrameType() = 0;

		//returns the length of the audio source in a human readable format.
		virtual AudioSourceTime GetLength() = 0;
		//returns the length of the audio source in frames.
		virtual long long GetFrameLength() = 0;
	};

	//input source
	class IAudioSource
		: public AAErrorHandler
		, public AAFlagHandler
		, public AADSPEffectHandler
		, public AASmartRef
	{
	public:
		AZ_RTTI(IAudioSource, "{FA8714F6-E26F-4420-9230-F46F84A99320}", AAErrorHandler, AAFlagHandler, AADSPEffectHandler, AASmartRef);
	public:
		virtual bool Seek(long long position) = 0;
		virtual long long GetFrames(long long framesToRead, float* buff) = 0;
		virtual bool GetFrame(float* frame) = 0;
		virtual double GetSampleRate() = 0;
		virtual const AlternativeAudio::AudioFrame::Type GetFrameType() = 0;
		virtual AudioSourceTime GetLength() = 0;
		virtual long long GetFrameLength() = 0;
	public:
			static void Reflect(AZ::SerializeContext* serialize) {
				serialize->Class<IAudioSource, AAErrorHandler, AAFlagHandler, AADSPEffectHandler>()
					->Version(0)
					->SerializerForEmptyClass();
			}
			static void Behavior(AZ::BehaviorContext* behaviorContext) {
				///audio source
				behaviorContext->Class<IAudioSource>("IAudioSource")
					->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
					->Method("GetSampleRate", &IAudioSource::GetSampleRate)
					->Method("GetFrameType", &IAudioSource::GetFrameType)
					->Method("GetLength", &IAudioSource::GetLength)
					->Method("GetFrameLength", &IAudioSource::GetFrameLength);
			}
	};
}