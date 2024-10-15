// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HarmonixDsp/Instruments/VirtualInstrument.h"

#include "SynthInstance.h"

/**
 * 
 */
class FLUIDSYNTHWRAPPER_API FSoundfontInstrument : public FVirtualInstrument
{
public:
	FSoundfontInstrument();
	virtual ~FSoundfontInstrument();

	//*************************************************************************
	// PInstrument state and MIDI message handling
	virtual void  NoteOn(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 Velocity, int8 MidiChannel = 0, int32 EventTick = 0, int32 CurrentTick = 0, float MsOffset = 0.0f) override;
	virtual void  NoteOnWithFrameOffset(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 Velocity, int8 MidiChannel = 0, int32 NumFrames = 0) override;
	virtual bool  NoteIsOn(int8 MidiNoteNumber, int8 MidiChannel = 0) override;
	virtual void  NoteOff(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 MidiChannel = 0) override;
	virtual void  NoteOffWithFrameOffset(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 MidiChannel = 0, int32 NumFrames = 0) override;

	// pitch bend value, on range [-1.0, 1.0]
	virtual void  SetPitchBend(float InValue, int8 InMidiChannel = 0) override;

	// pitch bend value, on range [-1.0, 1.0]
	virtual float GetPitchBend(int8 InMidiChannel = 0) const override;

	virtual void SetRawTransposition(int32 SemiTones) override;
	virtual int32 GetRawTransposition() const override;
	virtual void SetRawPitchMultiplier(float RawPitch) override;
	virtual float GetRawPitchMultiplier() const override;

	virtual void GetController(Harmonix::Midi::Constants::EControllerID InController, int8& InMsb, int8& InLsb, int8 InMidiChannel = 0) const override;

	virtual void KillAllVoices() override;
	virtual void AllNotesOff() override;
	virtual void AllNotesOffWithFrameOffset(int32 InNumFrames = 0) override;

	virtual void SetSpeed(float InSpeed, bool MaintainPitch = false) override;
	virtual float GetSpeed(bool* MaintainPitch = nullptr) override;

	// update the effect with the current beat/tempo
	virtual void SetBeat(float beat) override;
	virtual void SetTempo(float bpm) override;

	virtual void SetSampleRate(float InSampleRateHz) override;

	virtual int32 GetNumVoicesInUse() const override;

	virtual void  SetMidiChannelVolume(float InVolume, float InSeconds = 0.0f, int8 InMidiChannel = 0) override;
	virtual float GetMidiChannelVolume(int8 InMidiChannel = 0) const override;
	virtual void  SetMidiChannelGain(float InGain, float InSeconds = 0.0f, int8 InMidiChannel = 0) override;
	virtual float GetMidiChannelGain(int8 InMidiChannel = 0) const override;
	virtual void  SetMidiChannelMute(bool InMute, int8 InMidiChannel = 0) override;
	virtual bool  GetMidiChannelMute(int8 InMidiChannel = 0) const override;

	void SetTicksPerQuarterNote(int32 InTicksPerQuarterNote) { TicksPerQuarterNote = InTicksPerQuarterNote; }
	int32 GetTicksPerQuarterNote() const { return TicksPerQuarterNote; }

protected:
	virtual void Set7BitControllerImpl(Harmonix::Midi::Constants::EControllerID InController, int8 InValue, int8 InMidiChannel = 0) override;
	virtual void Set14BitControllerImpl(Harmonix::Midi::Constants::EControllerID InController, int16 InValue, int8 InMidiChannel = 0) override;
	virtual void ResetInstrumentStateImpl() override; // heavy weight! should completely resets the instrument.
	virtual void ResetMidiStateImpl() override;       // less heavy weight. should do all notes off, reset pitch bend, etc.

public:
	TWeakObjectPtr<USynthInstance> SynthInstance = nullptr;

private:
	int32 TicksPerQuarterNote = Harmonix::Midi::Constants::GTicksPerQuarterNoteInt;
};
