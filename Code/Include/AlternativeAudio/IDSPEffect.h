#pragma once

#include <AzCore\RTTI\RTTI.h>
#include <AlternativeAudio\IErrorHandler.h>
#include <AlternativeAudio\AudioFrame.h>
#include <AzCore\std\string\string.h>
#include <AzCore\std\sort.h>
#include <smartptr.h>

namespace AlternativeAudio {
	//where the dsp can be used
	enum DSPSection {
		//eDS_PerSource = 1 << 0, //in IAudioSource GetFrames/GetFrame
		eDS_PerSource_BC = 1 << 0, //before converting audio frame
		eDS_PerSource_AC = 1 << 1, //after converting audio frame
		eDS_PerSource_ARS = 1 << 2, //after resampling
		eDS_Output = 1 << 3, //output buffer
		eDS_Count = 4
	};

	enum DSP_ProcessType {
		eDPT_Buffer, //entire buffer //allows for using SSE or other types of buffer processing
		eDPT_Frame //single frame //allows for stacking dsp processing calls and multi-threading
	};

	/*
	How the dsp sections work.
	-for each source
	--Get raw source frames
	--Apply PerSource_BC DSP
	---for each frame
	----for each DSP
	-----process DSP frame
	--Convert source frames to playback audio format.
	--Apply PerSource_AC DSP
	---for each frame
	----for each DSP
	-----process DSP frame
	--Resample if needed
	--Apply PerSource_ARS
	---for each frame
	----for each DSP
	-----process DSP frame
	--Mix frames with output buffer.
	-Apply Output DSP
	---for each frame
	----for each DSP
	-----process DSP frame
	*/

	/*
	for (IDSPEffect effect : effects){
		effect.Process(srcFormat, srcBuffer, srcLen);
		--for (long long i=0; i<srcLen; i++){
		--  dsp effect code
		--}
	}

	OR

	for (long long i = 0; i < len; i++){ //can be parallised
		for (IDSPEffect effect : effects){
			effect.ProcessFrame(srcFormat, frame[i]);
			-- dsp effect code
		}
	}
	*/

	class IDSPEffect : public IErrorHandler, public _i_reference_target_t {
	public:
		AZ_RTTI(IDSPEffect, "{056BD074-4330-42DB-A073-824EE637BD94}");
	public:
		//IDSPEffect(void* userdata) {}
	public:
		virtual AZStd::string GetName() = 0;
	public:
		//can implement either or both.
		//best to implement both and toggle GetProcessType based on platform features.
		virtual void Process(AudioFrame::Type format, float * buffer, long long len) {} //processes a range of frames (has a for loop)
		virtual void ProcessFrame(AudioFrame::Type format, float * frame) {} //processes a single frame (used in a for loop)
	public:
		virtual int GetDSPSection() = 0;
		virtual DSP_ProcessType GetProcessType() = 0;
	public:
		bool HasError() { return IErrorHandler::HasError(); }
		IError GetError() { return IErrorHandler::GetError(); }
	public:
		void AddRef() { _i_reference_target_t::AddRef(); }
		void Release() { _i_reference_target_t::Release(); }
	};

	class IDSPEffectHandler {
	public:
		AZ_RTTI(IDSPEffectHandler, "{0E3CCAFC-F2B0-4458-8E4C-FC9461D9A465}");
	public:
		bool AddEffect(IDSPEffect * effect, unsigned long long slot) {
			if (this->effects.count(slot) == 0) {
				this->effects[slot] = effect;
				effect->AddRef();
				return true; //effect has been added
			}
			return false; //effect slot in use.
		}
		unsigned long long AddEffectFreeSlot(IDSPEffect * effect) {
			if (this->effects.empty()) {
				this->effects[0] = effect;
				effect->AddRef();
				return 0;
			}

			//find an open slot

			return -1;
		}
		bool RemoveEffect(unsigned long long slot) {
			if (this->effects.at(slot) != nullptr) {
				this->effects[slot]->Release();
				this->effects.erase(slot);
				return true; //effect has been removed.
			}
			return false; //no effect in that slot
		}
	protected:
		void ProcessEffects(AudioFrame::Type format, float* buffer, long long len) {
			for (std::pair<unsigned long long, IDSPEffect *> effect : this->effects) {
				switch (effect.second->GetProcessType()) {
				case eDPT_Buffer:
					effect.second->Process(format, buffer, len);
					break;
				case eDPT_Frame:

					#define CASE_FORMAT(Format) \
						case AlternativeAudio::AudioFrame::Type::eT_##Format##: \
						{ \
							AlternativeAudio::AudioFrame::##Format##* buff = (AlternativeAudio::AudioFrame::##Format##*)buffer; \
							for (long long i = 0; i < len; i++) effect.second->ProcessFrame(format, (float*)&buff[i]); \
						}

					switch (format) {
						CASE_FORMAT(af1)
						CASE_FORMAT(af2)
						CASE_FORMAT(af21)
						CASE_FORMAT(af3)
						CASE_FORMAT(af31)
						CASE_FORMAT(af5)
						CASE_FORMAT(af51)
						CASE_FORMAT(af7)
						CASE_FORMAT(af71)
					}
					break;
					#undef CASE_FORMAT
				}
			}
		}
	private:
		//AZStd::unordered_map<unsigned long long, IDSPEffect*> effects;
		std::map<unsigned long long, IDSPEffect*> effects;
	};
}