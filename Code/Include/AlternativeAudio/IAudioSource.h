#pragma once

#include <AzCore\RTTI\RTTI.h>

namespace AlternativeAudio {
	namespace AudioFrame {
		//base type
		typedef void Frame;

		//mono channel
		struct af1 {
			float mono{ 0.0f };
		};

		// 2 channel (left, right) - common
		struct af2 {
			float left{ 0.0f };
			float right{ 0.0f };
		};

		// 2.1 channel (left, right, sub)
		struct af21 {
			float left{ 0.0f };
			float right{ 0.0f };
			float sub{ 0.0f };
		};

		// 3 channel (left, right, center)
		struct af3 {
			float left{ 0.0f };
			float right{ 0.0f };
			float center{ 0.0f };
		};

		// 3.1 channel (left, right, center, subwoofer)
		struct af31 {
			float left{ 0.0f };
			float right{ 0.0f };
			float center{ 0.0f };
			float sub{ 0.0f };
		};

		// 5 channel (left, right, center, back left, back right)
		struct af5 {
			union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};
			float center;
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
		};

		// 5.1 channel (left, right, center, back left, back right, subwoofer)
		struct af51 {
			union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};
			float center;
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
			float sub;
		};

		// 7 channel  (left, right, center, side left, side right, back left, back right)
		struct af7 {
			union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};
			float center;
			union {
				struct {
					float left;
					float right;
				} side;
				struct {
					float sleft;
					float sright;
				};
			};
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
		};

		// 7.1 channel (left, right, center, side left, side right, back left, back right, subwoofer)
		struct af71 {
			union {
				struct {
					float left;
					float right;
				} front;
				struct {
					float left;
					float right;
				};
			};
			float center;
			union {
				struct {
					float left;
					float right;
				} side;
				struct {
					float sleft;
					float sright;
				};
			};
			union {
				struct {
					float left;
					float right;
				} back;
				struct {
					float bleft;
					float bright;
				};
			};
			float sub;
		};

		//audio frame types
		enum Type {
			eT_af1,
			eT_af2,
			eT_af21,
			eT_af3,
			eT_af31,
			eT_af5,
			eT_af51,
			eT_af7,
			eT_af71
		};
	}

	struct AudioSourceTime {
		int hrs{ 0 };
		int minutes{ 0 };
		double sec{ 0.0 };
		double totalSec{ 0.0 };
		AZ_RTTI(AudioSourceTime, "{90F357E5-05D2-4B41-B957-F12B62E28979}");

		int GetHours() { return this->hrs; }
		int GetMinutes() { return this->minutes; }
		double GetSeconds() { return this->sec; }
		double GetTotalSeconds() { return this->totalSec; }
	};

	struct AudioSourceError {
		int code{ 0 };
		const char * str{ "NoError" };
		AZ_RTTI(AudioSourceError, "{D7705870-4EA0-4DA9-8891-1A728BA56FC4}");

		int GetCode() { return this->code; }
		const char * GetStr() { return this->str; }
	};

	enum EAudioSourceFlags {
		eAF_None = 0,
		eAF_Loop = 1 << 0,
		eAF_LoopSection = 1 << 1,
		eAF_PausedOnStart = 1 << 2
	};

	class IAudioSource {
	public:
		AZ_RTTI(IAudioSource, "{FA8714F6-E26F-4420-9230-F46F84A99320}");
	public: //Developers, call this to initialize variables or initialize the variables yourself.
		IAudioSource() : m_flags(0), m_hasError(false) {}
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
		/*
		set the flags for the audio source.
		params:
		- int - flags to set (usually ored "|" together).
		See: EAudioSourceFlags
		*/
		void SetFlags(int flags) { this->m_flags = flags; }
		//returns the flags set for the audio source.
		int GetFlags() { return this->m_flags; }

		//virtual void GetMemoryUsage(ICrySizer* pSizer) const = 0;
	public: //error checking
		//returns if the audio source has an error.
		bool HasError() { return this->m_hasError; }
		/*
		returns what the error is in a human readable format.
		automatically clears error when there are no more errors to retrieve.
		*/
		AudioSourceError GetError() {
			if (this->m_errors.size() == 0) {
				AudioSourceError noErr;
				this->m_hasError = false;
				return noErr;
			}

			AudioSourceError ret = this->m_errors.at(this->m_errors.size() - 1);
			this->m_errors.pop_back();

			if (this->m_errors.size() == 0) this->m_hasError = false;

			return ret;
		}
	protected:
		int m_flags;
	protected:
		//if the audio source has an error, this variable will be set.
		bool m_hasError;
		/*
		push an error to the list of errors.
		automatically sets the m_hasError variable.
		params:
		- int errorCode - error code for the specific audio source (library dependent).
		- const char * errorStr - human readable format for the error code.
		*/
		void pushError(int errorCode, const char * errorStr) {
			AudioSourceError err;
			err.code = errorCode;
			err.str = errorStr;
			this->m_errors.push_back(err);
			this->m_hasError = true;
		}
	private:
		AZStd::vector<AudioSourceError> m_errors;
	};
}