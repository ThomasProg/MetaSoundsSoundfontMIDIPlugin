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

	UPROPERTY()
	TMap<FName, USynthInstance*> SynthInstances;

	UPROPERTY()
	class USynthSettings* DefaultSettings;

	int32 uuid = 0;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	USynthInstance* CreateNewSynthInstance(USynthSettings* Settings = nullptr);
	UFUNCTION(BlueprintCallable)
	USynthInstance* CreateSynthInstance(const FName& InstanceName, USynthSettings* Settings = nullptr);
	UFUNCTION(BlueprintCallable)
	USynthInstance* GetOrCreateSynthInstance(const FName& InstanceName, USynthSettings* Settings = nullptr);
	UFUNCTION(BlueprintCallable)
	USynthInstance* GetSynthInstance(const FName& InstanceName) const;

	UFUNCTION(BlueprintCallable)
	void Clear();
};
