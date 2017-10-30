#include "StdAfx.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include "AudioSourceComponent.h"

#include <AlternativeAudio\AlternativeAudioBus.h>

#include <AlternativeAudio\Components\DSPEffectComponentBus.h>
#include <AlternativeAudio\AAAttributeTypes.h>

namespace AlternativeAudio {
	AudioSourceComponent::AudioSourceComponent() {
		this->m_pSrc = nullptr;
		this->m_pDevice = nullptr;
		this->m_srcID = -1;
		this->m_path = "";
		this->m_lib = "";
		this->m_libCrc = -1;
		this->m_loop = false;
	}

	AudioSourceComponent::~AudioSourceComponent() {
	}

	void AudioSourceComponent::Reflect(AZ::ReflectContext* context) {
		if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context)) {
			//reflection
			serialize->Class<AudioSourceComponent, AZ::Component>()
				->Version(1)
				->Field("lib", &AudioSourceComponent::m_lib)
				->Field("libCrc", &AudioSourceComponent::m_libCrc)
				->Field("path", &AudioSourceComponent::m_path)
				->Field("loop", &AudioSourceComponent::m_loop)
				->Field("voidCheckButton", &AudioSourceComponent::m_checkButton);

			//edit context
			if (AZ::EditContext* ec = serialize->GetEditContext()) {
				auto editInfo = ec->Class<AudioSourceComponent>("AA Audio Source", "");

				editInfo->ClassElement(AZ::Edit::ClassElements::EditorData, "")
					->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/AudioArea.png")
					->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Viewport/AudioArea.png")
					->Attribute(AZ::Edit::Attributes::Category, "Alternative Audio")
					->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					;

				editInfo->DataElement(0, &AudioSourceComponent::m_path, "Path", "Audio file path.");
				editInfo->DataElement(AZ::Edit::UIHandlers::CheckBox, &AudioSourceComponent::m_loop, "Loop", "");

				editInfo->ClassElement(AZ::Edit::ClassElements::Group, "Audio Library")
					->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(0, &AudioSourceComponent::m_lib, "Library", "Library name to use.")
					->DataElement(AZ::Edit::UIHandlers::Button, &AudioSourceComponent::m_checkButton, "Check Library", "Check to see if the library is valid.")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, &AudioSourceComponent::CheckCrc)
						->Attribute(AZ::Edit::Attributes::ChangeNotify, AZ::Edit::PropertyRefreshLevels::EntireTree);
			}
		}

		AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context);
		if (behaviorContext) {
			#define EBUS_METHOD(name) ->Event(#name, &Components::AudioSourceComponentBus::Events::##name##)
			behaviorContext->EBus<Components::AudioSourceComponentBus>("AASourceComponentBus")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				EBUS_METHOD(Play)
				EBUS_METHOD(PlaySFX)
				EBUS_METHOD(Pause)
				EBUS_METHOD(Resume)
				EBUS_METHOD(Stop)
				EBUS_METHOD(IsPlaying)
				EBUS_METHOD(SetTime)
				EBUS_METHOD(GetTime)
				EBUS_METHOD(SetPath)
				EBUS_METHOD(GetPath)
				EBUS_METHOD(SetLooping)
				EBUS_METHOD(IsLooping)
				EBUS_METHOD(SetLibraryCrc)
				EBUS_METHOD(GetLibraryCrc)
				EBUS_METHOD(SetLibrary)
				EBUS_METHOD(GetLibrary)
				EBUS_METHOD(GetSource)
				EBUS_METHOD(SetDevice)
				;
			#undef EBUS_METHOD
		}
	}

	void AudioSourceComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided) {
		provided.push_back(AZ_CRC("AlternativeAudio_SourceComponentService", 0x548c783e));
	}

	void AudioSourceComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible) {
		incompatible.push_back(AZ_CRC("AlternativeAudio_SourceComponentService", 0x548c783e));
	}

	void AudioSourceComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required) {
		(void)required;
	}

	void AudioSourceComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent) {
		(void)dependent;
	}

	////////////////////////////////////////////////////////////////////////
	// AudioSourceComponentBus interface implementation
	void AudioSourceComponent::Play() {
		if (!this->m_pSrc) return;
		if (this->m_pDevice) this->m_srcID = this->m_pDevice->PlaySource(this->m_pSrc);
		else EBUS_EVENT_RESULT(this->m_srcID, AlternativeAudioDeviceBus, PlaySource, this->m_pSrc);
	}
	void AudioSourceComponent::PlaySFX() {
		if (!this->m_pSrc) return;
		if (this->m_pDevice) this->m_pDevice->PlaySFXSource(this->m_pSrc);
		else EBUS_EVENT(AlternativeAudioDeviceBus, PlaySFXSource, this->m_pSrc);
	}
	void AudioSourceComponent::Pause() {
		if (this->m_srcID == (unsigned long long)(-1)) return;
		if (this->m_pDevice) this->m_pDevice->PauseSource(this->m_srcID);
		else EBUS_EVENT(AlternativeAudioDeviceBus, PauseSource, this->m_srcID);
	}
	void AudioSourceComponent::Resume() {
		if (this->m_srcID == (unsigned long long)(-1)) return;
		if (this->m_pDevice) this->m_pDevice->ResumeSource(this->m_srcID);
		else EBUS_EVENT(AlternativeAudioDeviceBus, ResumeSource, this->m_srcID);
	}
	void AudioSourceComponent::Stop() {
		if (this->m_srcID == (unsigned long long)(-1)) return;
		if (this->m_pDevice) this->m_pDevice->StopSource(this->m_srcID);
		else EBUS_EVENT(AlternativeAudioDeviceBus, StopSource, this->m_srcID);
		this->m_srcID = -1;
	}
	bool AudioSourceComponent::IsPlaying(){
		if (this->m_srcID == (unsigned long long)(-1)) return false;
		if (this->m_pDevice) return this->m_pDevice->IsPlaying(this->m_srcID);

		bool ret = false;
		EBUS_EVENT_RESULT(ret, AlternativeAudioDeviceBus, IsPlaying, this->m_srcID);
		return ret;
	}
	AudioSourceTime AudioSourceComponent::GetTime(){
		if (this->m_srcID == (unsigned long long)(-1)) return AudioSourceTime();
		if (this->m_pDevice) return this->m_pDevice->GetTime(this->m_srcID);

		AudioSourceTime ret;
		EBUS_EVENT_RESULT(ret, AlternativeAudioDeviceBus, GetTime, this->m_srcID);
		return ret;
	}
	void AudioSourceComponent::SetTime(double time) {
		if (this->m_srcID == (unsigned long long)(-1)) return;
		if (this->m_pDevice) this->m_pDevice->SetTime(this->m_srcID, time);
		else EBUS_EVENT(AlternativeAudioDeviceBus, SetTime, this->m_srcID, time);
	}

	void AudioSourceComponent::SetPath(AZStd::string path) {
		this->m_path = path;
		this->reloadSource();
	}
	AZStd::string AudioSourceComponent::GetPath() { return this->m_path; }
	
	void AudioSourceComponent::SetLooping(bool looping) {
		this->m_loop = looping;
		
		if (this->m_pSrc)
			this->m_pSrc->setAttr(Attributes::Source::Loop, looping);

		if (this->m_srcID != (unsigned long long)(-1)) {
			auto attr = AAAttributeHandler::CreateAttribute(looping);
			if (this->m_pDevice) this->m_pDevice->UpdateAttribute(this->m_srcID, Attributes::Source::Loop, attr);
			else EBUS_EVENT(AlternativeAudioDeviceBus, UpdateAttribute, this->m_srcID, Attributes::Source::Loop, attr);
		}
	}
	bool AudioSourceComponent::IsLooping() { return this->m_loop; }

	void AudioSourceComponent::SetLibraryCrc(AZ::Crc32 lib) {
		this->m_libCrc = lib;
		this->reloadSource();
	}
	void AudioSourceComponent::SetLibrary(AZStd::string lib) {
		this->m_lib = lib;
		this->m_libCrc = AZ::Crc32(lib.c_str());
		this->reloadSource();
	}
	AZ::Crc32 AudioSourceComponent::GetLibraryCrc() { return this->m_libCrc; }
	AZStd::string AudioSourceComponent::GetLibrary() { return this->m_lib; }

	IAudioSource* AudioSourceComponent::GetSource() { return this->m_pSrc; }

	void AudioSourceComponent::SetDevice(OAudioDevice* device) {
		if (this->IsPlaying()) this->Stop();
		this->m_pDevice = device;
	}
	////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////
	// AZ::Component interface implementation
	void AudioSourceComponent::Init() {
	}

	void AudioSourceComponent::Activate() {
		//load audio source
		//if (gEnv->pFileIO->Exists(this->m_path.c_str())) {
			EBUS_EVENT_RESULT(this->m_pSrc, AlternativeAudioSourceBus, NewAudioSource, this->m_libCrc, this->m_path, nullptr);
			if (this->m_pSrc) {
				this->m_pSrc->AddRef();
				this->m_pSrc->Attribute(Attributes::Source::Loop, this->m_loop);
			} else {
				//error notify (could not create source)
				AZ_Printf("[Alternative Audio]", "[Alternative Audio](AudioSourceComponent){%l} Could not load \"%s\".", this->GetEntityId(), this->m_path.c_str());
			}
		//} else {
		//	//error notify (path doesnt exist)
		//}

		this->BusConnect(this->GetEntityId());
	}

	void AudioSourceComponent::Deactivate() {
		this->BusDisconnect();

		if (this->IsPlaying()) this->Stop();

		//release audio source
		if (this->m_pSrc) {
			this->m_pSrc->Release();
			this->m_pSrc = nullptr;
		}
	}
	////////////////////////////////////////////////////////////////////////

	void AudioSourceComponent::CheckCrc() {
		AZStd::vector<AZStd::pair<AZStd::string, AZ::Crc32>> crclist;
		EBUS_EVENT_RESULT(crclist, AlternativeAudioSourceBus, GetAudioLibraryNames);

		if (!this->m_lib.empty()) { //check library
			for (AZStd::pair<AZStd::string, AZ::Crc32> pairs : crclist) {
				if (pairs.first.compare(this->m_lib) == 0) {
					this->m_libCrc = pairs.second;
					return;
				}
			}
		}
		this->m_lib = "Invalid";
		this->m_libCrc = -1;
	}

	void AudioSourceComponent::reloadSource() {
		//reload source if activated
		if (this->BusIsConnectedId(this->GetEntityId())) {
			if (this->IsPlaying()) this->Stop();

			if (this->m_pSrc) {
				this->m_pSrc->Release();
				this->m_pSrc = nullptr;
			}

			//if (gEnv->pFileIO->Exists(this->m_path.c_str())) {
				EBUS_EVENT_RESULT(this->m_pSrc, AlternativeAudioSourceBus, NewAudioSource, this->m_libCrc, this->m_path, nullptr);
				if (this->m_pSrc) {
					this->m_pSrc->AddRef();
					this->m_pSrc->Attribute(Attributes::Source::Loop, this->m_loop);
				}
				else {
					//error notify (could not create source)
					AZ_Printf("[Alternative Audio]", "[Alternative Audio](AudioSourceComponent){%l} Could not load \"%s\".", this->GetEntityId(), this->m_path.c_str());
				}
			//} else {
			//	//error notify (path doesnt exist)
			//}

			//notify dsp effects that there is a new source
			EBUS_EVENT_ID(this->GetEntityId(), DSPEffectComponentNotificationBus, SourceReloaded);
		}
	}
}