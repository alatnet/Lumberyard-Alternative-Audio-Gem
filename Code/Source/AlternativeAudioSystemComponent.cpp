#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "AlternativeAudioSystemComponent.h"
#include "AlternativeAudio\AudioSourceAsset.h"

#include <AzCore\RTTI\RTTI.h>

namespace AlternativeAudio {
	AlternativeAudioSystemComponent::~AlternativeAudioSystemComponent() {
		this->m_libfuncs->clear();
		delete this->m_libfuncs;

		this->m_libnames->clear();
		delete this->m_libnames;
	}

	void AlternativeAudioSystemComponent::Reflect(AZ::ReflectContext* context) {
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
			serialize->Class<AlternativeAudioSystemComponent, AZ::Component>()
				->Version(0)
				->SerializerForEmptyClass();

		#define SerializeEmpty(c) \
				serialize->Class<##c##>() \
					->Version(0) \
					->SerializerForEmptyClass()

			SerializeEmpty(AudioSourceTime);
			SerializeEmpty(AudioSourceError);

			//audio frames
			/*SerializeEmpty(AudioFrame::af1);
			SerializeEmpty(AudioFrame::af2);
			SerializeEmpty(AudioFrame::af21);
			SerializeEmpty(AudioFrame::af3);
			SerializeEmpty(AudioFrame::af31);
			SerializeEmpty(AudioFrame::af5);
			SerializeEmpty(AudioFrame::af51);
			SerializeEmpty(AudioFrame::af7);
			SerializeEmpty(AudioFrame::af71);*/

		#undef SerializeEmpty

			if (AZ::EditContext* ec = serialize->GetEditContext()) {
				ec->Class<AlternativeAudioSystemComponent>("AlternativeAudio", "Provides an alternative audio system for usage in lumberyard.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Category, "Audio")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					;
			}
		}

		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			behaviorContext->Class<AlternativeAudioSystemComponent>("AlternativeAudio")
				->Enum<EAudioSourceFlags::eAF_Loop>("eAF_Loop")
				->Enum<EAudioSourceFlags::eAF_LoopSection>("eAF_LoopSection")
				->Enum<EAudioSourceFlags::eAF_PausedOnStart>("eAF_PausedOnStart");

			behaviorContext->Class<AudioSourceTime>("AudioSourceTime")
				->Method("GetHours", &AudioSourceTime::GetHours)
				->Method("GetMinutes", &AudioSourceTime::GetMinutes)
				->Method("GetSeconds", &AudioSourceTime::GetSeconds)
				->Method("GetTotalSeconds", &AudioSourceTime::GetTotalSeconds);

			behaviorContext->Class<AudioSourceError>("AudioSourceError")
				->Method("GetCode", &AudioSourceError::GetCode)
				->Method("GetStr", &AudioSourceError::GetStr);

			//audio frame types
			behaviorContext->Class<AudioFrame::Type>("AudioFrame")
				->Enum<AudioFrame::Type::eT_af1>("af1")
				->Enum<AudioFrame::Type::eT_af2>("af2")
				->Enum<AudioFrame::Type::eT_af21>("af21")
				->Enum<AudioFrame::Type::eT_af3>("af3")
				->Enum<AudioFrame::Type::eT_af31>("af31")
				->Enum<AudioFrame::Type::eT_af5>("af5")
				->Enum<AudioFrame::Type::eT_af51>("af51")
				->Enum<AudioFrame::Type::eT_af7>("af7")
				->Enum<AudioFrame::Type::eT_af71>("af71");

			//audio source
			behaviorContext->Class<IAudioSource>("IAudioSource")
				->Method("GetSampleRate", &IAudioSource::GetSampleRate)
				->Method("GetFrameType", &IAudioSource::GetFrameType)
				->Method("GetLength", &IAudioSource::GetLength)
				->Method("GetFrameLength", &IAudioSource::GetFrameLength)
				->Method("SetFlags", &IAudioSource::SetFlags)
				->Method("GetFlags", &IAudioSource::GetFlags)
				->Method("HasError", &IAudioSource::HasError)
				->Method("GetError", &IAudioSource::GetError);

			//alternative audio bus
			behaviorContext->EBus<AlternativeAudioRequestBus>("AlternativeAudioBus")
				->Event("NewAudioSource", &AlternativeAudioRequestBus::Events::NewAudioSource)
				->Event("GetAudioLibraryNames", &AlternativeAudioRequestBus::Events::GetAudioLibraryNames);
		}
	}

	void AlternativeAudioSystemComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC("AlternativeAudioService"));
	}

	void AlternativeAudioSystemComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
		incompatible.push_back(AZ_CRC("AlternativeAudioService"));
	}

	void AlternativeAudioSystemComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required) {
		(void)required;
	}

	void AlternativeAudioSystemComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent) {
		(void)dependent;
	}

	void AlternativeAudioSystemComponent::Init() {
		this->m_libfuncs = new AZStd::unordered_map<AZ::Crc32, NewAudioSourceFunc>();
		this->m_libnames = new AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>>();
	}

	void AlternativeAudioSystemComponent::Activate() {
		AlternativeAudioRequestBus::Handler::BusConnect();
	}

	void AlternativeAudioSystemComponent::Deactivate() {
		AlternativeAudioRequestBus::Handler::BusDisconnect();
	}

	////////////////////////////////////////////////////////////////////////
	// AlternativeAudioRequestBus interface implementation
	void AlternativeAudioSystemComponent::RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr) {
		this->m_libfuncs->insert({ crc, ptr });
		this->m_libnames->push_back({ libname, crc });

		//build filetypes for audio asset
	}
	IAudioSource * AlternativeAudioSystemComponent::NewAudioSource(AZ::Crc32 crc, const char * path, void* userdata) {
		auto funcEntry = this->m_libfuncs->find(crc);
		if (funcEntry != this->m_libfuncs->end()) return funcEntry->second(path, userdata);
		return nullptr;
	}

	void AlternativeAudioSystemComponent::ConvertAudioFrame(AudioFrame::Frame* from, AudioFrame::Frame* to, AudioFrame::Type inType, AudioFrame::Type outType, long long len) {
		if (inType == AudioFrame::Type::eT_af1) { //Mono to...
			AudioFrame::af1 *in = (AudioFrame::af1*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1 * ret = (AudioFrame::af1*)(to);
				for (int i=0;i<len;i++) ret[i].mono = in[i].mono;
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].mono;
					ret[i].right = in[i].mono;
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].mono;
					ret[i].right = in[i].mono;
					ret[i].sub = in[i].mono;
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].mono;
					ret[i].right = in[i].mono;
					ret[i].center = in[i].mono;
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].mono;
					ret[i].right = in[i].mono;
					ret[i].center = in[i].mono;
					ret[i].sub = in[i].mono;
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].mono;
					ret[i].front.right = in[i].mono;
					ret[i].center = in[i].mono;
					ret[i].back.left = in[i].mono / 2;
					ret[i].back.right = in[i].mono / 2;
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].mono;
					ret[i].front.right = in[i].mono;
					ret[i].center = in[i].mono;
					ret[i].back.left = in[i].mono / 2;
					ret[i].back.right = in[i].mono / 2;
					ret[i].sub = in[i].mono;
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].mono;
					ret[i].front.right = in[i].mono;
					ret[i].center = in[i].mono;
					ret[i].side.left = in[i].mono / 2;
					ret[i].side.right = in[i].mono / 2;
					ret[i].back.left = in[i].mono / 3;
					ret[i].back.right = in[i].mono / 3;
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].mono;
					ret[i].front.right = in[i].mono;
					ret[i].center = in[i].mono;
					ret[i].side.left = in[i].mono / 2;
					ret[i].side.right = in[i].mono / 2;
					ret[i].back.left = in[i].mono / 3;
					ret[i].back.right = in[i].mono / 3;
					ret[i].sub = in[i].mono;
				}
			}
		} else if (inType == AudioFrame::Type::eT_af2) { //2 channels to...
			AudioFrame::af2 *in = (AudioFrame::af2*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].back.left = in[i].left / 2;
					ret[i].back.right = in[i].right / 2;
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].back.left = in[i].left / 2;
					ret[i].back.right = in[i].right / 2;
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].side.left = in[i].left / 2;
					ret[i].side.right = in[i].right / 2;
					ret[i].back.left = in[i].left / 3;
					ret[i].back.right = in[i].right / 3;
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].side.left = in[i].left / 2;
					ret[i].side.right = in[i].right / 2;
					ret[i].back.left = in[i].left / 3;
					ret[i].back.right = in[i].right / 3;
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
				}
			}
		} else if (inType == AudioFrame::Type::eT_af21) { //2.1 channels to...
			AudioFrame::af21 *in = (AudioFrame::af21*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].right + in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].right + in[i].left, -1.0f, 1.0f);
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].right + in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].back.left = in[i].left / 2;
					ret[i].back.right = in[i].right / 2;
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp((in[i].left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp((in[i].right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = AZ::GetClamp(in[i].left + in[i].right, -1.0f, 1.0f);
					ret[i].side.left = in[i].left / 2;
					ret[i].side.right = in[i].right / 2;
					ret[i].back.left = in[i].left / 3;
					ret[i].back.right = in[i].right / 3;
					ret[i].sub = in[i].sub;
				}
			}
		} else if (inType == AudioFrame::Type::eT_af3) { //3 channels to...
			AudioFrame::af3 *in = (AudioFrame::af3*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].center = in[i].center;
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].back.left = in[i].left / 2;
					ret[i].back.right = in[i].right / 2;
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].back.left = in[i].left / 2;
					ret[i].back.right = in[i].right / 2;
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].side.left = in[i].left / 2;
					ret[i].side.right = in[i].right / 2;
					ret[i].back.left = in[i].left / 3;
					ret[i].back.right = in[i].right / 3;
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].side.left = in[i].left / 2;
					ret[i].side.right = in[i].right / 2;
					ret[i].back.left = in[i].left / 3;
					ret[i].back.right = in[i].right / 3;
					ret[i].sub = AZ::GetClamp(in[i].left + in[i].right + in[i].center, -1.0f, 1.0f);
				}
			}
		} else if (inType == AudioFrame::Type::eT_af31) { //3.1 channels to...
			AudioFrame::af31 *in = (AudioFrame::af31*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].left + in[i].right + in[i].center + in[i].sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].center / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].center / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = in[i].left;
					ret[i].right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].back.left = in[i].left / 2;
					ret[i].back.right = in[i].right / 2;
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.left = AZ::GetClamp((in[i].left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp((in[i].right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp((in[i].left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp((in[i].right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].left;
					ret[i].front.right = in[i].right;
					ret[i].center = in[i].center;
					ret[i].side.left = in[i].left / 2;
					ret[i].side.right = in[i].right / 2;
					ret[i].back.left = in[i].left / 3;
					ret[i].back.right = in[i].right / 3;
					ret[i].sub = in[i].sub;
				}
			}
		} else if (inType == AudioFrame::Type::eT_af5) { //5 channels to...
			AudioFrame::af5 *in = (AudioFrame::af5*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + (in[i].back.left / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].center = in[i].center;
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + (in[i].back.left / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].side.left = AZ::GetClamp((in[i].front.left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp((in[i].front.right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].side.left = AZ::GetClamp((in[i].front.left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp((in[i].front.right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				}
			}
		} else if (inType == AudioFrame::Type::eT_af51) { //5.1 channels to...
			AudioFrame::af51 *in = (AudioFrame::af51*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].back.left + in[i].back.right + in[i].sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f) / 2;
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.left = AZ::GetClamp((in[i].front.left / 2) + (in[i].back.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp((in[i].front.right / 2) + (in[i].back.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].side.left = AZ::GetClamp((in[i].front.left / 2) + (in[i].back.left / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp((in[i].front.right / 2) + (in[i].back.right / 2), -1.0f, 1.0f);
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
					ret[i].sub = in[i].sub;
				}
			}
		} else if (inType == AudioFrame::Type::eT_af7) { //7 channels to...
			AudioFrame::af7 *in = (AudioFrame::af7*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
					ret[i].sub = (in[i].front.left + in[i].front.right + in[i].center + (in[i].side.left / 2) + (in[i].side.right / 2) + (in[i].back.left / 3) + (in[i].back.right / 3), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
					ret[i].center = in[i].center;
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + (in[i].side.left / 2) + (in[i].side.right / 2) + (in[i].back.left / 3) + (in[i].back.right / 3), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.right / 2), -1.0f, 1.0f);
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + (in[i].side.left / 2) + (in[i].side.right / 2) + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].side.left = in[i].side.left;
					ret[i].side.right = in[i].side.right;
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].side.left = in[i].side.left;
					ret[i].side.right = in[i].side.right;
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
					ret[i].sub = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right, -1.0f, 1.0f);
				}
			}
		} else if (inType == AudioFrame::Type::eT_af71) { //7.1 channels to...
			AudioFrame::af71 *in = (AudioFrame::af71*)(from);
			if (outType == AudioFrame::Type::eT_af1) { //Mono
				AudioFrame::af1* ret = (AudioFrame::af1*)(to);
				for (int i = 0; i < len; i++)
					ret[i].mono = AZ::GetClamp(in[i].front.left + in[i].front.right + in[i].center + in[i].side.left + in[i].side.right + in[i].back.left + in[i].back.right + in[i].sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::Type::eT_af2) { //2 channels
				AudioFrame::af2* ret = (AudioFrame::af2*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af21) { //2.1 channels
				AudioFrame::af21* ret = (AudioFrame::af21*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].center / 2) + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].center / 2) + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af3) { //3 channels
				AudioFrame::af3* ret = (AudioFrame::af3*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2) + (in[i].back.left / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2) + (in[i].back.right / 3) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af31) { //3.1 channels
				AudioFrame::af31* ret = (AudioFrame::af31*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2) + (in[i].back.left / 3), -1.0f, 1.0f);
					ret[i].right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2) + (in[i].back.right / 3), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af5) { //5 channels
				AudioFrame::af5* ret = (AudioFrame::af5*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.left / 2) + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af51) { //5.1 channels
				AudioFrame::af51* ret = (AudioFrame::af51*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].side.left / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].side.right / 2), -1.0f, 1.0f);
					ret[i].center = in[i].center;
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].side.left / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].side.right / 2), -1.0f, 1.0f);
					ret[i].sub = in[i].sub;
				}
			} else if (outType == AudioFrame::Type::eT_af7) { //7 channels
				AudioFrame::af7* ret = (AudioFrame::af7*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = AZ::GetClamp(in[i].front.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].front.right = AZ::GetClamp(in[i].front.right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].center = AZ::GetClamp(in[i].center + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.left = AZ::GetClamp(in[i].side.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].side.right = AZ::GetClamp(in[i].side.right + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.left = AZ::GetClamp(in[i].back.left + (in[i].sub / 2), -1.0f, 1.0f);
					ret[i].back.right = AZ::GetClamp(in[i].back.right + (in[i].sub / 2), -1.0f, 1.0f);
				}
			} else if (outType == AudioFrame::Type::eT_af71) { //7.1 channels
				AudioFrame::af71* ret = (AudioFrame::af71*)(to);
				for (int i = 0; i < len; i++) {
					ret[i].front.left = in[i].front.left;
					ret[i].front.right = in[i].front.right;
					ret[i].center = in[i].center;
					ret[i].side.left = in[i].side.left;
					ret[i].side.right = in[i].side.right;
					ret[i].back.left = in[i].back.left;
					ret[i].back.right = in[i].back.right;
					ret[i].sub = in[i].sub;
				}
			}
		}
	}

	int AlternativeAudioSystemComponent::GetNumberOfChannels(AudioFrame::Type type) {
		if (type == AudioFrame::Type::eT_af1) return 1;
		else if (type == AudioFrame::Type::eT_af2) return 2;
		else if (type == AudioFrame::Type::eT_af21) return 3;
		else if (type == AudioFrame::Type::eT_af3) return 3;
		else if (type == AudioFrame::Type::eT_af31) return 4;
		else if (type == AudioFrame::Type::eT_af5) return 5;
		else if (type == AudioFrame::Type::eT_af51) return 6;
		else if (type == AudioFrame::Type::eT_af7) return 7;
		else if (type == AudioFrame::Type::eT_af71) return 8;
		return 1;
	}

	AudioFrame::Type AlternativeAudioSystemComponent::GetAudioFormat(int numberOfChannels) {
		switch (numberOfChannels) {
		case 1:
			return AudioFrame::Type::eT_af1;
		case 2:
			return AudioFrame::Type::eT_af2;
		case 3:
			return AudioFrame::Type::eT_af21;
		case 4:
			return AudioFrame::Type::eT_af31;
		case 5:
			return AudioFrame::Type::eT_af5;
		case 6:
			return AudioFrame::Type::eT_af51;
		case 7:
			return AudioFrame::Type::eT_af7;
		case 8:
			return AudioFrame::Type::eT_af71;
		}

		return AudioFrame::Type::eT_af1;
	}
	////////////////////////////////////////////////////////////////////////
}
