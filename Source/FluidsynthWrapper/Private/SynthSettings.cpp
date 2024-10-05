// Fill out your copyright notice in the Description page of Project Settings.


#include "SynthSettings.h"

USynthSettings::USynthSettings()
{
	Instance = new_fluid_settings();
}

USynthSettings::~USynthSettings()
{
	if (Instance != nullptr)
	{
		delete_fluid_settings(Instance);
	}
}