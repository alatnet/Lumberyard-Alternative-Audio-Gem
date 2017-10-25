#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "DSPEffectComponent.h"

#include <AlternativeAudio\Components\AudioSourceComponentBus.h>
#include <AlternativeAudio\AlternativeAudioBus.h>

using namespace AlternativeAudio::Components;

namespace AlternativeAudio {
	DSPEffectComponent::DSPEffectComponent() {
		this->m_pDSP = nullptr;
		this->m_DSPlib = "";
		this->m_DSPCrc = -1;
		this->m_DSPSlot = -1;
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
				//editInfo->DataElement(AZ::Edit::UIHandlers::Crc, &DSPEffectComponent::m_DSPCrc, "Effect Crc", "The DSP Crc.");

				editInfo->DataElement(AZ::Edit::UIHandlers::Button, &DSPEffectComponent::m_checkButton, "Check", "Check to see if the library is valid.")
					->Attribute(AZ::Edit::Attributes::ChangeNotify, &DSPEffectComponent::CheckCrc)
					->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree);
			}
		}

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
		}
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
	void DSPEffectComponent::SetDSPEffectCrc(AZ::Crc32 dsp) {
		this->m_DSPCrc = dsp;

		this->reloadDSP();
	}
	void DSPEffectComponent::SetDSPEffect(AZStd::string dsp) {
		this->m_DSPlib = dsp;
		this->m_DSPCrc = AZ::Crc32(dsp.c_str());

		this->reloadDSP();
	}
	AADSPEffect* DSPEffectComponent::GetDSPEffect() {
		return this->m_pDSP;
	}
	AZ::Crc32 DSPEffectComponent::GetDSPEffectCrc() {
		return this->m_DSPCrc;
	}
	AZStd::string DSPEffectComponent::GetDSPEffectName() {
		return this->m_DSPlib;
	}
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// DSPEffectComponentNotificationBus interface implementation
	void DSPEffectComponent::SourceReloaded() {
		if (!this->m_pDSP) return;
		IAudioSource * source;
		EBUS_EVENT_ID_RESULT(source, this->GetEntityId(), AudioSourceComponentBus, GetSource);
		this->m_DSPSlot = source->AddEffectFreeSlot(this->m_pDSP);
	}
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// AZ::Component interface implementation
	void DSPEffectComponent::Init() {
	}

	void DSPEffectComponent::Activate() {
		//load dsp effect
		EBUS_EVENT_RESULT(this->m_pDSP, AlternativeAudioDSPBus, NewDSPEffect, this->m_DSPCrc, nullptr);
		this->m_pDSP->AddRef();

		//get audio source
		//add dsp effect to audio source
		this->SourceReloaded();

		DSPEffectComponentBus::Handler::BusConnect(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()));
		DSPEffectComponentNotificationBus::Handler::BusConnect(this->GetEntityId());
	}

	void DSPEffectComponent::Deactivate() {
		DSPEffectComponentBus::Handler::BusDisconnect();
		DSPEffectComponentNotificationBus::Handler::BusDisconnect();

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
		//else { //check crc
		//	for (AZStd::pair<AZStd::string, AZ::Crc32> pairs : crclist) {
		//		if (pairs.second == this->m_DSPCrc) {
		//			this->m_DSPlib = pairs.second;
		//			return;
		//		}
		//	}
		//}
		this->m_DSPlib = "Invalid";
		this->m_DSPCrc = -1;
	}

	void DSPEffectComponent::reloadDSP() {
		//reload dsp effect on source
		if (DSPEffectComponentBus::Handler::BusIsConnectedId(AZ::EntityComponentIdPair(this->GetEntityId(), this->GetId()))) {
			IAudioSource * source;
			EBUS_EVENT_ID_RESULT(source, this->GetEntityId(), AudioSourceComponentBus, GetSource);

			if (this->m_DSPSlot != (unsigned long long)(-1)) {
				source->RemoveEffect(this->m_DSPSlot);
				source->AddEffect(this->m_pDSP, this->m_DSPSlot);
			}
		}
	}
}