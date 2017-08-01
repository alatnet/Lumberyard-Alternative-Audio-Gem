#pragma once

#include <AzCore\RTTI\RTTI.h>

namespace AlternativeAudio {
	namespace AudioFrame {
		//base struct, do not use EVER!
		struct Frame {
			AZ_RTTI(Frame, "{9747F891-4AC6-4DB3-906D-C3F53B3FBF28}");
		};

		//mono channel
		struct af1 : Frame {
			float mono{ 0.0f };
			AZ_RTTI(af1, "{B76004EE-F9A3-4F52-9996-D61FDD43A4B6}");
		};

		// 2 channel (left, right) - common
		struct af2 : Frame {
			float left{ 0.0f };
			float right{ 0.0f };
			AZ_RTTI(af2, "{AF2F4104-FD46-4BEB-8ECD-F07C86E0BA07}");
		};

		// 2.1 channel (left, right, sub)
		struct af21 : Frame {
			float left{ 0.0f };
			float right{ 0.0f };
			float sub{ 0.0f };
			AZ_RTTI(af21, "{CAE0ABB1-3DBB-4C29-B4BB-EB999514F8B3}");
		};

		// 3 channel (left, right, center)
		struct af3 : Frame {
			float left{ 0.0f };
			float right{ 0.0f };
			float center{ 0.0f };
			AZ_RTTI(af3, "{1A505C93-D6A5-4DB2-82EF-E803768C2AE2}");
		};

		// 3.1 channel (left, right, center, subwoofer)
		struct af31 : Frame {
			float left{ 0.0f };
			float right{ 0.0f };
			float center{ 0.0f };
			float sub{ 0.0f };
			AZ_RTTI(af31, "{73356738-D86B-4CE7-B8B1-7506630C80A2}");
		};

		// 5 channel (left, right, center, back left, back right)
		struct af5 : Frame {
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
			AZ_RTTI(af5, "{CD03693F-1979-48F8-AC0A-D3B22E310255}");
		};

		// 5.1 channel (left, right, center, back left, back right, subwoofer)
		struct af51 : Frame {
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
			AZ_RTTI(af51, "{CBBDAA85-7B49-404C-AAB1-6B7C4F30A149}");
		};

		// 7 channel  (left, right, center, side left, side right, back left, back right)
		struct af7 : Frame {
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
			AZ_RTTI(af7, "{509185CA-CF1D-4F61-95AE-F0A2426AD4A9}");
		};

		// 7.1 channel (left, right, center, side left, side right, back left, back right, subwoofer)
		struct af71 : Frame {
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
			AZ_RTTI(af71, "{DA284BAA-F8CD-4F6C-A60A-D60E4208BE7F}");
		};

		//audio frame types - how many channels an audio frame has
		/*enum Type {
			eAFT_None = 0,	//no audio channels - can be an error indicator
			eAFT_Mono = 1,	// mono - one channel
			eAFT_2 = 2,		// 2 channels - left, right
			eAFT_21 = 3,	// 2.1 channels - left, right, subwoofer
			eAFT_3 = 3,		// 3 channels - left. right, center
			eAFT_31 = 4,	// 3.1 channels - left, right, center, subwoofer
			eAFT_5 = 5,		// 5 channels - left, right, center, back left, back right
			eAFT_51 = 6,	// 5.1 channels - left, right, center, back left, back right, subwoofer
			eAFT_7 = 7,		// 7 channels - left, right, center, side left, side right, back left, back right
			eAFT_71 = 8,	// 7.1 channels - left, right, center, side left, side right, back left, back right, subwoofer
			eAFT_Max = 8	//max audio channels - port audio (and most high end setups) have a max of 8 channels.
		};*/
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
		virtual const AZ::Uuid& GetFrameType() = 0;

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