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

    TObjectPtr<class USynthSettings> SynthSettings = Settings ? Settings : DefaultSettings.Get();
    TObjectPtr<class USynthInstance> Synth = USynthInstance::CreateInstance(Settings);

    if (!Synth)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to create FluidSynth instance '%s'."), *InstanceName.ToString());
        return nullptr;
    }

    SynthInstances.Add(InstanceName, Synth);
    UE_LOG(LogTemp, Log, TEXT("FluidSynth instance '%s' created."), *InstanceName.ToString());
    return Synth;
}
USynthInstance* USoundfontSubsystem::GetSynthInstance(const FName& InstanceName) const
{
    if (SynthInstances.Contains(InstanceName))
    {
        return SynthInstances[InstanceName];
    }
    UE_LOG(LogTemp, Warning, TEXT("FluidSynth instance '%s' not found."), *InstanceName.ToString());
    return nullptr;
}