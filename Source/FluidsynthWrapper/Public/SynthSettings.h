// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <fluidsynth.h>
#include "SynthSettings.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class FLUIDSYNTHWRAPPER_API USynthSettings : public UObject
{
	GENERATED_BODY()
	
public:
	fluid_settings_t* Instance = nullptr;

	USynthSettings();
	~USynthSettings();
};
