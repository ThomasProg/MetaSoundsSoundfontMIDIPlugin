// Fill out your copyright notice in the Description page of Project Settings.


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