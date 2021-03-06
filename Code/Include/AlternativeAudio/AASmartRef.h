#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore\RTTI\RTTI.h>
#include <smartptr.h>

namespace AlternativeAudio {
	class AASmartRef : public _i_multithread_reference_target_t {
	public:
		AZ_RTTI(AASmartRef, "{B057D673-F47B-409F-8002-AD478E5C33AC}");
	public:
		virtual void AddRef() { _i_multithread_reference_target_t::AddRef(); }
		virtual void Release() { _i_multithread_reference_target_t::Release(); }
		virtual int NumRefs() { return _i_multithread_reference_target_t::NumRefs(); }
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AASmartRef>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AASmartRef>("AASmartRef")
				->Attribute(AZ::Script::Attributes::Category, "Alternative Audio")
				->Method("AddRef", &AASmartRef::AddRef)
				->Method("Release", &AASmartRef::Release)
				->Method("NumRefs", &AASmartRef::NumRefs);
		}
	};
}