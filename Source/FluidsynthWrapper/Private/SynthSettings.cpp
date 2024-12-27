// Copyright Prog'z. All Rights Reserved.

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