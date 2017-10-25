
#include "StdAfx.h"
#include <platform_impl.h>

#include "AlternativeAudioSystemComponent.h"

#include "Components\AudioSourceComponent.h"
#include "Components\DSPEffectComponent.h"

#include <IGem.h>

namespace AlternativeAudio
{
    class AlternativeAudioModule
        : public CryHooksModule
    {
    public:
        AZ_RTTI(AlternativeAudioModule, "{E02DF2A2-E5F6-4259-B529-87E421E0C07D}", CryHooksModule);

        AlternativeAudioModule()
            : CryHooksModule()
        {
            // Push results of [MyComponent]::CreateDescriptor() into m_descriptors here.
            m_descriptors.insert(m_descriptors.end(), {
                AlternativeAudioSystemComponent::CreateDescriptor(),
				AudioSourceComponent::CreateDescriptor(),
				DSPEffectComponent::CreateDescriptor(),
            });
        }

        /**
         * Add required SystemComponents to the SystemEntity.
         */
        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<AlternativeAudioSystemComponent>(),
            };
        }
    };
}

// DO NOT MODIFY THIS LINE UNLESS YOU RENAME THE GEM
// The first parameter should be GemName_GemIdLower
// The second should be the fully qualified name of the class above
AZ_DECLARE_MODULE_CLASS(AlternativeAudio_b4745a69bade49a3aacc01e4475bdee7, AlternativeAudio::AlternativeAudioModule)
