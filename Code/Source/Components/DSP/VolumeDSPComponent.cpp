#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "VolumeDSPComponent.h"
#include <AlternativeAudio\Components\AudioSourceComponentBus.h>
#include <AlternativeAudio\AlternativeAudioBus.h>
#include <AlternativeAudio\DSP\VolumeDSPBus.h>

namespace AlternativeAudio {
	namespace DSP {
		VolumeDSPComponent::VolumeDSPComponent() {
			this->m_pDSP = nullptr;
			this->m_DSPSlot = -1;
			this->m_shared = false;
			this->m_tag = "";
			this->m_vol = 1.0f;
			this->m_slot = -1;
			this->m_customSlot = false;
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
					->Field("vol", &VolumeDSPComponent::m_vol)
					->Field("customSlot", &VolumeDSPComponent::m_customSlot)
					->Field("slot", &VolumeDSPComponent::m_slot);

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

					editInfo->ClassElement(AZ::Edit::ClassElements::Group, "Custom Slot")
						->Attribute(AZ::Edit::Attributes::AutoExpand, true)
						->DataElement(AZ::Edit::UIHandlers::CheckBox, &VolumeDSPComponent::m_customSlot, "Use Slot", " ")
							->Attribute(AZ::Edit::Attributes::ChangeNotify, &VolumeDSPComponent::resetCustomSlot)
							->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
						->DataElement(AZ::Edit::UIHandlers::SpinBox, &VolumeDSPComponent::m_slot, "Slot", "")
							->Attribute(AZ::Edit::Attributes::Visibility, &VolumeDSPComponent::CustomSlotVisibilty)
							->Attribute(AZ::Edit::Attributes::Min, -1);
				}
			}
		#ifdef ENABLE_COMPONENT_SCRIPT_BUS
			AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
			if (behaviorContext) {
			#define EBUS_METHOD(name) ->Event(#name, &Components::DSP::VolumeComponentBus::Events::##name##)
				behaviorContext->EBus<Components::DSP::VolumeComponentBus>("AAVolumeDSPComponentBus")
					->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
					EBUS_METHOD(SetVol)
					EBUS_METHOD(GetVol)
					;
			#undef EBUS_METHOD
			}
		#endif
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

			if (source) {
				if (this->m_DSPSlot != (unsigned long long)(-1)) { //if we have a dsp slot number
					source->AddEffect(this->m_pDSP, this->m_DSPSlot); //add it to that slot
				} else { //otherwise
					if (this->m_customSlot) { //if we are using a custom slot
						if (source->AddEffect(this->m_pDSP, this->m_slot)) //add it to that slot
							this->m_DSPSlot = this->m_slot;
						else {
							this->m_DSPSlot = -1;
							//error notify? (cannot use slot)
							AZ_Printf("[Alternative Audio]", "[Alternative Audio](VolumeDSPComponent){%l} Could not assign AA Volume Control to slot \"%l\".", this->GetEntityId(), this->m_slot);
						}
					} else { //otherwise
						this->m_DSPSlot = source->AddEffectFreeSlot(this->m_pDSP); //add it to a free slot
					}
				}
			} else {
				//error notify? (no source)
				AZ_Printf("[Alternative Audio]", "[Alternative Audio](VolumeDSPComponent){%l} No source attached to entity.", this->GetEntityId());
			}
		}
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// DSPEffectCustomSlotComponentBus interface implementation
		void VolumeDSPComponent::UseCustomSlot(bool x) { this->m_customSlot = x; }
		bool VolumeDSPComponent::IsUsingCustomSlot() { return this->m_customSlot; }
		void VolumeDSPComponent::SetSlot(unsigned long long slot) {
			if (!this->m_customSlot) return;
			this->m_slot = slot;
			this->reloadDSP();
		}
		unsigned long long VolumeDSPComponent::GetSlot() { return this->m_DSPSlot; }
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

			if (this->m_pDSP) {
				this->m_pDSP->AddRef();
			} else {
				//error notify (dsp not created)
				AZ_Printf("[Alternative Audio]", "[Alternative Audio](VolumeDSPComponent){%l} Could not load AA Volume Control.", this->GetEntityId());
			}

			this->SourceReloaded();

			this->SetVol(this->m_vol);

			VolumeComponentBus::Handler::BusConnect(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()));
			DSPEffectComponentNotificationBus::Handler::BusConnect(this->GetEntityId());
			DSPEffectCustomSlotComponentBus::Handler::BusConnect(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()));
		}

		void VolumeDSPComponent::Deactivate() {
			VolumeComponentBus::Handler::BusDisconnect();
			DSPEffectComponentNotificationBus::Handler::BusDisconnect();
			DSPEffectCustomSlotComponentBus::Handler::BusDisconnect();

			//release dsp effect
			if (this->m_pDSP) {
				this->m_pDSP->Release();
				this->m_pDSP = nullptr;
				this->m_DSPSlot = -1;
			}
		}
		////////////////////////////////////////////////////////////////////////

		void VolumeDSPComponent::resetCustomSlot() {
			if (!this->m_customSlot) this->m_slot = -1;
		}

		void VolumeDSPComponent::reloadDSP() {
			if (VolumeComponentBus::Handler::BusIsConnectedId(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()))) {
				IAudioSource * source;
				EBUS_EVENT_ID_RESULT(source, this->GetEntityId(), AudioSourceComponentBus, GetSource);

				if (source) {
					if (this->m_DSPSlot != (unsigned long long)(-1)) {
						source->RemoveEffect(this->m_DSPSlot); //remove the dsp effect from the source

						if (this->m_pDSP) {
							//add the new dsp effect to the same slot of the previous one
							if (this->m_customSlot) { //if we are using a custom slot
								if (source->AddEffect(this->m_pDSP, this->m_slot)) //add it to that slot
									this->m_DSPSlot = this->m_slot;
								else {
									this->m_DSPSlot = -1;
									//error notify? (cannot use slot)
									AZ_Printf("[Alternative Audio]", "[Alternative Audio](VolumeDSPComponent){%l} Could not assign AA Volume Control to slot \"%l\".", this->GetEntityId(), this->m_slot);
								}
							} else { //otherwise
								//source->AddEffect(this->m_pDSP, this->m_DSPSlot); //set the dsp to the same slot
								this->m_DSPSlot = source->AddEffectFreeSlot(this->m_pDSP); //add it to a free slot
							}
						}
					}
				} else {
					//error notify? (no source)
					AZ_Printf("[Alternative Audio]", "[Alternative Audio](VolumeDSPComponent){%l} No source attached to entity.", this->GetEntityId());
				}
			}
		}
	}
}