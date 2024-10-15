// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundfontInstrument.h"

FSoundfontInstrument::FSoundfontInstrument()
{
}

FSoundfontInstrument::~FSoundfontInstrument()
{
}

void FSoundfontInstrument::NoteOn(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 Velocity, int8 MidiChannel, int32 EventTick, int32 CurrentTick, float MsOffset)
{
	SynthInstance->noteon(MidiChannel, MidiNoteNumber, Velocity);
}

void  FSoundfontInstrument::NoteOnWithFrameOffset(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 Velocity, int8 MidiChannel, int32 NumFrames)
{

}
bool  FSoundfontInstrument::NoteIsOn(int8 MidiNoteNumber, int8 MidiChannel)
{
	return false;
}
void  FSoundfontInstrument::NoteOff(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 MidiChannel)
{
	SynthInstance->noteoff(MidiChannel, MidiNoteNumber);
}
void  FSoundfontInstrument::NoteOffWithFrameOffset(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 MidiChannel, int32 NumFrames)
{

}

// pitch bend value, on range [-1.0, 1.0]
void  FSoundfontInstrument::SetPitchBend(float InValue, int8 InMidiChannel)
{
	SynthInstance->pitch_bend(InMidiChannel, int32(InValue));
}

// pitch bend value, on range [-1.0, 1.0]
float FSoundfontInstrument::GetPitchBend(int8 InMidiChannel) const
{
	return 0.0;
}

void FSoundfontInstrument::SetRawTransposition(int32 SemiTones)
{

}
int32 FSoundfontInstrument::GetRawTransposition() const
{
	return 0;
}
void FSoundfontInstrument::SetRawPitchMultiplier(float RawPitch)
{

}
float FSoundfontInstrument::GetRawPitchMultiplier() const
{
	return 1.0;
}

void FSoundfontInstrument::GetController(Harmonix::Midi::Constants::EControllerID InController, int8& InMsb, int8& InLsb, int8 InMidiChannel) const
{

}

void FSoundfontInstrument::KillAllVoices()
{

}
void FSoundfontInstrument::AllNotesOff()
{

}
void FSoundfontInstrument::AllNotesOffWithFrameOffset(int32 InNumFrames)
{

}

void FSoundfontInstrument::SetSpeed(float InSpeed, bool MaintainPitch)
{

}
float FSoundfontInstrument::GetSpeed(bool* MaintainPitch)
{
	return 1.0;
}

// update the effect with the current beat/tempo
void FSoundfontInstrument::SetBeat(float beat)
{
}
void FSoundfontInstrument::SetTempo(float bpm)
{
}

void FSoundfontInstrument::SetSampleRate(float InSampleRateHz)
{

}

int32 FSoundfontInstrument::GetNumVoicesInUse() const
{
	return 0;
}

void  FSoundfontInstrument::SetMidiChannelVolume(float InVolume, float InSeconds, int8 InMidiChannel)
{

}
float FSoundfontInstrument::GetMidiChannelVolume(int8 InMidiChannel) const
{
	return 1.0;
}
void  FSoundfontInstrument::SetMidiChannelGain(float InGain, float InSeconds, int8 InMidiChannel)
{

}
float FSoundfontInstrument::GetMidiChannelGain(int8 InMidiChannel) const
{
	return 0.0;
}
void  FSoundfontInstrument::SetMidiChannelMute(bool InMute, int8 InMidiChannel)
{

}
bool  FSoundfontInstrument::GetMidiChannelMute(int8 InMidiChannel) const
{
	return false;
}

void FSoundfontInstrument::Set7BitControllerImpl(Harmonix::Midi::Constants::EControllerID InController, int8 InValue, int8 InMidiChannel)
{

}
void FSoundfontInstrument::Set14BitControllerImpl(Harmonix::Midi::Constants::EControllerID InController, int16 InValue, int8 InMidiChannel)
{

}
void FSoundfontInstrument::ResetInstrumentStateImpl()
{
}

void FSoundfontInstrument::ResetMidiStateImpl()
{
}