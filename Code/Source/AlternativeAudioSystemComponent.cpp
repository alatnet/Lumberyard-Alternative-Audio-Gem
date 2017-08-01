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
			SerializeEmpty(AudioFrame::Frame);
			SerializeEmpty(AudioFrame::af1);
			SerializeEmpty(AudioFrame::af2);
			SerializeEmpty(AudioFrame::af21);
			SerializeEmpty(AudioFrame::af3);
			SerializeEmpty(AudioFrame::af31);
			SerializeEmpty(AudioFrame::af5);
			SerializeEmpty(AudioFrame::af51);
			SerializeEmpty(AudioFrame::af7);
			SerializeEmpty(AudioFrame::af71);

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
			behaviorContext->Class<AudioFrame::Frame>("AudioFrame")
				->Method("af1", &AudioFrame::af1::RTTI_Type)
				->Method("af2", &AudioFrame::af2::RTTI_Type)
				->Method("af21", &AudioFrame::af21::RTTI_Type)
				->Method("af3", &AudioFrame::af3::RTTI_Type)
				->Method("af31", &AudioFrame::af31::RTTI_Type)
				->Method("af5", &AudioFrame::af5::RTTI_Type)
				->Method("af51", &AudioFrame::af51::RTTI_Type)
				->Method("af7", &AudioFrame::af7::RTTI_Type)
				->Method("af71", &AudioFrame::af71::RTTI_Type);

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

	void AlternativeAudioSystemComponent::ConvertAudioFrame(AudioFrame::Frame* from, AudioFrame::Frame* to, AZ::Uuid inType, AZ::Uuid outType) {
		if (inType == AudioFrame::af1::RTTI_Type()) { //Mono to...
			AudioFrame::af1 *in = azdynamic_cast<AudioFrame::af1*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
			AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = in->mono;
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
				ret->sub = in->mono;
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
				ret->center = in->mono;
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
				ret->center = in->mono;
				ret->sub = in->mono;
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->back.left = in->mono / 2;
				ret->back.right = in->mono / 2;
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->back.left = in->mono / 2;
				ret->back.right = in->mono / 2;
				ret->sub = in->mono;
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->side.left = in->mono / 2;
				ret->side.right = in->mono / 2;
				ret->back.left = in->mono / 3;
				ret->back.right = in->mono / 3;
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->side.left = in->mono / 2;
				ret->side.right = in->mono / 2;
				ret->back.left = in->mono / 3;
				ret->back.right = in->mono / 3;
				ret->sub = in->mono;
			}
		} else if (inType == AudioFrame::af2::RTTI_Type()) { //2 ret-> to...
			AudioFrame::af2 *in = azdynamic_cast<AudioFrame::af2*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = in->left;
				ret->right = in->right;
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			}
		} else if (inType == AudioFrame::af21::RTTI_Type()) { //2.1 ret-> to...
			AudioFrame::af21 *in = azdynamic_cast<AudioFrame::af21*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right + in->sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->right + in->left + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = AZ::GetClamp(in->right + in->left, -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->right + in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->left + in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 3) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = in->sub;
			}
		} else if (inType == AudioFrame::af3::RTTI_Type()) { //3 ret-> to...
			AudioFrame::af3 *in = azdynamic_cast<AudioFrame::af3*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2), -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = in->center;
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = in->center;
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			}
		} else if (inType == AudioFrame::af31::RTTI_Type()) { //3.1 ret-> to...
			AudioFrame::af31 *in = azdynamic_cast<AudioFrame::af31*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right + in->center + in->sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = in->center;
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 3) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = in->sub;
			}
		} else if (inType == AudioFrame::af5::RTTI_Type()) { //5 ret-> to...
			AudioFrame::af5 *in = azdynamic_cast<AudioFrame::af5*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->back.left / 2) + (in->back.right / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->back.left / 2) + (in->back.right / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right, -1.0f, 1.0f);
			}
		} else if (inType == AudioFrame::af51::RTTI_Type()) { //5.1 ret-> to...
			AudioFrame::af51 *in = azdynamic_cast<AudioFrame::af51*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right + in->sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f) / 2;
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = in->sub;
			}
		} else if (inType == AudioFrame::af7::RTTI_Type()) { //7 ret-> to...
			AudioFrame::af7 *in = azdynamic_cast<AudioFrame::af7*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->side.left + in->side.right + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->sub = (in->front.left + in->front.right + in->center + (in->side.left / 2) + (in->side.right / 2) + (in->back.left / 3) + (in->back.right / 3), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->center = in->center;
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->side.left / 2) + (in->side.right / 2) + (in->back.left / 3) + (in->back.right / 3), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.left / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->side.left / 2) + (in->side.right / 2) + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = in->side.left;
				ret->side.right = in->side.right;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = in->side.left;
				ret->side.right = in->side.right;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + in->side.left + in->side.right + in->back.left + in->back.right, -1.0f, 1.0f);
			}
		} else if (inType == AudioFrame::af71::RTTI_Type()) { //7.1 ret-> to...
			AudioFrame::af71 *in = azdynamic_cast<AudioFrame::af71*>(from);
			if (outType == AudioFrame::af1::RTTI_Type()) { //Mono
				AudioFrame::af1* ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->side.left + in->side.right + in->back.left + in->back.right + in->sub, -1.0f, 1.0f);
			} else if (outType == AudioFrame::af2::RTTI_Type()) { //2 ret->
				AudioFrame::af2* ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af21::RTTI_Type()) { //2.1 ret->
				AudioFrame::af21* ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af3::RTTI_Type()) { //3 ret->
				AudioFrame::af3* ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af31::RTTI_Type()) { //3.1 ret->
				AudioFrame::af31* ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af5::RTTI_Type()) { //5 ret->
				AudioFrame::af5* ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.left / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af51::RTTI_Type()) { //5.1 ret->
				AudioFrame::af51* ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (outType == AudioFrame::af7::RTTI_Type()) { //7 ret->
				AudioFrame::af7* ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp(in->side.left + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp(in->side.right + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (outType == AudioFrame::af71::RTTI_Type()) { //7.1 ret->
				AudioFrame::af71* ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = in->side.left;
				ret->side.right = in->side.right;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = in->sub;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////
}
