// Copyright Prog'z. All Rights Reserved.

#include "SoundfontInstrument.h"

FSoundfontInstrument::FSoundfontInstrument()
{
}

FSoundfontInstrument::~FSoundfontInstrument()
{
}


void FSoundfontInstrument::HandleMidiMessage(FMidiVoiceId InVoiceId, int8 InStatus, int8 InData1, int8 InData2, int32 InEventTick, int32 InCurrentTick, float InMsOffset)
{
	using namespace Harmonix::Midi::Constants;
	int8 InChannel = InStatus & 0xF;
	switch (InStatus & 0xF0)
	{
	case GNoteOff:
		NoteOff(InVoiceId, InData1, InChannel);
		break;
	case GNoteOn:
		NoteOn(InVoiceId, InData1, InData2, InChannel, InEventTick, InCurrentTick, InMsOffset);
		break;
	case GPolyPres:
		PolyPressure(InVoiceId, InData1, InData2, InChannel);
		break;
	case GChanPres:
		ChannelPressure(InData1, InData2, InChannel);
		break;
	case GControl:
		SetHighOrLowControllerByte((EControllerID)InData1, InData2, InChannel);
		break;
	case GPitch:
		SetPitchBend(FMidiMsg::GetPitchBendFromData(InData1, InData2), InChannel);
		break;
	case GProgram:
		ProgramChange(InVoiceId, InData1, InChannel);
		break;

	}
}

void FSoundfontInstrument::ProgramChange(FMidiVoiceId InVoiceId, int8 InProgram, int8 MidiChannel)
{
	SynthInstance->program_change(MidiChannel, InProgram);
}


void FSoundfontInstrument::NoteOn(FMidiVoiceId InVoiceId, int8 MidiNoteNumber, int8 Velocity, int8 MidiChannel, int32 EventTick, int32 CurrentTick, float MsOffset)
{
	//SynthInstance->program_change(MidiChannel, InVoiceId);
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
	//SynthInstance->program_change(MidiChannel, InVoiceId);
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
	using namespace Harmonix::Midi::Constants;

	switch (InController)
	{
	case EControllerID::BankSelection:
	{
		SynthInstance->program_change(InMidiChannel, InValue);
		break;
	}
	//case EControllerID::Volume:
	//{
	//	SetMidiChannelVolume(InValue);
	//	break;
	//}
	//case EControllerID::Expression:
	//{
	//	SetMidiExpressionGain(InValue);
	//	break;
	//}
	//case EControllerID::PanRight:
	//{
	//	PanSettings.Detail.Pan = InValue;
	//	break;
	//}
	//case EControllerID::Release:
	//{
	//	AdsrVolumeSettings.ReleaseTime = InValue;
	//	AdsrVolumeSettings.BuildReleaseTable();
	//	break;
	//}
	//case EControllerID::Attack:
	//{
	//	AdsrVolumeSettings.AttackTime = InValue;
	//	AdsrVolumeSettings.BuildAttackTable();
	//	break;
	//}
	//case EControllerID::PortamentoSwitch:
	//{
	//	SetIsPortamentoEnabled(InValue > 0.0f);
	//	break;
	//}
	//case EControllerID::PortamentoTime:
	//{
	//	SetPortamentoTime(gPortamentoTimeInterp.EvalClamped(InValue) / 1000);
	//	break;
	//}
	//case EControllerID::FilterFrequency:
	//{
	//	FilterSettings.Freq = gFreqInterp.EvalClamped(InValue);
	//	break;
	//}
	//case EControllerID::FilterQ:
	//{
	//	FilterSettings.Q = gFilterQInterp.EvalClamped(InValue);
	//	break;
	//}
	//case EControllerID::CoarsePitchBend:
	//{
	//	SetPitchBend(InValue);
	//	break;
	//}
	//case EControllerID::SampleStartTime:
	//{
	//	SetStartPointMs(gStartPointTimeInterp.EvalClamped(InValue));
	//	break;
	//}
	//case EControllerID::LFO0Frequency:
	//case EControllerID::LFO1Frequency:
	//{
	//	int32 LfoIdx = (InController == EControllerID::LFO1Frequency);

	//	if (LfoSettings[LfoIdx].Freq != InValue)
	//	{
	//		LfoSettings[LfoIdx].Freq = InValue;
	//		Lfos[LfoIdx].UseSettings(&LfoSettings[LfoIdx]);
	//		UpdateVoiceLfos();
	//	}
	//	break;
	//}
	//case EControllerID::LFO0Depth:
	//case EControllerID::LFO1Depth:
	//{
	//	int32 LfoIdx = (InController == EControllerID::LFO1Depth);
	//	if (LfoSettings[LfoIdx].Depth != InValue)
	//	{
	//		LfoSettings[LfoIdx].Depth = InValue;
	//		Lfos[LfoIdx].UseSettings(&LfoSettings[LfoIdx]);
	//		UpdateVoiceLfos();
	//	}
	//	break;
	//}
	case EControllerID::BitCrushWetMix:
	{
		break;
	}
	case EControllerID::BitCrushLevel:
	{
		break;
	}
	case EControllerID::BitCrushSampleHold:
	{
		break;
	}
	case EControllerID::DelayTime:
	{
		break;
	}
	case EControllerID::DelayDryGain:
	{
		break;
	}
	case EControllerID::DelayWetGain:
	{
		break;
	}
	case EControllerID::DelayFeedback:
	{
		break;
	}
	case EControllerID::DelayEQEnabled:
	{
		break;
	}
	case EControllerID::DelayEQType:
	{
		break;
	}
	case EControllerID::DelayEQFreq:
	{
		break;
	}
	case EControllerID::DelayEQQ:
	{
		break;
	}
	case EControllerID::DelayLFOEnabled:
	{
		break;
	}
	case EControllerID::DelayLFOBeatSync:
	{
		break;
	}
	case EControllerID::DelayLFORate:
	{
		break;
	}
	case EControllerID::DelayLFODepth:
	{
		break;
	}
	case EControllerID::DelayStereoType:
	{
		break;
	}
	case EControllerID::DelayPanLeft:
	{
		break;
	}
	case EControllerID::DelayPanRight:
	{
		break;
	}
	//case EControllerID::TimeStretchEnvelopeOrder:
	//{
	//	TimeStretchEnvelopeOverride = (int16)InValue;
	//	UpdateVoicesForEnvelopeOrderChange();
	//	break;
	//}
	//case EControllerID::SubStreamVol1:
	//{
	//	SetSubstreamMidiGain(0, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol2:
	//{
	//	SetSubstreamMidiGain(1, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol3:
	//{
	//	SetSubstreamMidiGain(2, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol4:
	//{
	//	SetSubstreamMidiGain(3, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol5:
	//{
	//	SetSubstreamMidiGain(4, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol6:
	//{
	//	SetSubstreamMidiGain(5, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol7:
	//{
	//	SetSubstreamMidiGain(6, (int32)InValue);
	//	break;
	//}
	//case EControllerID::SubStreamVol8:
	//{
	//	SetSubstreamMidiGain(7, (int32)InValue);
	//	break;
	//}
	default:
		break;
	}
}
void FSoundfontInstrument::Set14BitControllerImpl(Harmonix::Midi::Constants::EControllerID InController, int16 InValue, int8 InMidiChannel)
{
	using namespace Harmonix::Midi::Constants;

	switch (InController)
	{
	case EControllerID::BankSelection:
	{
		SynthInstance->program_change(InMidiChannel, InValue);
		break;
	}

	}
}
void FSoundfontInstrument::ResetInstrumentStateImpl()
{
}

void FSoundfontInstrument::ResetMidiStateImpl()
{
}