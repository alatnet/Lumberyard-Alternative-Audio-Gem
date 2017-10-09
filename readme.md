# Alternative Audio Lumberyard Gem
An Amazon Lumberyard Gem that adds in an alternative audio system.

## Requirements
Usage of the gem requires that you have a companion audio library gem and audio playback gem.  
You can optinally add in a DSP library gem to apply DSP effects to either an audio source or playback.

## C++ Usage Example
```C++
#include <AlternativeAudio/AlternativeAudioBus.h>
#include <AlternativeAudio\DSP\VolumeDSPBus.h>

AlternativeAudio::OAudioDevice* defaultDevice;
AlternativeAudio::AADSPEffect* masterVolumeDSP;

void MusicInit(){
    //setup audio devices
    auto lib = AZ::Crc32("ExampleAudioPlayback");
    long long defaultDeviceId = -1;

    //get default device id
    EBUS_EVENT_RESULT(
        defaultDeviceId,
        AlternativeAudio::AlternativeAudioDeviceBus,
        GetDefaultPlaybackDevice,
        lib
    );

    //create new device with default device id
    EBUS_EVENT_RESULT(
        defaultDevice,
        AlternativeAudio::AlternativeAudioDeviceBus,
        NewDevice,
        lib,
        defaultDeviceId,
        44100.0, //samplerate
        AlternativeAudio::AudioFrame::Type::eT_af2, //audio format
        nullptr //device userdata
    );

    //set the master device for alternative audio
    EBUS_EVENT(
        AlternativeAudio::AlternativeAudioDeviceBus,
        SetMasterDevice,
        defaultDevice
    );

    //global master volume dsp effect setup
    EBUS_EVENT(
        AlternativeAudio::AlternativeAudioDSPBus,
        AddEffect,
        AlternativeAudio::AADSPSection::eDS_Output, //where to set the dsp
        AZ_CRC("AAVolumeControl", 0x722dd2a9), //which dsp effect to use
        nullptr, //dsp userdata
        99 //slot number
    );

    EBUS_EVENT_RESULT(
        this->m_masterVolumeDSP,
        AlternativeAudio::AlternativeAudioDSPBus,
        GetEffect,
        AlternativeAudio::AADSPSection::eDS_Output,
        99
    );

    masterVolumeDSP->AddRef(); //add a reference
}

void MusicShutdown(){
    //close master device
    EBUS_EVENT(
        AlternativeAudio::AlternativeAudioDeviceBus,
        SetMasterDevice,
        nullptr
    );

    //remove global dsp effect
    EBUS_EVENT(
        AlternativeAudio::AlternativeAudioDSPBus,
        RemoveEffect,
        AlternativeAudio::AADSPSection::eDS_Output,
        99
    );

    masterVolumeDSP->Release(); //free up the dsp effect
}

int main(){
    MusicInit();
    
    auto lib = AZ::Crc32("ExampleAudioLibrary");
    AlternativeAudio::IAudioSource* source;
    AlternativeAudio::AADSPEffect* sourceVolume;
    long long sourceID;
    
    //load an audio source
    EBUS_EVENT_RESULT(
        source,
        AlternativeAudio::AlternativeAudioSourceBus,
        NewAudioSource,
        lib,
        "<AudioPath>",
        nullptr //library userdata
    );
    
    //create an audio source volume dsp effect
    EBUS_EVENT_RESULT(
        sourceVolume,
        AlternativeAudio::AlternativeAudioDSPBus,
        NewDSPEffect,
        AZ_CRC("AAVolumeControl", 0x722dd2a9), //dsp effect name
        nullptr //dsp userdata
    );

    sourceVolume->AddRef();

    //set the source volume dsp effect to a free slot.
    source->AddEffectFreeSlot(sourceVolume);

    //set volume to 50% on the source
    EBUS_EVENT_ID(
        sourceVolume,
        AlternativeAudio::DSP::VolumeDSPBus,
        SetVol,
        0.5f //(range from 0.0f to 1.0f)
    );

    //start the playback
    EBUS_EVENT_RESULT(
        sourceID, //returns the source id for playback control
        AlternativeAudio::AlternativeAudioDeviceBus,
        PlaySource,
        source
    );

    bool isPlaying = true;

    while (isPlaying){
        //keep querying to make sure that the audio source is still playing.
        EBUS_EVENT_RESULT(
            isPlaying,
            AlternativeAudio::AlternativeAudioDeviceBus,
            IsPlaying,
            sourceID
        );
    }

    delete source; //delete the audio source
    sourceVolume->Release(); //free up the source volume dsp effect.

    MusicShutdown();
}
```

## Lua Usage Example
```Lua
Example = {
    Properties = {
    },
    deviceLib = Crc32("ExampleAudioPlayback"),
    audioLib = Crc32("ExampleAudioLibrary")
}

function Example:MusicInit()
    --get default device id
    defaultDeviceId = AlternativeAudioDeviceBus.Broadcast.GetDefaultPlaybackDevice(self.deviceLib)

    --create new device with default device id
    self.device = AlternativeAudioDeviceBus.Broadcast.NewDevice(self.deviceLib)

    --set the master device for alternative audio
    AlternativeAudioDeviceBus.Broadcast.SetMasterDevice(self.device)

    --global master volume dsp effect setup
    AlternativeAudioDSPBus.Broadcast.AddEffect(
        AADSPSection.Output, --where to set the dsp effect
        Crc32("AAVolumeControl"), --which dsp effect to use
        nil, --dsp userdata
        99 --slot number
    )

    self.masterVolume = AlternativeAudioDSPBus.Broadcast.GetEffect(AADSPSection.Output, 99)
    self.masterVolume:AddRef()
end

function Example:MusicShutdown()
    --close master device
    AlternativeAudioDeviceBus.Broadcast.SetMasterDevice(nil)
    
    --remove global dsp effect
    AlternativeAudioDSPBus.Broadcast.RemoveEffect(AADSPSection.Output, 99)
    self.masterVolume:Release()
end

function Example:OnActivate()
    self:MusicInit()

    --load an audio source
    self.source = AlternativeAudioSourceBus.Broadcast.NewAudioSource(self.audioLib, "<AudioPath>", nil)

    --create an audio source volume dsp effect
    self.sourceVolume = AlternativeAudioDSPBus.Broadcast.NewDSPEffect(Crc32("AAVolumeControl"), nil)
    self.sourceVolume:AddRef()
   
    --set the source volume dsp effect to a free slot.
    self.source.AddEffectFreeSlot(self.sourceVolume)

    --set volume to 50% on the source
    AAVolumeDSPBus.Event.SetVol(self.sourceVolume, 0.5)

    self.sourceID = AlternativeAudioDeviceBus.Broadcast.PlaySource(self.source)
end

function Example:OnDeactivate()
    --stop the source playback
    AlternativeAudioDeviceBus.Broadcast.StopSource(self.sourceID)

    self.source = nil --delete the audio source (lua GC will do this automatically)
    self.sourceVolume:Release() --free up the source volume dsp effect
    self:MusicShutdown()
end

return Example
```

## Creating an Audio Playback Gem
Example Playback Gem: [Alternative Audio - Port Audio Lumberyard Gem](https://github.com/alatnet/Lumberyard-Alternative-Audio-Port-Audio-Gem)  
To start off, create a gem to hold your code.  
How you code the playback gem is up to you but the instructions layed out is how I have coded a playback gem.  
Have your system component inherit from AlternativeAudio\:\:OAudioDeviceProvider which is provided from the include file "AlternativeAudio\Device\OAudioDevice.h".  
The OAudioDeviceProvider is a class to handle device creation and destruction and is required that you implement all of the functions needed.  
Next you will need to create a class to have the device provider, well, provide.  
An audio device inherits from AlternativeAudio\:\:OAudioDevice, which is provided from the include "AlternativeAudio\Device\OAudioDevice.h".  
The OAudioDevice is a class to handle source playback on an individual device and is required that you implement all of the required functions.  
Finally after a provider and device have been coded, it is time to register the playback with Alternative Audio itself.  
This is only required once.  
In the system gem's Activate function, you use the Alternative Audio's "RegisterPlaybackLibrary" ebus function provided by AlternativeAudioDeviceBus.  
Example:  
```C++
EBUS_EVENT(
    AlternativeAudio::AlternativeAudioDeviceBus,
    RegisterPlaybackLibrary,
    "<Library Name>",
    AZ_CRC("<Library Name>"),
    <OAudioDeviceProvider>
);
```
With this, an audio playback gem is registered and usable within Alternative Audio using the AZ::Crc32 of what you have set as your library name.  
You can choose to set the library name to the playback library's name or to a name of your choosing.  
Do note that registration will work if two libraries will share the same name but the second library will not be accessable as Alternative Audio uses the first library it finds.

## Creating an Audio Library Gem
Example Library Gem: [Alternative Audio - Libsndfile Lumberyard Gem](https://github.com/alatnet/Lumberyard-Alternative-Audio-Libsndfile-Gem)  
To start off, create a gem to hold your code.  
How you code the library gem is up to you but the instructions layed out is how I have coded a library gem.  
Create a class and have it inherit from AlternativeAudio\:\:IAudioSource which is provided by the include "AlternativeAudio\AudioSource\IAudioSource.h".  
The IAudioSoruce is a class to handle reading data from an audio file or source and is required that you implement all of the required functions.  
After an audio source has been coded it is time to register the library with Alternative Audio itself.  
This is required only once.  
In the system gem's Activate function, you use the Alternative Audio's "RegisterAudioLibrary" ebus function provided by AlternativeAudioSourceBus.  
Example:
```C++
AZStd::vector<AZStd::string> filetypes;
filetypes.push_back("*.wav");

EBUS_EVENT(
    AlternativeAudio::AlternativeAudioSourceBus,
    RegisterAudioLibrary,
    "<Library Name>",
    AZ_CRC("<Library Name>"),
    filetypes, //a vector of filetypes that the source provides
    // you can use a lambda function or a separate class function to provide a new audio source.
    [](const char * path, void * userdata)-> AlternativeAudio::IAudioSource* {
	    return new AudioSourceClass(path);
    }
);
```
With this, an audio library gem is registered and usable within Alternative Audio using the AZ::Crc32 of what you have set as your library name.  
You can choose to set the library name to the playback library's name or to a name of your choosing.  
Do note that registration will work if two libraries will share the same name but the second library will not be accessable as Alternative Audio uses the first library it finds.

## Creating a DSP Library Gem
Creating a DSP gem is almost exactly the same as creating an audio library gem.  
The only difference is that you are inheriting from AlternativeAudio\:\:AADSPEffect instead.  
Afterwards, registration of the gem is the same as an audio library gem except for which ebus to use to register the dsp library gem.  
Example:
```C++
EBUS_EVENT(
    AlternativeAudio::AlternativeAudioDSPBus,
    RegisterDSPEffect,
    "<Library Name>",
    AZ_CRC("<Library Name>"),
    // you can use a lambda function or a separate class function to provide a new DSP effect.
    [](void* userdata)-> AADSPEffect* { return new DSP::VolumeDSPEffect(userdata); }
);
```
As stated with the audio library gem creation, two libraries can share the same name but the second one will not be accessable.