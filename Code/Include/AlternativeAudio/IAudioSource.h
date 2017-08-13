#pragma once

#include <AzCore\RTTI\RTTI.h>
#include <AlternativeAudio\IErrorHandler.h>
#include <AlternativeAudio\AudioFrame.h>
#include <AlternativeAudio\IDSPEffect.h>

namespace AlternativeAudio {
	class IAudioSource
		: public IErrorHandler
		, public IDSPEffectHandler {
	public:
		AZ_RTTI(IAudioSource, "{FA8714F6-E26F-4420-9230-F46F84A99320}");
	public:
		//IAudioSource(const char * path, void* userdata) {}
	//public: //Developers, call this to initialize variables or initialize the variables yourself.
		IAudioSource() : m_flags(0) {}
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

	public:
		bool AddEffect(IDSPEffect * effect, unsigned long long slot) { return IDSPEffectHandler::AddEffect(effect, slot); }
		unsigned long long AddEffectFreeSlot(IDSPEffect * effect) { return IDSPEffectHandler::AddEffectFreeSlot(effect); }
		bool RemoveEffect(unsigned long long slot) { return IDSPEffectHandler::RemoveEffect(slot); }
	public:
		/*
		set the flags for the audio source.
		params:
		- int - flags to set (usually ored "|" together).
		See: EAudioSourceFlags
		*/
		void SetFlags(int flags) { this->m_flags = flags; }
		//returns the flags set for the audio source.
		int GetFlags() { return this->m_flags; }
	protected:
		int m_flags;

	public:
		bool HasError() { return IErrorHandler::HasError(); }
		IError GetError() { return IErrorHandler::GetError(); }

		//virtual void GetMemoryUsage(ICrySizer* pSizer) const = 0;
	//public: //error checking
	//	//returns if the audio source has an error.
	//	bool HasError() { return this->m_hasError; }
	//	/*
	//	returns what the error is in a human readable format.
	//	automatically clears error when there are no more errors to retrieve.
	//	*/
	//	AudioSourceError GetError() {
	//		if (this->m_errors.size() == 0) {
	//			AudioSourceError noErr;
	//			this->m_hasError = false;
	//			return noErr;
	//		}
	//
	//		AudioSourceError ret = this->m_errors.at(this->m_errors.size() - 1);
	//		this->m_errors.pop_back();
	//
	//		if (this->m_errors.size() == 0) this->m_hasError = false;
	//
	//		return ret;
	//	}
	//protected:
	//	int m_flags;
	//protected:
	//	//if the audio source has an error, this variable will be set.
	//	bool m_hasError;
	//	/*
	//	push an error to the list of errors.
	//	automatically sets the m_hasError variable.
	//	params:
	//	- int errorCode - error code for the specific audio source (library dependent).
	//	- const char * errorStr - human readable format for the error code.
	//	*/
	//	void pushError(int errorCode, const char * errorStr) {
	//		AudioSourceError err;
	//		err.code = errorCode;
	//		err.str = errorStr;
	//		this->m_errors.push_back(err);
	//		this->m_hasError = true;
	//	}
	//private:
	//	AZStd::vector<AudioSourceError> m_errors;
	};
}