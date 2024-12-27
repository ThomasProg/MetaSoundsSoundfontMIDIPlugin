// Copyright Prog'z. All Rights Reserved.

#include "SoundfontSubsystem.h"
#include "SynthInstance.h"
#include "SynthSettings.h"

void USoundfontSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Initialization code here
    DefaultSettings = NewObject<USynthSettings>();
}

void USoundfontSubsystem::Deinitialize()
{
    // Cleanup code here

	if (driverCallback)
	{
		delete_fluid_audio_driver(driverCallback);
		driverCallback = nullptr;
	}

    Super::Deinitialize();
}

USynthInstance* USoundfontSubsystem::CreateNewSynthInstance(FName InstanceName, USynthSettings* Settings)
{
    if (InstanceName.IsNone())
    {
        FString NameStr = "Generated_" + FString::FromInt(uuid);
        InstanceName = FName(*NameStr);
    }
    SynthInstances.Remove(InstanceName);
    return CreateSynthInstance(InstanceName, Settings);
}

USynthInstance* USoundfontSubsystem::CreateSynthInstance(const FName& InstanceName, USynthSettings* Settings)
{
    if (SynthInstances.Contains(InstanceName))
    {
        UE_LOG(LogTemp, Warning, TEXT("FluidSynth instance '%s' already exists."), *InstanceName.ToString());
        return SynthInstances[InstanceName];
    }

    TObjectPtr<class USynthSettings> SynthSettings = Settings ? Settings : DefaultSettings;
    class USynthInstance* Synth = USynthInstance::CreateInstance(SynthSettings).Get();

    if (!Synth)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FluidSynth instance '%s'."), *InstanceName.ToString());
        return nullptr;
    }

    uuid += 1;
    SynthInstances.Add(InstanceName, Synth);
    UE_LOG(LogTemp, Log, TEXT("FluidSynth instance '%s' created."), *InstanceName.ToString());
    return Synth;
}

USynthInstance* USoundfontSubsystem::GetOrCreateSynthInstance(const FName& InstanceName, USynthSettings* Settings)
{
    USynthInstance* ptr = GetSynthInstance(InstanceName);
    if (ptr == nullptr)
    {
        ptr = CreateSynthInstance(InstanceName, Settings);
    }

    return ptr;
}

USynthInstance* USoundfontSubsystem::GetSynthInstance(const FName& InstanceName) const
{
    return SynthInstances.FindRef(InstanceName);
}

void USoundfontSubsystem::Clear()
{
    //for (auto [key, value] : SynthInstances)
    //{
    //    
    //}

    SynthInstances.Empty();
}