#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore\RTTI\RTTI.h>

namespace AlternativeAudio {
	struct AAError {
		int code{ 0 };
		const char * str{ "NoError\0" };
		AZ_RTTI(AAError, "{D7705870-4EA0-4DA9-8891-1A728BA56FC4}");

		int GetCode() { return this->code; }
		const char * GetStr() { return this->str; }

		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AAError>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AAError>("AAError")
				->Method("GetCode", &AAError::GetCode)
				->Method("GetStr", &AAError::GetStr);
		}
	};

    class AAErrorHandler {
    public:
		AZ_RTTI(AAErrorHandler, "{A68D2813-85F1-4B5E-ADFF-707DF729A8D0}");
    public:
        AAErrorHandler() : m_hasError(false) {}
    public: //error checking
		//returns if the audio source has an error.
		bool HasError() {
			this->m_mutex.lock();
			bool ret = this->m_hasError;
			this->m_mutex.unlock();
			return ret;
		}
		/*
		returns what the error is in a human readable format.
		automatically clears error when there are no more errors to retrieve.
		*/
		AAError GetError() {
			if (this->m_errors.size() == 0) {
				AAError noErr;
				this->m_hasError = false;
				return noErr;
			}

			this->m_mutex.lock();
			AAError ret = this->m_errors.back();
			this->m_errors.pop_back();

			if (this->m_errors.size() == 0) this->m_hasError = false;
			this->m_mutex.unlock();

			return ret;
		}
	protected:
		//if the audio source has an error, this variable will be set.
		bool m_hasError;
		/*
		push an error to the list of errors.
		automatically sets the m_hasError variable.
		params:
		- int errorCode - error code for the specific audio source (library dependent).
		- const char * errorStr - human readable format for the error code.
		*/
		void pushError(int errorCode, const char * errorStr) {
			AAError err;
			err.code = errorCode;
			err.str = errorStr;
			this->m_mutex.lock();
			this->m_errors.push_back(err);
			this->m_hasError = true;
			this->m_mutex.unlock();
		}
	private:
		AZStd::vector<AAError> m_errors;
		AZStd::mutex m_mutex;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AAErrorHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}

		static void Behavior(AZ::BehaviorContext* behaviorContext) {
			behaviorContext->Class<AAErrorHandler>("AAErrorHandler")
				->Method("HasError", &AAErrorHandler::HasError)
				->Method("GetError", &AAErrorHandler::GetError);
		}
	};
}