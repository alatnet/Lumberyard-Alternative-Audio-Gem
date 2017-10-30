#pragma once

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore\RTTI\RTTI.h>

#include <AzCore\RTTI\ReflectContext.h>

namespace AlternativeAudio {
	//parts of the code is taken from "AZCore/RTTI/ReflectContext.h" and "AZCore/Serialization/EditContext.h" and modified to support additional features.

	class AAAttribute {
	public:
		AZ_RTTI(AAAttribute, "{B3E7E012-A762-4C88-924A-D63800B4AD03}");
	public:
		AAAttribute() {}
		virtual ~AAAttribute() = default;
	public:
		virtual AAAttribute* clone() const = 0;
	private:
	};

	template<class T>
	class AAAttributeData : public AAAttribute {
	public:
		AZ_RTTI((AAAttributeData<T>, "{35DCBB1E-1584-4CA7-AAB1-61B412C88C46}", T), AAAttribute);
		AZ_CLASS_ALLOCATOR(AAAttributeData<T>, AZ::SystemAllocator, 0)
	public:
		template<class U>
		explicit AAAttributeData(U data) : m_data(data) {}
	public:
		virtual const T& Get() const { return m_data; }
		virtual void Set(T& data) { m_data = data; }
	public:
		T& operator = (T& data) { m_data = data; return m_data; }
		T& operator = (const T& data) { m_data = data; return m_data; }
	public:
		AAAttribute* clone() const {
			return aznew AAAttributeData<T>(this->m_data);
		}
	private:
		T m_data;
	};

	template<class T>
	class AAAttributeFunction;

	template<class R, class... Args>
	class AAAttributeFunction<R(Args...)> : public AAAttribute {
	public:
		AZ_RTTI((AAAttributeFunction<R(Args...)>, "{6AFD5576-C7F8-439B-894A-51029ED3C754}", R, Args...), AAAttribute);
		AZ_CLASS_ALLOCATOR(AAAttributeFunction<R(Args...)>, AZ::SystemAllocator, 0);
	public:
		typedef R(*FunctionPtr)(Args...);
	public:
		explicit AAAttributeFunction(FunctionPtr f) : m_function(f) {}
	public:
		virtual R Invoke(Args&&... args) { return m_function(AZStd::forward<Args>(args) ...); }
	public:
		virtual AZ::Uuid GetInstanceType() const { return AZ::Uuid::CreateNull(); }
	public:
		AAAttribute* clone() const {
			return aznew AAAttributeFunction<R(Args...)>(this->m_function);
		}
	public:
		FunctionPtr m_function;
	};

	class AAAttributeHandler {
	public:
		AZ_RTTI(AAAttributeHandler, "{BD5D2CC8-5B08-488A-9E4E-B3423E387121}");
	public:
		AAAttributeHandler() {}
		AAAttributeHandler(const AAAttributeHandler& handler) { this->CopyAttributes(handler); }
		~AAAttributeHandler(){ this->m_attr.clear(); }
	public:
		template<class T>
		AAAttributeHandler* Attr(const char* id, T value) { return Attribute(AZ::Crc32(id), value); }
		template<class T>
		AAAttributeHandler* Attr(AZ::Crc32 idCrc, T value) { return Attribute(idCrc, value); }
		AAAttributeHandler* Attr(AZ::Crc32 idCrc, AAAttribute* value) { return Attribute(idCrc, value); }
	public:
		template<class T>
		AAAttributeHandler* Attribute(const char* id, T value) { return Attribute(AZ::Crc32(id), value); }
		template<class T>
		AAAttributeHandler* Attribute(AZ::Crc32 idCrc, T value) {
			typedef typename AZStd::Utils::if_c<
				AZStd::is_function<typename AZStd::remove_pointer<T>::type>::value,
				AAAttributeFunction<typename AZStd::remove_pointer<T>::type>,
				AAAttributeData<T>
			>::type ContainerType;

			auto entry = this->m_attr.find(idCrc);
			if (entry != this->m_attr.end()) this->m_attr.erase(entry);

			this->m_attr.insert(AttributePair(idCrc, aznew ContainerType(value)));

			return this;
		}
		AAAttributeHandler* Attribute(AZ::Crc32 idCrc, AAAttribute* value) {
			auto entry = this->m_attr.find(idCrc);
			if (entry != this->m_attr.end()) this->m_attr.erase(entry);

			this->m_attr.insert(AttributePair(idCrc, value));

			return this;
		}
	public:
		AAAttribute* getAttr(const char* id) { return this->getAttr(AZ::Crc32(id)); }
		AAAttribute* getAttr(AZ::Crc32 idCrc) {
			auto entry = this->m_attr.find(idCrc);
			if (entry != this->m_attr.end()) return entry->second;
			return nullptr;
		}
	public:
		template<class T>
		AAAttributeData<T>* getDataAttr(const char* id) { return getDataAttr<T>(AZ::Crc32(id)); }
		template<class T>
		AAAttributeData<T>* getDataAttr(AZ::Crc32 idCrc) { return (AAAttributeData<T>*)this->getAttr(idCrc); }
	public:
		template<class T>
		T getAttrValue(const char* id) { return this->getAttrValue<T>(AZ::Crc32(id)); }
		template<class T>
		T getAttrValue(AZ::Crc32 idCrc) { return this->getDataAttr<T>(idCrc)->Get(); }
	public:
		template<class R, class... Args>
		AAAttributeFunction<R(Args...)>* getAttrFunc(const char* id) { return this->getAttr<R(Args...)>(AZ::Crc32(id)); }
		template<class R, class... Args>
		AAAttributeFunction<R(Args...)>* getAttrFunc(AZ::Crc32 idCrc) { return (AAAttributeFunction<R(Args...)>*)this->getAttr(idCrc); }
	/*public:
		template<class R, class... Args>
		R invokeAttr(const char id, Args&&... args) { return this->invokeAttr<R, Args...>(AZ::Crc32(id), args); }
		template<class R, class... Args>
		R invokeAttr(AZ::Crc32 idCrc, Args&&... args) { return ((AAAttributeFunction<R(Args...)>*)this->getAttrFunc<R, Args...>(idCrc))->Invoke(args); }*/
	public:
		template<class T>
		AAAttributeHandler* setAttr(const char* id, T value) { return this->Attribute<T>(id, value); }
		template<class T>
		AAAttributeHandler* setAttr(AZ::Crc32 idCrc, T value) { return this->Attribute<T>(idCrc, value); }
		AAAttributeHandler* setAttr(AZ::Crc32 idCrc, AAAttribute* value) { return this->Attribute(idCrc, value); }
	public:
		template<class T>
		AAAttributeHandler* setAttrValue(const char* id, T value) { return this->setAttrValue<T>(AZ::Crc32(id), value); }
		template<class T>
		AAAttributeHandler* setAttrValue(AZ::Crc32 idCrc, T value) {
			AAAttributeData<T>* entry = (AAAttributeData<T>*) this->getDataAttr<T>(idCrc);
			&entry = value;
			return this;
		}
	public:
		void unsetAttr(const char* id) { this->unsetAttr(AZ::Crc32(id)); }
		void unsetAttr(AZ::Crc32 idCrc) { this->m_attr.erase(idCrc); }
	public:
		bool hasAttr(const char* id) { return this->hasAttr(AZ::Crc32(id)); }
		bool hasAttr(AZ::Crc32 idCrc) {
			auto entry = this->m_attr.find(idCrc);
			if (entry != this->m_attr.end()) return true;
			return false;
		}
	public:
		template<class T>
		static AAAttribute* CreateAttribute(T value) {
			typedef typename AZStd::Utils::if_c<
				AZStd::is_function<typename AZStd::remove_pointer<T>::type>::value,
				AAAttributeFunction<typename AZStd::remove_pointer<T>::type>,
				AAAttributeData<T>
			>::type ContainerType;

			return aznew ContainerType(value);
		}
	protected:
		void CopyAttributes(const AAAttributeHandler& handler) {
			auto entry = handler.m_attr.begin();
			while (entry != handler.m_attr.end()) {
				//clear the attribute
				//if (this->m_attr.find(entry->first) != this->m_attr.end()) this->m_attr.erase(entry->first);

				//copy the attribute
				this->m_attr.insert(AttributePair(entry->first, entry->second->clone()));
				++entry;
			}
		}
	protected:
		using AttributePair = AZStd::pair<AZ::AttributeId, AAAttribute*>;
		using AttributeMap = AZStd::unordered_map<AZ::AttributeId, AAAttribute*>;
		AttributeMap m_attr;
	public:
		static void Reflect(AZ::SerializeContext* serialize) {
			serialize->Class<AAAttributeHandler>()
				->Version(0)
				->SerializerForEmptyClass();
		}
	};
}