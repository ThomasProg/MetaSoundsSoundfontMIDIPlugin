// Fill out your copyright notice in the Description page of Project Settings.


#include "SynthInstance.h"
#include "SynthSettings.h"

TObjectPtr<USynthInstance> USynthInstance::CreateInstance(TObjectPtr<class USynthSettings> Settings)
{
	TObjectPtr<USynthInstance> Synth = NewObject<USynthInstance>();

	Synth->Instance = new_fluid_synth(Settings->Instance);

	return Synth;
}

USynthInstance::~USynthInstance()
{
	if (Instance != nullptr)
	{
		delete_fluid_synth(Instance);
	}
}


void USynthInstance::noteon(int32 chan, int32 key, int32 vel)
{
	int32 result = fluid_synth_noteon(Instance, chan, key, vel);
	verifyf(result == FLUID_OK, TEXT("noteon: Failed"));
}

void USynthInstance::noteoff(int32 chan, int32 key)
{
	int32 result = fluid_synth_noteoff(Instance, chan, key);
	verifyf(result == FLUID_OK, TEXT("noteoff: Failed"));
}

void USynthInstance::cc(int32 chan, int32 ctrl, int32 val)
{
	int32 result = fluid_synth_cc(Instance, chan, ctrl, val);
	verifyf(result == FLUID_OK, TEXT("cc: Failed"));
}

void USynthInstance::get_cc(int32 chan, int32 ctrl, int32& pval)
{
	int32 result = fluid_synth_get_cc(Instance, chan, ctrl, &pval);
	verifyf(result == FLUID_OK, TEXT("get_cc: Failed"));
}

//void USynthInstance::sysex(const char* data, int32 len, char* response, int* response_len, int* handled, int32 dryrun)
//{
//
//}

void USynthInstance::pitch_bend(int32 chan, int32 val)
{
	int32 result = fluid_synth_pitch_bend(Instance, chan, val);
	verifyf(result == FLUID_OK, TEXT("pitch_bend: Failed"));
}

void USynthInstance::get_pitch_bend(int32 chan, int32& ppitch_bend)
{
	int32 result = fluid_synth_get_pitch_bend(Instance, chan, &ppitch_bend);
	verifyf(result == FLUID_OK, TEXT("get_pitch_bend: Failed"));
}

void USynthInstance::pitch_wheel_sens(int32 chan, int32 val)
{
	int32 result = fluid_synth_pitch_wheel_sens(Instance, chan, val);
	verifyf(result == FLUID_OK, TEXT("pitch_wheel_sens: Failed"));
}

void USynthInstance::get_pitch_wheel_sens(int32 chan, int32& pval)
{
	int32 result = fluid_synth_get_pitch_wheel_sens(Instance, chan, &pval);
	verifyf(result == FLUID_OK, TEXT("get_pitch_wheel_sens: Failed"));
}

void USynthInstance::program_change(int32 chan, int32 program)
{
	int32 result = fluid_synth_program_change(Instance, chan, program);
	verifyf(result == FLUID_OK, TEXT("program_change: Failed"));
}

void USynthInstance::channel_pressure(int32 chan, int32 val)
{
	int32 result = fluid_synth_channel_pressure(Instance, chan, val);
	verifyf(result == FLUID_OK, TEXT("channel_pressure: Failed"));
}

void USynthInstance::key_pressure(int32 chan, int32 key, int32 val)
{
	int32 result = fluid_synth_key_pressure(Instance, chan, key, val);
	verifyf(result == FLUID_OK, TEXT("key_pressure: Failed"));
}

void USynthInstance::bank_select(int32 chan, int32 bank)
{
	int32 result = fluid_synth_bank_select(Instance, chan, bank);
	verifyf(result == FLUID_OK, TEXT("bank_select: Failed"));
}

void USynthInstance::sfont_select(int32 chan, int32 sfont_id)
{
	int32 result = fluid_synth_sfont_select(Instance, chan, sfont_id);
	verifyf(result == FLUID_OK, TEXT("sfont_select: Failed"));
}

void USynthInstance::program_select(int32 chan, int32 sfont_id, int32 bank_num, int32 preset_num)
{
	int32 result = fluid_synth_program_select(Instance, chan, sfont_id, bank_num, preset_num);
	verifyf(result == FLUID_OK, TEXT("program_select: Failed"));
}

void USynthInstance::program_select_by_sfont_name(int32 chan, const FString& sfont_name, int32 bank_num, int32 preset_num)
{
	int32 result = fluid_synth_program_select_by_sfont_name(Instance, chan, TCHAR_TO_UTF8(*sfont_name), bank_num, preset_num);
	verifyf(result == FLUID_OK, TEXT("program_select_by_sfont_name: Failed"));
}

void USynthInstance::get_program(int32 chan, int32& sfont_id, int32& bank_num, int32& preset_num)
{
	int32 result = fluid_synth_get_program(Instance, chan, &sfont_id, &bank_num, &preset_num);
	verifyf(result == FLUID_OK, TEXT("get_program: Failed"));
}

void USynthInstance::unset_program(int32 chan)
{
	int32 result = fluid_synth_unset_program(Instance, chan);
	verifyf(result == FLUID_OK, TEXT("unset_program: Failed"));
}

void USynthInstance::program_reset()
{
	int32 result = fluid_synth_program_reset(Instance);
	verifyf(result == FLUID_OK, TEXT("program_reset: Failed"));
}

void USynthInstance::system_reset()
{
	int32 result = fluid_synth_system_reset(Instance);
	verifyf(result == FLUID_OK, TEXT("system_reset: Failed"));
}


void USynthInstance::all_notes_off(int32 chan)
{
	int32 result = fluid_synth_all_notes_off(Instance, chan);
	verifyf(result == FLUID_OK, TEXT("all_notes_off: Failed"));
}

void USynthInstance::all_sounds_off(int32 chan)
{
	int32 result = fluid_synth_all_sounds_off(Instance, chan);
	verifyf(result == FLUID_OK, TEXT("all_sounds_off: Failed"));
}


void USynthInstance::set_gen(int32 chan, int32 param, float value)
{
	int32 result = fluid_synth_set_gen(Instance, chan, param, value);
	verifyf(result == FLUID_OK, TEXT("set_gen: Failed"));
}

float USynthInstance::get_gen(int32 chan, int32 param)
{
	return fluid_synth_get_gen(Instance, chan, param);
}







void USynthInstance::sfload(const FString& filename, int32 reset_presets)
{
	int32 result = fluid_synth_sfload(Instance, TCHAR_TO_UTF8(*filename), reset_presets);
	verifyf(result == FLUID_OK, TEXT("sfload: Failed"));
}
void USynthInstance::sfreload(int32 id)
{
	int32 result = fluid_synth_sfreload(Instance, id);
	verifyf(result == FLUID_OK, TEXT("sfreload: Failed"));
}
void USynthInstance::sfunload(int32 id, int32 reset_presets)
{
	int32 result = fluid_synth_sfunload(Instance, id, reset_presets);
	verifyf(result == FLUID_OK, TEXT("sfunload: Failed"));
}
//void USynthInstance::add_sfont(fluid_sfont_t* sfont)
//{
//	int32 result = fluid_synth_add_sfont(Instance, sfont);
//	verifyf(result == FLUID_OK, TEXT("add_sfont: Failed"));
//}
//void USynthInstance::remove_sfont(fluid_sfont_t* sfont)
//{
//	int32 result = fluid_synth_remove_sfont(Instance, sfont);
//	verifyf(result == FLUID_OK, TEXT("remove_sfont: Failed"));
//}
void USynthInstance::sfcount()
{
	int32 result = fluid_synth_sfcount(Instance);
	verifyf(result == FLUID_OK, TEXT("sfcount: Failed"));
}
//fluid_sfont_t* USynthInstance::get_sfont(uint32 num)
//{
//	fluid_sfont_t* result = fluid_synth_get_sfont(Instance, num);
//	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
//}
//fluid_sfont_t* USynthInstance::get_sfont_by_id(int32 id)
//{
//	fluid_sfont_t* result = fluid_synth_get_sfont_by_id(Instance, id);
//	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
//}
//fluid_sfont_t* USynthInstance::get_sfont_by_name(const char* name)
//{
//	fluid_sfont_t* result = fluid_synth_get_sfont_by_name(Instance, name);
//	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
//}
void USynthInstance::set_bank_offset(int32 sfont_id, int32 offset)
{
	int32 result = fluid_synth_set_bank_offset(Instance, sfont_id, offset);
	verifyf(result == FLUID_OK, TEXT("fluid_synth_set_gen: Failed"));
}
void USynthInstance::get_bank_offset(int32 sfont_id)
{
	int32 result = fluid_synth_get_bank_offset(Instance, sfont_id);
	verifyf(result == FLUID_OK, TEXT("set_gen: Failed"));
}