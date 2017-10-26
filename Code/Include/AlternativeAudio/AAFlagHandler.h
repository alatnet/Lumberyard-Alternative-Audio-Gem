#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore\RTTI\RTTI.h>

namespace AlternativeAudio {
	class AAFlagHandler {
	public:
		AZ_RTTI(AAFlagHandler, "{CCC824FF-15E0-43FF-9B43-58662D4850C3}");
	public:
		AAFlagHandler() : m_flags(0) {}
	public:
		/*
		set the flags
		params:
		- int - flags to set (usually ored "|" together).
		*/
		virtual void SetFlags(long long flags) { this->m_flags = flags; }
		//returns the flags set
		virtual long long GetFlags() { return this->m_flags; }
	protected:
		long long m_flags;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AAFlagHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AAFlagHandler>("AAFlagHandler")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("SetFlags", &AAFlagHandler::SetFlags)
				->Method("GetFlags", &AAFlagHandler::GetFlags);
		}
	};
}