// Fill out your copyright notice in the Description page of Project Settings.


#include "SynthInstance.h"
#include "SynthSettings.h"

#include "MetasoundDataTypeRegistrationMacro.h"
#include "MetasoundSynthInstance.h"

REGISTER_METASOUND_DATATYPE(Metasound::FSynthInstance, "SynthInstance", Metasound::ELiteralType::UObjectProxy, USynthInstance);

FSynthInstanceProxy::FSynthInstanceProxy(USynthInstance* InSynthInstance)
{
	if (InSynthInstance)
	{
		SynthInstance = InSynthInstance;
	}
}

TObjectPtr<USynthInstance> USynthInstance::CreateInstance(TObjectPtr<USynthSettings> Settings)
{
	TObjectPtr<USynthInstance> Synth = NewObject<USynthInstance>();

	fluid_settings_t* SettingsInstance = new_fluid_settings();

	//double val;
	//fluid_settings_getnum(SettingsInstance, "synth.sample-rate", &val);

	// TODO : set that value to the metasounds node
	fluid_settings_setnum(SettingsInstance, "synth.sample-rate", 48000.0f);

	Synth->Instance = new_fluid_synth(SettingsInstance);

	// @TODO : load default soundfont from settings file

	FString RelativePath = FPaths::ProjectContentDir();

	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);

	//Synth->sfload(*(FullPath + "/soundfonts/" + "Touhou.sf2"), 1);
	Synth->sfload(*(FullPath + "/soundfonts/" + "SGM.sf2"), 1);

	delete_fluid_settings(SettingsInstance);

	return Synth;
}

void USynthInstance::BeginDestroy()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	if (Instance != nullptr)
	{
		delete_fluid_synth(Instance);
		Instance = nullptr;
	}

	Super::BeginDestroy();
}


void USynthInstance::noteon(int32 chan, int32 key, int32 vel)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_noteon(Instance, chan, key, vel);
	verifyf(result == FLUID_OK, TEXT("noteon: Failed"));
	onNotes.Add(FIntVector2(key, chan));
}

void USynthInstance::noteoff(int32 chan, int32 key)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);

	FIntVector2 val = FIntVector2(key, chan);
	if (!onNotes.Contains(val))
		return;

	int32 result = fluid_synth_noteoff(Instance, chan, key);
	//verifyf(result == FLUID_OK, TEXT("noteoff: Failed"));
	onNotes.Remove(val);
}

void USynthInstance::cc(int32 chan, int32 ctrl, int32 val)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_cc(Instance, chan, ctrl, val);
	verifyf(result == FLUID_OK, TEXT("cc: Failed"));
}

void USynthInstance::get_cc(int32 chan, int32 ctrl, int32& pval)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_get_cc(Instance, chan, ctrl, &pval);
	verifyf(result == FLUID_OK, TEXT("get_cc: Failed"));
}

//void USynthInstance::sysex(const char* data, int32 len, char* response, int* response_len, int* handled, int32 dryrun)
//{
//UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
//
//}

void USynthInstance::pitch_bend(int32 chan, int32 val)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_pitch_bend(Instance, chan, val);
	verifyf(result == FLUID_OK, TEXT("pitch_bend: Failed"));
}

void USynthInstance::get_pitch_bend(int32 chan, int32& ppitch_bend)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_get_pitch_bend(Instance, chan, &ppitch_bend);
	verifyf(result == FLUID_OK, TEXT("get_pitch_bend: Failed"));
}

void USynthInstance::pitch_wheel_sens(int32 chan, int32 val)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_pitch_wheel_sens(Instance, chan, val);
	verifyf(result == FLUID_OK, TEXT("pitch_wheel_sens: Failed"));
}

void USynthInstance::get_pitch_wheel_sens(int32 chan, int32& pval)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_get_pitch_wheel_sens(Instance, chan, &pval);
	verifyf(result == FLUID_OK, TEXT("get_pitch_wheel_sens: Failed"));
}

int check_program_exists(fluid_synth_t* synth, int sfont_id, int bank, int program) {
	// Get the soundfont by its ID
	fluid_sfont_t* sfont = fluid_synth_get_sfont(synth, sfont_id);
	if (sfont == NULL) {
		return 0; // SoundFont not found
	}

	// Try to get the preset (program) by bank and program number
	fluid_preset_t* preset = fluid_sfont_get_preset(sfont, bank, program);
	if (preset == NULL) {
		return 0; // Program does not exist
	}

	return 1; // Program exists
}

void USynthInstance::program_change(int32 chan, int32 program)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);

	int exists = check_program_exists(Instance, 0, 0, 1);
	verifyf(exists != 0, TEXT("Program doesn't exist"));

	int32 result = fluid_synth_program_change(Instance, chan, program);
	verifyf(result == FLUID_OK, TEXT("program_change: Failed"));
}

void USynthInstance::channel_pressure(int32 chan, int32 val)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_channel_pressure(Instance, chan, val);
	verifyf(result == FLUID_OK, TEXT("channel_pressure: Failed"));
}

void USynthInstance::key_pressure(int32 chan, int32 key, int32 val)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_key_pressure(Instance, chan, key, val);
	verifyf(result == FLUID_OK, TEXT("key_pressure: Failed"));
}

void USynthInstance::bank_select(int32 chan, int32 bank)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_bank_select(Instance, chan, bank);
	verifyf(result == FLUID_OK, TEXT("bank_select: Failed"));
}

void USynthInstance::sfont_select(int32 chan, int32 sfont_id)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_sfont_select(Instance, chan, sfont_id);
	verifyf(result == FLUID_OK, TEXT("sfont_select: Failed"));
}

void USynthInstance::program_select(int32 chan, int32 sfont_id, int32 bank_num, int32 preset_num)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_program_select(Instance, chan, sfont_id, bank_num, preset_num);
	verifyf(result == FLUID_OK, TEXT("program_select: Failed"));
}

void USynthInstance::program_select_by_sfont_name(int32 chan, const FString& sfont_name, int32 bank_num, int32 preset_num)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_program_select_by_sfont_name(Instance, chan, TCHAR_TO_UTF8(*sfont_name), bank_num, preset_num);
	verifyf(result == FLUID_OK, TEXT("program_select_by_sfont_name: Failed"));
}

void USynthInstance::get_program(int32 chan, int32& sfont_id, int32& bank_num, int32& preset_num)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_get_program(Instance, chan, &sfont_id, &bank_num, &preset_num);
	verifyf(result == FLUID_OK, TEXT("get_program: Failed"));
}

void USynthInstance::unset_program(int32 chan)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_unset_program(Instance, chan);
	verifyf(result == FLUID_OK, TEXT("unset_program: Failed"));
}

void USynthInstance::program_reset()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_program_reset(Instance);
	verifyf(result == FLUID_OK, TEXT("program_reset: Failed"));
}

void USynthInstance::system_reset()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_system_reset(Instance);
	verifyf(result == FLUID_OK, TEXT("system_reset: Failed"));
}


void USynthInstance::all_notes_off(int32 chan)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_all_notes_off(Instance, chan);
	verifyf(result == FLUID_OK, TEXT("all_notes_off: Failed"));
}

void USynthInstance::all_sounds_off(int32 chan)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_all_sounds_off(Instance, chan);
	verifyf(result == FLUID_OK, TEXT("all_sounds_off: Failed"));
}


void USynthInstance::set_gen(int32 chan, int32 param, float value)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_set_gen(Instance, chan, param, value);
	verifyf(result == FLUID_OK, TEXT("set_gen: Failed"));
}

float USynthInstance::get_gen(int32 chan, int32 param)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_get_gen(Instance, chan, param);
}






int32 USynthInstance::sfload(const FString& filename, int32 reset_presets)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_sfload(Instance, TCHAR_TO_UTF8(*filename), reset_presets);
	verifyf(result != FLUID_FAILED, TEXT("sfload: Failed loading %s"), *filename);
	return result;
}
int32 USynthInstance::sfreload(int32 id)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_sfreload(Instance, id);
	verifyf(result != FLUID_FAILED, TEXT("sfreload: Failed"));
	return result;
}
void USynthInstance::sfunload(int32 id, int32 reset_presets)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_sfunload(Instance, id, reset_presets);
	verifyf(result == FLUID_OK, TEXT("sfunload: Failed"));
}
//void USynthInstance::add_sfont(fluid_sfont_t* sfont)
//{
//UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
//	int32 result = fluid_synth_add_sfont(Instance, sfont);
//	verifyf(result == FLUID_OK, TEXT("add_sfont: Failed"));
//}
//void USynthInstance::remove_sfont(fluid_sfont_t* sfont)
//{
//UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
//	int32 result = fluid_synth_remove_sfont(Instance, sfont);
//	verifyf(result == FLUID_OK, TEXT("remove_sfont: Failed"));
//}
int32 USynthInstance::sfcount()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_sfcount(Instance);
}
//fluid_sfont_t* USynthInstance::get_sfont(uint32 num)
//{
//UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
//	fluid_sfont_t* result = fluid_synth_get_sfont(Instance, num);
//	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
//}
//fluid_sfont_t* USynthInstance::get_sfont_by_id(int32 id)
//{
//UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
//	fluid_sfont_t* result = fluid_synth_get_sfont_by_id(Instance, id);
//	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
//}
//fluid_sfont_t* USynthInstance::get_sfont_by_name(const char* name)
//{
//UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
//	fluid_sfont_t* result = fluid_synth_get_sfont_by_name(Instance, name);
//	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
//}
void USynthInstance::set_bank_offset(int32 sfont_id, int32 offset)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_set_bank_offset(Instance, sfont_id, offset);
	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
}
int32 USynthInstance::get_bank_offset(int32 sfont_id)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_get_bank_offset(Instance, sfont_id);
}




void USynthInstance::process(int32 len, int32 nfx, float* fx[], int32 nout, float* out[])
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_process(Instance, len, nfx, fx, nout, out);
	verifyf(result == FLUID_OK, TEXT("set_gen: Failed"));
}



int USynthInstance::count_midi_channels()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_count_midi_channels(Instance);
}
int USynthInstance::count_audio_channels()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_count_audio_channels(Instance);
}
int USynthInstance::count_audio_groups()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_count_audio_groups(Instance);
}
int USynthInstance::count_effects_channels()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_count_effects_channels(Instance);
}
int USynthInstance::count_effects_groups()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_count_effects_groups(Instance);
}

void USynthInstance::set_gain(float gain)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_set_gain(Instance, gain);
}
float USynthInstance::get_gain()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_get_gain(Instance);
}
int USynthInstance::set_polyphony(int polyphony)
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	int32 result = fluid_synth_set_polyphony(Instance, polyphony);
	verifyf(result == FLUID_OK, TEXT("sfunload: Failed"));
	return result;
}
int USynthInstance::get_polyphony()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_get_polyphony(Instance);
}
int USynthInstance::get_active_voice_count()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_get_active_voice_count(Instance);
}
int USynthInstance::get_internal_bufsize()
{
	UE::TScopeLock<UE::FSpinLock> ScopeLock(FluidsynthLock);
	return fluid_synth_get_internal_bufsize(Instance);
}



TSharedPtr<Audio::IProxyData> USynthInstance::CreateProxyData(const Audio::FProxyDataInitParams& InitParams)
{
	return MakeShared<FSynthInstanceProxy, ESPMode::ThreadSafe>(this);;
}
