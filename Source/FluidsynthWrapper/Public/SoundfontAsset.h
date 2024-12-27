// Copyright Prog'z. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundfontAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Category = "MIDI Generation", meta = (DisplayName = "MIDI Soundfont"))
class FLUIDSYNTHWRAPPER_API USoundfontAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SoundfontPath;
};
