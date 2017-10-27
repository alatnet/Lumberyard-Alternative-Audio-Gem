#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "VolumeDSPComponent.h"
#include <AlternativeAudio\Components\AudioSourceComponentBus.h>
#include <AlternativeAudio\AlternativeAudioBus.h>
#include <AlternativeAudio\DSP\VolumeDSPBus.h>

using namespace AlternativeAudio::Components;
using namespace AlternativeAudio::Components::DSP;

namespace AlternativeAudio {
	namespace DSP {
		VolumeDSPComponent::VolumeDSPComponent() {
			this->m_pDSP = nullptr;
			this->m_DSPSlot = -1;
			this->m_shared = false;
			this->m_tag = "";
			this->m_vol = 1.0f;
		}

		VolumeDSPComponent::~VolumeDSPComponent() {
		}

		void VolumeDSPComponent::Reflect(AZ::ReflectContext* context) {
			if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
				//reflection
				serialize->Class<VolumeDSPComponent, AZ::Component>()
					->Version(1)
					->Field("shared", &VolumeDSPComponent::m_shared)
					->Field("tag", &VolumeDSPComponent::m_tag)
					->Field("vol", &VolumeDSPComponent::m_vol);

				//edit context
				if (AZ::EditContext* ec = serialize->GetEditContext()) {
					auto editInfo = ec->Class<VolumeDSPComponent>("AA Volume Control", "")
						->ClassElement(AZ::Edit::ClassElements::EditorData, "")
						->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/AudioRtpc.png")
						->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/AudioRtpc.png")
						->Attribute(AZ::Edit::Attributes::Category, "Alternative Audio - DSP")
						->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)
						;

					editInfo->DataElement(AZ::Edit::UIHandlers::Slider, &VolumeDSPComponent::m_vol, "Volume", "The volume to set the audio source.")
						->Attribute(AZ::Edit::Attributes::Max, 1.0f)
						->Attribute(AZ::Edit::Attributes::Min, 0.0f);

					editInfo->ClassElement(AZ::Edit::ClassElements::Group, "Shared")
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)
						->DataElement(AZ::Edit::UIHandlers::CheckBox, &VolumeDSPComponent::m_shared, "Shared", "Use a shared DSP Effect.")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
						->DataElement(0, &VolumeDSPComponent::m_tag, "Tag", "The tag of the shared DSP Effect.\nWill create the DSP Effect if there is no DSP Effect associated with the tag.")
						->Attribute(AZ::Edit::Attributes::Visibility, &VolumeDSPComponent::SharedVisibility);
				}
			}

			/*AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
			if (behaviorContext) {
			#define EBUS_METHOD(name) ->Event(#name, &Components::DSP::VolumeComponentBus::Events::##name##)
				behaviorContext->EBus<Components::DSP::VolumeComponentBus>("AAVolumeDSPComponentBus")
					->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
					EBUS_METHOD(SetVol)
					EBUS_METHOD(GetVol)
					;
			#undef EBUS_METHOD
			}*/
		}

		void VolumeDSPComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
			provided.push_back(AZ_CRC("AlternativeAudio_VolumeComponentService", 0x546c50ed));
		}

		void VolumeDSPComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
			(void)incompatible;
		}

		void VolumeDSPComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required) {
			required.push_back(AZ_CRC("AlternativeAudio_SourceComponentService", 0x548c783e));
		}

		void VolumeDSPComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent) {
			dependent.push_back(AZ_CRC("AlternativeAudio_SourceComponentService", 0x548c783e));
		}

		////////////////////////////////////////////////////////////////////////
		// AudioSourceComponentBus interface implementation
		void VolumeDSPComponent::SetVol(float vol) {
			this->m_vol = vol;
			EBUS_EVENT_ID(this->m_pDSP->get(), VolumeDSPBus, SetVol, vol);
		}
		float VolumeDSPComponent::GetVol() {
			float ret = 0.0f;
			EBUS_EVENT_ID_RESULT(ret, this->m_pDSP->get(), VolumeDSPBus, GetVol);
			this->m_vol = ret;
			return ret;
		}
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// DSPEffectComponentNotificationBus interface implementation
		void VolumeDSPComponent::SourceReloaded() {
			if (!this->m_pDSP) return;

			//get the source
			IAudioSource * source;
			EBUS_EVENT_ID_RESULT(source, this->GetEntityId(), AudioSourceComponentBus, GetSource);

			if (this->m_DSPSlot != (unsigned long long)(-1)) { //if we have a dsp slot number
				source->AddEffect(this->m_pDSP, this->m_DSPSlot); //add it to that slot
			} else { //otherwise
				this->m_DSPSlot = source->AddEffectFreeSlot(this->m_pDSP); //add it to a free slot
			}
		}
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void VolumeDSPComponent::Init() {
		}

		void VolumeDSPComponent::Activate() {
			//load dsp effect
			if (this->m_shared) {
				EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, GetSharedDSPEffect, this->m_tag, AZ_CRC("AAVolumeControl", 0x722dd2a9), nullptr);
			} else {
				EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, NewDSPEffect, AZ_CRC("AAVolumeControl", 0x722dd2a9), nullptr);
			}
			this->m_pDSP->AddRef();

			this->SourceReloaded();

			this->SetVol(this->m_vol);

			VolumeComponentBus::Handler::BusConnect(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()));
			DSPEffectComponentNotificationBus::Handler::BusConnect(this->GetEntityId());
		}

		void VolumeDSPComponent::Deactivate() {
			VolumeComponentBus::Handler::BusDisconnect();
			DSPEffectComponentNotificationBus::Handler::BusDisconnect();

			//release dsp effect
			if (this->m_pDSP) {
				this->m_pDSP->Release();
				this->m_pDSP = nullptr;
				this->m_DSPSlot = -1;
			}
		}
		////////////////////////////////////////////////////////////////////////
	}
}