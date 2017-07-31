
#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

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

			if (AZ::EditContext* ec = serialize->GetEditContext()) {
				ec->Class<AlternativeAudioSystemComponent>("AlternativeAudio", "Provides an alternative audio system for usage in lumberyard.")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					// ->Attribute(AZ::Edit::Attributes::Category, "") Set a category
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("System"))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					;
			}
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

	void AlternativeAudioSystemComponent::ConvertAudioFrame(AudioFrame::Frame* from, AudioFrame::Frame* to) {
		if (from->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono to...
			AudioFrame::af1 * in = azdynamic_cast<AudioFrame::af1*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = in->mono;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
				ret->sub = in->mono;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
				ret->center = in->mono;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->mono;
				ret->right = in->mono;
				ret->center = in->mono;
				ret->sub = in->mono;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->back.left = in->mono / 2;
				ret->back.right = in->mono / 2;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->back.left = in->mono / 2;
				ret->back.right = in->mono / 2;
				ret->sub = in->mono;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->side.left = in->mono / 2;
				ret->side.right = in->mono / 2;
				ret->back.left = in->mono / 3;
				ret->back.right = in->mono / 3;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->mono;
				ret->front.right = in->mono;
				ret->center = in->mono;
				ret->side.left = in->mono / 2;
				ret->side.right = in->mono / 2;
				ret->back.left = in->mono / 3;
				ret->back.right = in->mono / 3;
				ret->sub = in->mono;
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels to...
			AudioFrame::af2 * in = azdynamic_cast<AudioFrame::af2*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = in->left;
				ret->right = in->right;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels to...
			AudioFrame::af21 * in = azdynamic_cast<AudioFrame::af21*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right + in->sub, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->right + in->left + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = AZ::GetClamp(in->right + in->left, -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->right + in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->left + in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 3) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = AZ::GetClamp(in->left + in->right, -1.0f, 1.0f);
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = in->sub;
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels to...
			AudioFrame::af3 * in = azdynamic_cast<AudioFrame::af3*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2), -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = in->center;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = in->center;
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = AZ::GetClamp(in->left + in->right + in->center, -1.0f, 1.0f);
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels to...
			AudioFrame::af31 * in = azdynamic_cast<AudioFrame::af31*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->left + in->right + in->center + in->sub, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->left + (in->center / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->center / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = in->left;
				ret->right = in->right;
				ret->center = in->center;
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->back.left = in->left / 2;
				ret->back.right = in->right / 2;
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp((in->left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp((in->left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp((in->right / 3) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->left;
				ret->front.right = in->right;
				ret->center = in->center;
				ret->side.left = in->left / 2;
				ret->side.right = in->right / 2;
				ret->back.left = in->left / 3;
				ret->back.right = in->right / 3;
				ret->sub = in->sub;
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels to...
			AudioFrame::af5 * in = azdynamic_cast<AudioFrame::af5*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->back.left / 2) + (in->back.right / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->back.left / 2) + (in->back.right / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right, -1.0f, 1.0f);
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels to...
			AudioFrame::af51 * in = azdynamic_cast<AudioFrame::af51*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->back.left + in->back.right + in->sub, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f) / 2;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->back.left / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->back.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = AZ::GetClamp((in->front.left / 2) + (in->back.left / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp((in->front.right / 2) + (in->back.right / 2), -1.0f, 1.0f);
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = in->sub;
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels to...
			AudioFrame::af7 * in = azdynamic_cast<AudioFrame::af7*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->side.left + in->side.right + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->sub = (in->front.left + in->front.right + in->center + (in->side.left / 2) + (in->side.right / 2) + (in->back.left / 3) + (in->back.right / 3), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->center = in->center;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->side.left / 2) + (in->side.right / 2) + (in->back.left / 3) + (in->back.right / 3), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.left / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + (in->side.left / 2) + (in->side.right / 2) + in->back.left + in->back.right, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = in->side.left;
				ret->side.right = in->side.right;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
				ret->front.left = in->front.left;
				ret->front.right = in->front.right;
				ret->center = in->center;
				ret->side.left = in->side.left;
				ret->side.right = in->side.right;
				ret->back.left = in->back.left;
				ret->back.right = in->back.right;
				ret->sub = AZ::GetClamp(in->front.left + in->front.right + in->center + in->side.left + in->side.right + in->back.left + in->back.right, -1.0f, 1.0f);
			}
		} else if (from->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels to...
			AudioFrame::af71 * in = azdynamic_cast<AudioFrame::af71*>(from);
			if (to->RTTI_IsTypeOf(AudioFrame::af1::RTTI_Type())) { //Mono
				AudioFrame::af1 * ret = azdynamic_cast<AudioFrame::af1*>(to);
				ret->mono = AZ::GetClamp(in->front.left + in->front.right + in->center + in->side.left + in->side.right + in->back.left + in->back.right + in->sub, -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af2::RTTI_Type())) { //2 channels
				AudioFrame::af2 * ret = azdynamic_cast<AudioFrame::af2*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af21::RTTI_Type())) { //2.1 channels
				AudioFrame::af21 * ret = azdynamic_cast<AudioFrame::af21*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->center / 2) + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->center / 2) + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af3::RTTI_Type())) { //3 channels
				AudioFrame::af3 * ret = azdynamic_cast<AudioFrame::af3*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3) + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af31::RTTI_Type())) { //3.1 channels
				AudioFrame::af31 * ret = azdynamic_cast<AudioFrame::af31*>(to);
				ret->left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->back.left / 3), -1.0f, 1.0f);
				ret->right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->back.right / 3), -1.0f, 1.0f);
				ret->center = in->center;
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af5::RTTI_Type())) { //5 channels
				AudioFrame::af5 * ret = azdynamic_cast<AudioFrame::af5*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2) + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.left / 2) + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af51::RTTI_Type())) { //5.1 channels
				AudioFrame::af51 * ret = azdynamic_cast<AudioFrame::af51*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->center = in->center;
				ret->back.left = AZ::GetClamp(in->back.left + (in->side.left / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->side.right / 2), -1.0f, 1.0f);
				ret->sub = in->sub;
			} else if (to->RTTI_IsTypeOf(AudioFrame::af7::RTTI_Type())) { //7 channels
				AudioFrame::af7 * ret = azdynamic_cast<AudioFrame::af7*>(to);
				ret->front.left = AZ::GetClamp(in->front.left + (in->sub / 2), -1.0f, 1.0f);
				ret->front.right = AZ::GetClamp(in->front.right + (in->sub / 2), -1.0f, 1.0f);
				ret->center = AZ::GetClamp(in->center + (in->sub / 2), -1.0f, 1.0f);
				ret->side.left = AZ::GetClamp(in->side.left + (in->sub / 2), -1.0f, 1.0f);
				ret->side.right = AZ::GetClamp(in->side.right + (in->sub / 2), -1.0f, 1.0f);
				ret->back.left = AZ::GetClamp(in->back.left + (in->sub / 2), -1.0f, 1.0f);
				ret->back.right = AZ::GetClamp(in->back.right + (in->sub / 2), -1.0f, 1.0f);
			} else if (to->RTTI_IsTypeOf(AudioFrame::af71::RTTI_Type())) { //7.1 channels
				AudioFrame::af71 * ret = azdynamic_cast<AudioFrame::af71*>(to);
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
