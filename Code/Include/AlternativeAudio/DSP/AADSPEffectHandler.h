#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore\RTTI\RTTI.h>
#include <AlternativeAudio\AAAudioFrame.h>
#include <AlternativeAudio\DSP\AADSPEffect.h>
#include <AlternativeAudio\AAFlagHandler.h>

namespace AlternativeAudio {
	//basic effect handler, useful for audio sources.
	class AADSPEffectHandler {
	public:
		AZ_RTTI(AADSPEffectHandler, "{0E3CCAFC-F2B0-4458-8E4C-FC9461D9A465}");
	public:
		bool AddEffect(AADSPEffect * effect, unsigned long long slot) {
			if (this->effects.count(slot) == 0) {
				this->effects[slot] = effect;
				effect->AddRef();
				return true; //effect has been added
			}
			return false; //effect slot in use.
		}
		unsigned long long AddEffectFreeSlot(AADSPEffect * effect) {
			if (this->effects.empty()) {
				this->effects[0] = effect;
				effect->AddRef();
				return 0;
			}

			//find an open slot
			auto end = this->effects.rbegin();
			unsigned long long open = end->first+1; //store the next open end index

			for (auto it = this->effects.begin(); it != --this->effects.end(); it++) {
				auto it2 = it;
				it2++;

				if (it->first+1 != it2->first) {
					//open slot
					open = it->first + 1;
					this->effects[open] = effect;
					return open;
				}
			}

			this->effects[open] = effect; //add effect to end
			return open;
		}
		AADSPEffect * GetEffect(unsigned long long slot) { return this->effects.at(slot); }
		bool RemoveEffect(unsigned long long slot) {
			if (this->effects.at(slot) != nullptr) {
				this->effects[slot]->Release();
				this->effects.erase(slot);
				return true; //effect has been removed.
			}
			return false; //no effect in that slot
		}
	protected:
		void ProcessEffects(AudioFrame::Type format, float* buffer, long long len, AAFlagHandler * flags) {
			for (std::pair<unsigned long long, AADSPEffect *> effect : this->effects) {
				switch (effect.second->GetProcessType()) {
				case eDPT_Buffer:
					effect.second->Process(format, buffer, len, flags);
					break;
				case eDPT_Frame:

					#define CASE_FORMAT(Format) \
						case AlternativeAudio::AudioFrame::Type::eT_##Format##: \
						{ \
							AlternativeAudio::AudioFrame::##Format##* buff = (AlternativeAudio::AudioFrame::##Format##*)buffer; \
							for (long long i = 0; i < len; i++) effect.second->ProcessFrame(format, (float*)&buff[i], flags); \
						}

					switch (format) {
						CASE_FORMAT(af1)
						CASE_FORMAT(af2)
						CASE_FORMAT(af21)
						CASE_FORMAT(af3)
						CASE_FORMAT(af31)
						CASE_FORMAT(af4)
						CASE_FORMAT(af41)
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
		//AZStd::unordered_map<unsigned long long, AADSPEffect*> effects;
		std::map<unsigned long long, AADSPEffect*> effects;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AADSPEffectHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AADSPEffectHandler>("AADSPEffectHandler")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("AddEffect", &AADSPEffectHandler::AddEffect)
				->Method("AddEffectFreeSlot", &AADSPEffectHandler::AddEffectFreeSlot)
				->Method("GetEffect", &AADSPEffectHandler::GetEffect)
				->Method("RemoveEffect", &AADSPEffectHandler::RemoveEffect);
		}
	};

	//basic effect handler for devices, useful for playback devices
	class AADSPDeviceEffectHandler {
	public:
		AZ_RTTI(AADSPDeviceEffectHandler, "{6DEE9F29-070F-42D1-8626-D13E7182B887}");
	public:
		AADSPDeviceEffectHandler(){
			//this->m_dspEffects = new AZStd::unordered_map<unsigned long long, AADSPEffect *>[eDS_Count];
			this->m_dspEffects = new std::map<unsigned long long, AADSPEffect *>[eDS_Count];
		}

		~AADSPDeviceEffectHandler(){
			for (int i = 0; i < eDS_Count; i++) {
				for (std::pair<unsigned long long, AADSPEffect *> effect : this->m_dspEffects[i])
					//delete effect.second;
					effect.second->Release();
				this->m_dspEffects[i].clear();
			}
			delete[] this->m_dspEffects;
		}
	public:
		bool AddEffect(AADSPSection section, AADSPEffect* effect, unsigned long long slot) {
			if (effect == nullptr) return false;
			int sectionInt = GetSection(section);
			if (this->m_dspEffects[sectionInt].count(slot) == 0) {
				if (effect->GetDSPSection() & section) {
					this->m_dspEffects[sectionInt][slot] = effect;
					effect->AddRef();
					return true; //dsp effect is added to slot specified
				}
				return false; //dsp effect is not suited for section specified
			}
			return false; //slot already has a dsp effect.
		}
		unsigned long long AddEffectFreeSlot(AADSPSection section, AADSPEffect* effect) {
			if (effect == nullptr) return -2;
			int sectionInt = GetSection(section);

			if (!(effect->GetDSPSection() & section)) {
				return -1; //dsp is not for this specific dsp section
			}

			effect->AddRef();
			if (this->m_dspEffects[sectionInt].empty()) {
				this->m_dspEffects[sectionInt][0] = effect;
				return 0;
			}

			//find an open slot
			auto end = this->m_dspEffects[sectionInt].rbegin();

			unsigned long long open = end->first + 1; //store the next open end index

			for (auto it = this->m_dspEffects[sectionInt].begin(); it != --this->m_dspEffects[sectionInt].end(); it++) {
				auto it2 = it;
				it2++;

				if (it->first + 1 != it2->first) {
					//open slot
					open = it->first + 1;
					this->m_dspEffects[sectionInt][open] = effect;
					return open;
				}
			}

			this->m_dspEffects[sectionInt][open] = effect; //add effect to end
			return open;
		}
		AADSPEffect * GetEffect(AADSPSection section, unsigned long long slot) {
			int sectionInt = GetSection(section);
			return this->m_dspEffects[sectionInt].at(slot);
		}
		bool RemoveEffect(AADSPSection section, unsigned long long slot) {
			int sectionInt = GetSection(section);
			if (this->m_dspEffects[sectionInt].at(slot) != nullptr) {
				this->m_dspEffects[sectionInt][slot]->Release();
				this->m_dspEffects[sectionInt].erase(slot);
				return true; //dsp effect removed.
			}
			return false; //there is no dsp effect in slot specified
		}
	protected:
		void ProcessEffects(AADSPSection section, AudioFrame::Type format, float* buffer, long long len, AAFlagHandler * flags) {
			int sectionInt = this->GetSection(section);
			for (std::pair<unsigned long long, AADSPEffect *> effect : this->m_dspEffects[sectionInt]) {
				switch (effect.second->GetProcessType()) {
				case eDPT_Buffer:
					effect.second->Process(format, buffer, len, flags);
					break;
				case eDPT_Frame:

				#define CASE_FORMAT(Format) \
					case AlternativeAudio::AudioFrame::Type::eT_##Format##: \
					{ \
						AlternativeAudio::AudioFrame::##Format##* buff = (AlternativeAudio::AudioFrame::##Format##*)buffer; \
						for (long long i = 0; i < len; i++) effect.second->ProcessFrame(format, (float*)&buff[i], flags); \
					}

					switch (format) {
						CASE_FORMAT(af1)
							CASE_FORMAT(af2)
							CASE_FORMAT(af21)
							CASE_FORMAT(af3)
							CASE_FORMAT(af31)
							CASE_FORMAT(af4)
							CASE_FORMAT(af41)
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
		int GetSection(AADSPSection section) {
			switch (section) {
			case eDS_PerSource_BC:
				return 0;
			case eDS_PerSource_AC:
				return 1;
			case eDS_PerSource_ARS:
				return 2;
			case eDS_Output:
				return 3;
			}
			return 0;
		}
	private:
		//AZStd::unordered_map<unsigned long long, AADSPEffect *> * m_dspEffects;
		std::map<unsigned long long, AADSPEffect*> * m_dspEffects;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AADSPDeviceEffectHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AADSPDeviceEffectHandler>("AADSPDeviceEffectHandler")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("AddEffect", &AADSPDeviceEffectHandler::AddEffect)
				->Method("AddEffectFreeSlot", &AADSPDeviceEffectHandler::AddEffectFreeSlot)
				->Method("GetEffect", &AADSPDeviceEffectHandler::GetEffect)
				->Method("RemoveEffect", &AADSPDeviceEffectHandler::RemoveEffect);
		}
	};
}