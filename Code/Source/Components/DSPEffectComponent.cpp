#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "DSPEffectComponent.h"

#include <AlternativeAudio\Components\AudioSourceComponentBus.h>
#include <AlternativeAudio\AlternativeAudioBus.h>

namespace AlternativeAudio {
	DSPEffectComponent::DSPEffectComponent() {
		this->m_pDSP = nullptr;
		this->m_DSPlib = "";
		this->m_DSPCrc = -1;
		this->m_DSPSlot = -1;
		this->m_tag = "";
		this->m_shared = false;
		this->m_pDSPUserdata = nullptr;
		this->m_customSlot = false;
		this->m_slot = -1;
	}

	DSPEffectComponent::~DSPEffectComponent() {
	}

	void DSPEffectComponent::Reflect(AZ::ReflectContext* context) {
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
			//reflection
			serialize->Class<DSPEffectComponent, AZ::Component>()
				->Version(1)
				->Field("lib", &DSPEffectComponent::m_DSPlib)
				->Field("crc", &DSPEffectComponent::m_DSPCrc)
				->Field("shared", &DSPEffectComponent::m_shared)
				->Field("tag", &DSPEffectComponent::m_tag)
				->Field("customSlot", &DSPEffectComponent::m_customSlot)
				->Field("slot", &DSPEffectComponent::m_slot)
				->Field("voidCheckButton", &DSPEffectComponent::m_checkButton);

			//edit context
			if (AZ::EditContext* ec = serialize->GetEditContext()) {
				auto editInfo = ec->Class<DSPEffectComponent>("AA DSP Effect", "")
					->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/AudioRtpc.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/AudioRtpc.png")
					->Attribute(AZ::Edit::Attributes::Category, "Alternative Audio")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					;

				editInfo->DataElement(0, &DSPEffectComponent::m_DSPlib, "Effect", "The DSP Effect to use.");

				editInfo->DataElement(AZ::Edit::UIHandlers::Button, &DSPEffectComponent::m_checkButton, "Check Library", "Check to see if the library is valid.")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, &DSPEffectComponent::CheckCrc)
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					;

				editInfo->ClassElement(AZ::Edit::ClassElements::Group, "Shared")
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &DSPEffectComponent::m_shared, "Shared", "Use a shared DSP Effect.")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->DataElement(0, &DSPEffectComponent::m_tag, "Tag", "The tag of the shared DSP Effect.\nWill create the DSP Effect if there is no DSP Effect associated with the tag.")
						->Attribute(AZ::Edit::Attributes::Visibility, &DSPEffectComponent::SharedVisibility)
					;

				editInfo->ClassElement(AZ::Edit::ClassElements::Group, "Custom Slot")
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(AZ::Edit::UIHandlers::CheckBox, &DSPEffectComponent::m_customSlot, "Use Slot", " ")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, &DSPEffectComponent::resetCustomSlot)
						->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree)
					->DataElement(AZ::Edit::UIHandlers::SpinBox, &DSPEffectComponent::m_slot, "Slot", "")
						->Attribute(AZ::Edit::Attributes::Visibility, &DSPEffectComponent::CustomSlotVisibilty)
						->Attribute(AZ::Edit::Attributes::Min, -1);
			}
		}

	#ifdef ENABLE_COMPONENT_SCRIPT_BUS
		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			#define EBUS_METHOD(name) ->Event(#name, &Components::DSPEffectComponentBus::Events::##name##)
			behaviorContext->EBus<Components::DSPEffectComponentBus>("AADSPEffectComponentBus")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				EBUS_METHOD(SetDSPEffectCrc)
				EBUS_METHOD(SetDSPEffect)
				EBUS_METHOD(GetDSPEffect)
				EBUS_METHOD(GetDSPEffectCrc)
				EBUS_METHOD(GetDSPEffectName)
				;
			#undef EBUS_METHOD

		#define EBUS_METHOD(name) ->Event(#name, &Components::DSPEffectCustomSlotComponentBus::Events::##name##)
			behaviorContext->EBus<Components::DSPEffectCustomSlotComponentBus>("AADSPEffectCustomSlotComponentBus")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				EBUS_METHOD(UseCustomSlot)
				EBUS_METHOD(IsUsingCustomSlot)
				EBUS_METHOD(SetSlot)
				EBUS_METHOD(GetSlot)
				;
		#undef EBUS_METHOD
		}
	#endif
	}

	void DSPEffectComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC("AlternativeAudio_DSPComponentService", 0x19520fed));
	}

	void DSPEffectComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
		(void)incompatible;
	}

	void DSPEffectComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required) {
		required.push_back(AZ_CRC("AlternativeAudio_SourceComponentService", 0x548c783e));
	}

	void DSPEffectComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent) {
		dependent.push_back(AZ_CRC("AlternativeAudio_SourceComponentService", 0x548c783e));
	}

	////////////////////////////////////////////////////////////////////////
	// DSPEffectComponentBus interface implementation
	void DSPEffectComponent::SetDSPEffectCrc(AZ::Crc32 dsp, void* userdata) {
		this->m_DSPCrc = dsp;
		this->m_pDSPUserdata = userdata;
		this->reloadDSP();
	}
	void DSPEffectComponent::SetDSPEffect(AZStd::string dsp, void* userdata) {
		this->m_DSPlib = dsp;
		this->m_DSPCrc = AZ::Crc32(dsp.c_str());
		this->m_pDSPUserdata = userdata;
		this->reloadDSP();
	}
	AADSPEffect* DSPEffectComponent::GetDSPEffect() { return this->m_pDSP; }
	AZ::Crc32 DSPEffectComponent::GetDSPEffectCrc() { return this->m_DSPCrc; }
	AZStd::string DSPEffectComponent::GetDSPEffectName() { return this->m_DSPlib; }

	void DSPEffectComponent::SetShared(bool shared) {
		this->m_shared = shared;
		this->reloadDSP();
	}
	void DSPEffectComponent::SetSharedTag(AZStd::string tag) {
		this->m_tag = tag;
		this->reloadDSP();
	}
	bool DSPEffectComponent::GetShared() { return this->m_shared; }
	AZStd::string DSPEffectComponent::GetSharedTag() { return this->m_tag; }
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// DSPEffectCustomSlotComponentBus interface implementation
	void DSPEffectComponent::UseCustomSlot(bool x) { this->m_customSlot = x; }
	bool DSPEffectComponent::IsUsingCustomSlot() { return this->m_customSlot; }
	void DSPEffectComponent::SetSlot(unsigned long long slot) {
		if (!this->m_customSlot) return;
		this->m_slot = slot;
		this->reloadDSP();
	}
	unsigned long long DSPEffectComponent::GetSlot() { return this->m_DSPSlot; }
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// DSPEffectComponentNotificationBus interface implementation
	void DSPEffectComponent::SourceReloaded() {
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
						AZ_Printf("[Alternative Audio]", "[Alternative Audio](DSPEffectComponent){%l} Could not assign dsp \"%l\" to slot \"%l\".", this->GetEntityId(), this->m_DSPCrc, this->m_slot);
					}
				} else { //otherwise
					this->m_DSPSlot = source->AddEffectFreeSlot(this->m_pDSP); //add it to a free slot
				}
			}
		} else {
			//error notify? (no source)
			AZ_Printf("[Alternative Audio]", "[Alternative Audio](DSPEffectComponent){%l} No source attached to entity.", this->GetEntityId());
		}
	}
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// AZ::Component interface implementation
	void DSPEffectComponent::Init() {
	}

	void DSPEffectComponent::Activate() {
		//load dsp effect
		if (this->m_shared) {
			EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, GetSharedDSPEffect, this->m_tag, this->m_DSPCrc, this->m_pDSPUserdata);
		} else {
			EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, NewDSPEffect, this->m_DSPCrc, this->m_pDSPUserdata);
		}
		if (this->m_pDSP) this->m_pDSP->AddRef();
		else {
			//error notify
			AZ_Printf("[Alternative Audio]", "[Alternative Audio](DSPEffectComponent){%l} Could not load dsp \"%l\".", this->GetEntityId(), this->m_DSPCrc);
		}

		this->SourceReloaded();

		DSPEffectComponentBus::Handler::BusConnect(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()));
		DSPEffectComponentNotificationBus::Handler::BusConnect(this->GetEntityId());
		DSPEffectCustomSlotComponentBus::Handler::BusConnect(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()));
	}

	void DSPEffectComponent::Deactivate() {
		DSPEffectComponentBus::Handler::BusDisconnect();
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

	void DSPEffectComponent::CheckCrc() {
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> crclist;
		EBUS_EVENT_RESULT(crclist, AlternativeAudioDSPBus, GetDSPEffectNames);

		if (!this->m_DSPlib.empty()) { //check library
			for (AZStd::pair<AZStd::string, AZ::Crc32> pairs : crclist) {
				if (pairs.first.compare(this->m_DSPlib) == 0) {
					this->m_DSPCrc = pairs.second;
					return;
				}
			}
		}
		this->m_DSPlib = "Invalid";
		this->m_DSPCrc = -1;
	}

	void DSPEffectComponent::reloadDSP() {
		//if the dsp effect is already active
		if (DSPEffectComponentBus::Handler::BusIsConnectedId(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()))) {
			//get the audio source
			IAudioSource * source;
			EBUS_EVENT_ID_RESULT(source, this->GetEntityId(), AudioSourceComponentBus, GetSource);

			if (source) {
				//if we have a dsp slot number
				if (this->m_DSPSlot != (unsigned long long)(-1)) {
					source->RemoveEffect(this->m_DSPSlot); //remove the dsp effect from the source

					//release the dsp effect
					if (this->m_pDSP) {
						this->m_pDSP->Release();
						this->m_pDSP = nullptr;
					}

					//create a new dsp effect
					if (this->m_shared) {
						EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, GetSharedDSPEffect, this->m_tag, this->m_DSPCrc, this->m_pDSPUserdata);
					} else {
						EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, NewDSPEffect, this->m_DSPCrc, this->m_pDSPUserdata);
					}

					if (this->m_pDSP) {
						this->m_pDSP->AddRef();

						//add the new dsp effect to the same slot of the previous one
						if (this->m_customSlot) { //if we are using a custom slot
							if (source->AddEffect(this->m_pDSP, this->m_slot)) //add it to that slot
								this->m_DSPSlot = this->m_slot;
							else {
								this->m_DSPSlot = -1;
								//error notify? (cannot use slot)
								AZ_Printf("[Alternative Audio]", "[Alternative Audio](DSPEffectComponent){%l} Could not assign dsp \"%l\" to slot \"%l\".", this->GetEntityId(), this->m_DSPCrc, this->m_slot);
							}
						} else { //otherwise
							this->m_DSPSlot = source->AddEffectFreeSlot(this->m_pDSP);
						}
					} else {
						//error notify (dsp not created)
						AZ_Printf("[Alternative Audio]", "[Alternative Audio](DSPEffectComponent){%l} Could not load dsp \"%l\".", this->GetEntityId(), this->m_DSPCrc);
					}
				}
			} else {
				//error notify? (no source)
				AZ_Printf("[Alternative Audio]", "[Alternative Audio](DSPEffectComponent){%l} No source attached to entity.", this->GetEntityId());
			}
		}
	}

	void DSPEffectComponent::resetCustomSlot() {
		if (!this->m_customSlot) this->m_slot = -1;
	}
}