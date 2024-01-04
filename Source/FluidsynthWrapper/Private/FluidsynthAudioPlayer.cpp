// Fill out your copyright notice in the Description page of Project Settings.


#include "FluidsynthAudioPlayer.h"
#include <fluidsynth.h>

void UFluidsynthAudioPlayer::PostInitProperties()
{
	UObject::PostInitProperties();

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Create settings and synth
		settings = new_fluid_settings();
		synth = new_fluid_synth(settings);

		// Create an audio driver
		//driverCallback = new_fluid_audio_driver2(settings, fluidsynth_callback, this);
		driverCallback = new_fluid_audio_driver(settings, synth);
	}
}

void UFluidsynthAudioPlayer::LoadSoundfont(const FString& path)
{
	fluid_synth_sfload(synth, TCHAR_TO_ANSI(*path), 1);
}

void UFluidsynthAudioPlayer::NoteOn(int32 channel, int32 key, int32 velocity)
{
	fluid_synth_noteon(synth, channel, key, velocity);
}

void UFluidsynthAudioPlayer::NoteOff(int32 channel, int32 key)
{
	fluid_synth_noteoff(synth, channel, key);
}

void UFluidsynthAudioPlayer::BeginDestroy()
{
	if (driverCallback)
	{
		delete_fluid_audio_driver(driverCallback);
		driverCallback = nullptr;
	}

	if (synth)
	{
		delete_fluid_synth(synth);
		synth = nullptr;
	}
	if (settings)
	{
		delete_fluid_settings(settings);
		settings = nullptr;
	}

	UObject::BeginDestroy();
}