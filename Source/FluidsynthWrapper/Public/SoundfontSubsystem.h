// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include <fluidsynth.h>
#include "SoundfontSubsystem.generated.h"

/**
 * 
 */
UCLASS(DisplayName = "Soundfont Subsystem")
class FLUIDSYNTHWRAPPER_API USoundfontSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
	fluid_audio_driver_t* driverCallback = nullptr;
	TMap<FName, TObjectPtr<class USynthInstance>> SynthInstances;
	TObjectPtr<class USynthSettings> DefaultSettings;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	class USynthInstance* CreateSynthInstance(const FName& InstanceName, USynthSettings* Settings = nullptr);
	UFUNCTION(BlueprintCallable)
	class USynthInstance* GetSynthInstance(const FName& InstanceName) const;
};
