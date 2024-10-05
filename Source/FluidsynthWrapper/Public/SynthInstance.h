// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <fluidsynth.h>
#include "SynthInstance.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class FLUIDSYNTHWRAPPER_API USynthInstance : public UObject
{
	GENERATED_BODY()
	
	fluid_synth_t* Instance = nullptr;

public:
	static TObjectPtr< USynthInstance> CreateInstance(TObjectPtr<class USynthSettings> Settings);
	~USynthInstance();


	/**
	 * @defgroup midi_messages MIDI Channel Messages
	 * @ingroup synth
	 *
	 * The MIDI channel message functions are mostly directly named after their
	 * counterpart MIDI messages. They are a high-level interface to controlling
	 * the synthesizer, playing notes and changing note and channel parameters.
	 *
	 * @{
	 */
	UFUNCTION(BlueprintCallable)
	void noteon(int32 chan, int32 key, int32 vel);
	UFUNCTION(BlueprintCallable)
	void noteoff(int32 chan, int32 key);
	UFUNCTION(BlueprintCallable)
	void cc(int32 chan, int32 ctrl, int32 val);
	UFUNCTION(BlueprintCallable)
	void get_cc(int32 chan, int32 ctrl, int32& pval);
	//UFUNCTION(BlueprintCallable)
	//void sysex(const char *data, int32 len, char *response, int32 *response_len, int32 *handled, int32 dryrun);
	UFUNCTION(BlueprintCallable)
	void pitch_bend(int32 chan, int32 val);
	UFUNCTION(BlueprintCallable)
	void get_pitch_bend(int32 chan, int32& ppitch_bend);
	UFUNCTION(BlueprintCallable)
	void pitch_wheel_sens(int32 chan, int32 val);
	UFUNCTION(BlueprintCallable)
	void get_pitch_wheel_sens(int32 chan, int32& pval);
	UFUNCTION(BlueprintCallable)
	void program_change(int32 chan, int32 program);
	UFUNCTION(BlueprintCallable)
	void channel_pressure(int32 chan, int32 val);
	UFUNCTION(BlueprintCallable)
	void key_pressure(int32 chan, int32 key, int32 val);
	UFUNCTION(BlueprintCallable)
	void bank_select(int32 chan, int32 bank);
	UFUNCTION(BlueprintCallable)
	void sfont_select(int32 chan, int32 sfont_id);
	UFUNCTION(BlueprintCallable)	
	void program_select(int32 chan, int32 sfont_id,
								   int32 bank_num, int32 preset_num);
	UFUNCTION(BlueprintCallable)
	void program_select_by_sfont_name(int32 chan, const FString& sfont_name, int32 bank_num, int32 preset_num);
	UFUNCTION(BlueprintCallable)	
	void get_program(int32 chan, int32& sfont_id, int32& bank_num, int32& preset_num);
	UFUNCTION(BlueprintCallable)
	void unset_program(int32 chan);
	UFUNCTION(BlueprintCallable)
	void program_reset();
	UFUNCTION(BlueprintCallable)
	void system_reset();

	UFUNCTION(BlueprintCallable)
	void all_notes_off(int32 chan);
	UFUNCTION(BlueprintCallable)
	void all_sounds_off(int32 chan);

	UFUNCTION(BlueprintCallable)
	void set_gen(int32 chan, int32 param, float value);
	UFUNCTION(BlueprintCallable)
	float get_gen(int32 chan, int32 param);
	/** @} MIDI Channel Messages */



	/**
	 * @defgroup soundfont_management SoundFont Management
	 * @ingroup synth
	 *
	 * Functions to load and unload SoundFonts.
	 *
	 * @{
	 */
	UFUNCTION(BlueprintCallable)
	void sfload(const FString& filename, int32 reset_presets);
	UFUNCTION(BlueprintCallable)
	void sfreload(int32 id);
	UFUNCTION(BlueprintCallable)
	void sfunload(int32 id, int32 reset_presets);
	//UFUNCTION(BlueprintCallable)
	//void add_sfont(fluid_sfont_t *sfont);
	//UFUNCTION(BlueprintCallable)
	//void remove_sfont(fluid_sfont_t *sfont);
	UFUNCTION(BlueprintCallable)
	void sfcount();
	//UFUNCTION(BlueprintCallable)
	//fluid_sfont_t* get_sfont(uint32 num);
	//UFUNCTION(BlueprintCallable)
	//fluid_sfont_t* get_sfont_by_id(int32 id);
	//UFUNCTION(BlueprintCallable)
	//fluid_sfont_t* get_sfont_by_name(const char *name);
	UFUNCTION(BlueprintCallable)
	void set_bank_offset(int32 sfont_id, int32 offset);
	UFUNCTION(BlueprintCallable)
	void get_bank_offset(int32 sfont_id);
	/** @} Soundfont Management */



	friend class USoundfontSubsystem;
};
