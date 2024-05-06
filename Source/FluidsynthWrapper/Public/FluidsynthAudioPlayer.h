// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Fluidsynth/types.h"
#include "FluidsynthAudioPlayer.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class FLUIDSYNTHWRAPPER_API UFluidsynthAudioPlayer : public UObject
{
	GENERATED_BODY()

	fluid_settings_t* settings = nullptr;
	fluid_synth_t* synth = nullptr;
	fluid_audio_driver_t* driverCallback = nullptr;

	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintCallable)
	void LoadSoundfont(const FString& path = "C:/Users/thoma/PandorasBox/Projects/ModularMusicGenerationModules/Assets/Soundfonts/Touhou/Touhou.sf2");
	
	UFUNCTION(BlueprintCallable)
	void NoteOn(int32 channel, int32 key, int32 velocity);

	UFUNCTION(BlueprintCallable)
	void NoteOff(int32 channel, int32 key);

	UFUNCTION(BlueprintCallable)
	void ProgramChange(int32 channel, int32 newProgram);
};
