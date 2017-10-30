#pragma once

#include <AzCore/Component/Component.h>

#include <AlternativeAudio/AlternativeAudioBus.h>
#include <AlternativeAudio\DSP\AADSPEffectHandler.h>
#include "Built-in DSP\InterlaceDSP.h"

#include <AzCore/Math/Random.h>
#include <AzCore\Component\TickBus.h>

#define SOURCE_CACHE_STALE_COUNT 20

namespace AlternativeAudio {
	class AlternativeAudioSystemComponent
		: public AZ::Component
		, protected AlternativeAudioRequestBus::Handler
		, protected AlternativeAudioSourceBus::Handler
		, protected AlternativeAudioDSPBus::Handler
		, protected AlternativeAudioDeviceBus::Handler
		, public AADSPDeviceEffectHandler
		, public AZ::TickBus::Handler
	{
	public:
		AlternativeAudioSystemComponent();
		~AlternativeAudioSystemComponent();
	public:
		AZ_COMPONENT(AlternativeAudioSystemComponent, "{8C2C3BF1-C34E-4BEC-B299-A9AAC5276511}");

		static void Reflect(AZ::ReflectContext* context);

		static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);
		static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);
		static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);
		static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

	protected:
		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioRequestBus interface implementation
	protected: //frame conversion
		void ConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) { this->currentConvert(in, out, inType, outType, len); }
		void DefaultConvertAudioFrame(AudioFrame::Frame* in, AudioFrame::Frame* out, AudioFrame::Type inType, AudioFrame::Type outType, long long len) { this->defaultConvert(in, out, inType, outType, len); }
		void SetConvertFunction(ConvertAudioFrameFunc convertFunc);
		void ResetConvertFunction() { this->currentConvert = this->defaultConvert; }
	protected: //frame mixing
		void MixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) { this->currentMix(output, source, frameType, len); }
		void DefaultMixAudioFrames(AudioFrame::Frame* output, AudioFrame::Frame* source, AudioFrame::Type frameType, long long len) { this->defaultMix(output, source, frameType, len); }
		void SetMixFunction(MixAudioFramesFunc mixFunc);
		void ResetMixFunction() { this->currentMix = this->defaultMix; }
	protected: //audio utilities
		AudioFrame::Type GetAudioFormat(int numberOfChannels);
		int GetNumberOfChannels(AudioFrame::Type type);
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioSourceBus interface implementation
	protected: //IAudioSource
		void RegisterAudioLibrary(AZStd::string libname, AZ::Crc32 crc, AZStd::vector<AZStd::string> filetypes, NewAudioSourceFunc ptr);
		IAudioSource * NewAudioSource(AZ::Crc32 crc, AZStd::string path, void* userdata);
		SourceLibNameVector& GetAudioLibraryNames() { return *(this->m_sourceLibNames); }
	protected:
		void ClearAllCache();
		void ClearCache(AZ::Crc32 crc);
		void ClearCacheFile(AZ::Crc32 crc, AZStd::string path);
	protected:
		void CleanCache();
		void CleanCacheNow();
		void SetCleanCacheThreshold(unsigned long long val);
		unsigned long long GetCleanCacheThreshold();
	private: //IAudioSource
		using SourceLibMap = AZStd::unordered_map<AZ::Crc32, NewAudioSourceFunc>;
		using SourceLibMapPair = AZStd::pair<AZ::Crc32, NewAudioSourceFunc>;
		SourceLibMap *m_sourceLibFuncs;

		SourceLibNameVector *m_sourceLibNames;

		struct SourceCacheInfo {
			unsigned long long stale{ SOURCE_CACHE_STALE_COUNT };
			IAudioSourceLib * src{ nullptr };
		};

		using SourceCacheMap = AZStd::unordered_map<AZStd::string, SourceCacheInfo*>;
		using SourceCacheMapPair = AZStd::pair<AZStd::string, SourceCacheInfo*>;
		using LibSourceCacheMap = AZStd::unordered_map<AZ::Crc32, SourceCacheMap*>;
		using LibSourceCacheMapPair = AZStd::pair<AZ::Crc32, SourceCacheMap*>;
		LibSourceCacheMap *m_sharedSources;

		unsigned long long m_cleanCacheThreshold, m_cleanCacheIt;
		AZStd::mutex m_cacheMutex, m_cacheThreshold;
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioDSPBus interface implementation
	protected: //AADSPEffect
		void RegisterDSPEffect(AZStd::string libname, AZ::Crc32 crc, NewDSPEffectFunc ptr);
		AADSPEffect * NewDSPEffect(AZ::Crc32 crc, void* userdata);
		DSPLibNameVector& GetDSPEffectNames() { return *(this->m_dspLibNames); }
	protected:
		AADSPEffect * GetSharedDSPEffect(AZStd::string tag, AZ::Crc32 crc, void * userdata);
		void ReleaseSharedDSPEffect(AZStd::string tag);
	protected: //basic DSP system //remove as moving it to AADSPDeviceEffectHandler?
		bool AddEffect(AADSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot);
		unsigned long long AddEffectFreeSlot(AADSPSection section, AZ::Crc32 crc, void* userdata);
		bool AddSharedEffect(AZStd::string tag, AADSPSection section, AZ::Crc32 crc, void* userdata, unsigned long long slot);
		unsigned long long AddSharedEffectFreeSlot(AZStd::string tag, AADSPSection section, AZ::Crc32 crc, void* userdata);
		AADSPEffect * GetEffect(AADSPSection section, unsigned long long slot) { return AADSPDeviceEffectHandler::GetEffect(section, slot); }
		bool RemoveEffect(AADSPSection section, unsigned long long slot) { return AADSPDeviceEffectHandler::RemoveEffect(section, slot); }
		void ProcessEffects(AADSPSection section, AudioFrame::Type format, float* buffer, long long len, AAAttributeHandler * flags) { AADSPDeviceEffectHandler::ProcessEffects(section, format, buffer, len, flags); }
		AADSPDeviceEffectHandler* GetDSPDeviceEffectHandler() { return (AADSPDeviceEffectHandler*)this; }
	private: //AADSPEffect
		using DSPLibMap = AZStd::unordered_map<AZ::Crc32, NewDSPEffectFunc>;
		using DSPLibMapPair = AZStd::pair<AZ::Crc32, NewDSPEffectFunc>;
		DSPLibMap *m_dspLibFuncs;

		DSPLibNameVector *m_dspLibNames;

		using SharedDSPMap = AZStd::unordered_map<AZStd::string, AADSPEffect*>;
		using SharedDSPMapPair = AZStd::pair<AZStd::string, AADSPEffect*>;
		SharedDSPMap *m_sharedDSP;
		////////////////////////////////////////////////////////////////////////

		////////////////////////////////////////////////////////////////////////
		// AlternativeAudioDeviceBus
	protected:
		void RegisterPlaybackLibrary(AZStd::string libname, AZ::Crc32 crc, OAudioDeviceProvider* ptr);
		OAudioDevice * NewDevice(AZ::Crc32 crc, long long device, double samplerate, AlternativeAudio::AudioFrame::Type audioFormat, void* userdata);
		DeviceLibNameVector& GetPlaybackLibraryNames() { return *(this->m_deviceLibNames); }
		DeviceInfoVector& GetPlaybackDevices(AZ::Crc32 playbackLib);
		long long GetDefaultPlaybackDevice(AZ::Crc32 playbackLib);
		OAudioDeviceProvider* GetDeviceProvider(AZ::Crc32 playbackLib);
	protected:
		void SetMasterDevice(OAudioDevice * device);
		void SetStream(double samplerate, AudioFrame::Type audioFormat, void * userdata);
		void SetResampleQuality(AAResampleQuality quality);
		OAudioDeviceInfo GetDeviceInfo();
		unsigned long long PlaySource(AlternativeAudio::IAudioSource * source);
		void PlaySFXSource(AlternativeAudio::IAudioSource * source);
		void PauseSource(unsigned long long id);
		void ResumeSource(unsigned long long id);
		void StopSource(unsigned long long id);
		bool IsPlaying(unsigned long long id);
		AlternativeAudio::AudioSourceTime GetTime(unsigned long long id);
		void SetTime(unsigned long long id, double time);
		void UpdateAttribute(unsigned long long id, AZ::Crc32 idCrc, AlternativeAudio::AAAttribute* attr);
		void ClearAttribute(unsigned long long id, AZ::Crc32 idCrc);
		virtual void PauseAll();
		virtual void ResumeAll();
		virtual void StopAll();
		void Queue(bool startstop);
	private:
		using DeviceProvidersMap = AZStd::unordered_map<AZ::Crc32, OAudioDeviceProvider*>;
		using DeviceProvidersMapPair = AZStd::pair<AZ::Crc32, OAudioDeviceProvider*>;
		DeviceProvidersMap *m_deviceProviders;

		DeviceLibNameVector *m_deviceLibNames;

		AANullProvider m_nullProvider;
		DeviceInfoVector m_NullDeviceInfo;
		OAudioDevice * m_MasterDevice;
		////////////////////////////////////////////////////////////////////////
	private:
		////////////////////////////////////////////////////////////////////////
		// AZ::TickBus implementation
		//used for random cache cleaning
		void OnTick(float deltaTime, AZ::ScriptTimePoint time);
		int GetTickOrder() { return AZ::TICK_LAST; }
		AZ::SimpleLcgRandom m_rand;
		////////////////////////////////////////////////////////////////////////
	public:
		////////////////////////////////////////////////////////////////////////
		// AZ::Component interface implementation
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		////////////////////////////////////////////////////////////////////////
	private:
		DSP::InterleaveDSPEffect *interlaceDSP;
		DSP::DeinterleaveDSPEffect *deinterlaceDSP;
	private:
		ConvertAudioFrameFunc defaultConvert, currentConvert;
		MixAudioFramesFunc defaultMix, currentMix;
	private:
		bool m_commandsRegistered;
	};

	//used to share a common audio source between multiple areas but have different dsp effects.
	class IAudioSourceShared : public IAudioSource {
	public:
		AZ_RTTI(IAudioSourceShared, "{02405B5D-BB87-4A00-934A-E8F58361773F}", IAudioSource);
	public:
		IAudioSourceShared(IAudioSourceLib* source) {
			this->m_pSource = source;
			this->m_pSource->AddRef();
			this->m_type = source->GetFrameType();
			this->m_pSource->AddErrorHandler(this);
			this->CopyAttributes(*source);
		}
		~IAudioSourceShared() {
			this->m_pSource->RemoveErrorHandler(this);
			this->m_pSource->Release();
		}
	public:
		bool Seek(long long position) { return this->m_pSource->Seek(position); }
		long long GetFrames(long long framesToRead, float* buff) {
			if (this->m_hasError) return 0;
			long long ret = this->m_pSource->GetFrames(framesToRead, buff);
			this->ProcessEffects(this->m_type, buff, ret, this);
			return ret;
		}
		bool GetFrame(float* frame) {
			if (this->m_hasError) return false;
			bool ret = this->m_pSource->GetFrame(frame);
			this->ProcessEffects(this->m_type, frame, 1, this);
			return ret;
		}
		double GetSampleRate() { return this->m_pSource->GetSampleRate(); }
		const AlternativeAudio::AudioFrame::Type GetFrameType() { return this->m_pSource->GetFrameType(); }
		AudioSourceTime GetLength() { return this->m_pSource->GetLength(); }
		long long GetFrameLength() { return this->m_pSource->GetFrameLength(); }
	public:
		operator IAudioSourceLib*() { return this->m_pSource; }
	private:
		IAudioSourceLib* m_pSource;
		AlternativeAudio::AudioFrame::Type m_type;
	};

	class AADSPEffectShared : public AADSPEffect {
	public:
		AZ_RTTI(AADSPEffectShared, "{ED908D8E-C7F7-41A8-AE4E-283AD472BF5C}", AADSPEffect);
	public:
		AADSPEffectShared(AZStd::string tag, AADSPEffect* effect) {
			this->m_pDSPEffect = effect;
			this->m_pDSPEffect->AddRef(); //ref++
			this->m_tag = tag;
		}
		~AADSPEffectShared() {
			//this->m_pDSPEffect->Release(); //ref--
			EBUS_EVENT(AlternativeAudioDSPBus, ReleaseSharedDSPEffect, this->m_tag); //ref--
		}
	public:
		AZStd::string GetName() { return this->m_pDSPEffect->GetName(); }
		int GetDSPSection() { return this->m_pDSPEffect->GetDSPSection(); }
		AADSPProcessType GetProcessType() { return this->m_pDSPEffect->GetProcessType(); }
	public:
		void Process(AudioFrame::Type format, float * buffer, long long len, AAAttributeHandler * attributes) { this->m_pDSPEffect->Process(format, buffer, len, attributes); }
		void ProcessFrame(AudioFrame::Type format, float * frame, AAAttributeHandler * attributes) { this->m_pDSPEffect->ProcessFrame(format, frame, attributes); }
	public:
		operator AADSPEffect*() override { return this->m_pDSPEffect->get(); }
		virtual AADSPEffect* get() override { return this->m_pDSPEffect->get(); }
	private:
		AADSPEffect* m_pDSPEffect;
		AZStd::string m_tag;
	};
}
