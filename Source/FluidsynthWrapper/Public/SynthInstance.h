// Copyright Prog'z. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <fluidsynth.h>
#include "Misc/SpinLock.h"
#include "SynthInstance.generated.h"

class FSynthInstanceProxy;

using FSynthInstanceProxyPtr = TSharedPtr<FSynthInstanceProxy, ESPMode::ThreadSafe>;
class FSynthInstanceProxy final : public Audio::TProxyData<FSynthInstanceProxy>
{
public:
	IMPL_AUDIOPROXY_CLASS(FSynthInstanceProxy);

	FLUIDSYNTHWRAPPER_API explicit FSynthInstanceProxy(USynthInstance* InSynthInstance);

	FSynthInstanceProxy(const FSynthInstanceProxy& Other) = default;

	virtual ~FSynthInstanceProxy() override {}

	USynthInstance* SynthInstance = nullptr;
};

/**
 * 
 */
UCLASS(BlueprintType)
class FLUIDSYNTHWRAPPER_API USynthInstance : public UObject, public IAudioProxyDataFactory
{
	GENERATED_BODY()
	
public:
	fluid_synth_t* Instance = nullptr;
	UE::FSpinLock FluidsynthLock;
	//FCriticalSection FluidsynthLock;

	// Key, channel
	TSet<FIntVector2> onNotes;


public:
	static TObjectPtr< USynthInstance> CreateInstance(TObjectPtr<class USynthSettings> Settings);
	virtual void BeginDestroy() override;


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
	int32 sfload(const FString& filename, int32 reset_presets);
	UFUNCTION(BlueprintCallable)
	int32 sfreload(int32 id);
	UFUNCTION(BlueprintCallable)
	void sfunload(int32 id, int32 reset_presets);
	//UFUNCTION(BlueprintCallable)
	//void add_sfont(fluid_sfont_t *sfont);
	//UFUNCTION(BlueprintCallable)
	//void remove_sfont(fluid_sfont_t *sfont);
	UFUNCTION(BlueprintCallable)
	int32 sfcount();
	//UFUNCTION(BlueprintCallable)
	//fluid_sfont_t* get_sfont(uint32 num);
	//UFUNCTION(BlueprintCallable)
	//fluid_sfont_t* get_sfont_by_id(int32 id);
	//UFUNCTION(BlueprintCallable)
	//fluid_sfont_t* get_sfont_by_name(const char *name);
	UFUNCTION(BlueprintCallable)
	void set_bank_offset(int32 sfont_id, int32 offset);
	UFUNCTION(BlueprintCallable)
	int32 get_bank_offset(int32 sfont_id);
	/** @} Soundfont Management */




	/**
	 * @defgroup synthesis_params Synthesis Parameters
	 * @ingroup synth
	 *
	 * Functions to control and query synthesis parameters like gain and
	 * polyphony count.
	 *
	 * @{
	 */
	int count_midi_channels();
	int count_audio_channels();
	int count_audio_groups();
	int count_effects_channels();
	int count_effects_groups();

	void set_gain(float gain);
	float get_gain();
	int set_polyphony(int polyphony);
	int get_polyphony();
	int get_active_voice_count();
	int get_internal_bufsize();

	///**
	// * Synthesis interpolation method.
	// */
	//enum class fluid_interp
	//{
	//	FLUID_INTERP_NONE = 0,        /**< No interpolation: Fastest, but questionable audio quality */
	//	FLUID_INTERP_LINEAR = 1,      /**< Straight-line interpolation: A bit slower, reasonable audio quality */
	//	FLUID_INTERP_4THORDER = 4,    /**< Fourth-order interpolation, good quality, the default */
	//	FLUID_INTERP_7THORDER = 7,    /**< Seventh-order interpolation */

	//	FLUID_INTERP_DEFAULT = FLUID_INTERP_4THORDER, /**< Default interpolation method */
	//	FLUID_INTERP_HIGHEST = FLUID_INTERP_7THORDER, /**< Highest interpolation method */
	//};

	//int fluid_synth_set_interp_method(fluid_synth_t *synth, int chan, fluid_interp interp_method);

	///**
	// * Enum used with fluid_synth_add_default_mod() to specify how to handle duplicate modulators.
	// */
	//enum class fluid_synth_add_mod
	//{
	//	FLUID_SYNTH_OVERWRITE,        /**< Overwrite any existing matching modulator */
	//	FLUID_SYNTH_ADD,              /**< Sum up modulator amounts */
	//};

	//int fluid_synth_add_default_mod(fluid_synth_t *synth, const fluid_mod_t *mod, fluid_synth_add_mod mode);
	//int fluid_synth_remove_default_mod(fluid_synth_t *synth, const fluid_mod_t *mod);
	/** @} Synthesis Parameters */














	//UFUNCTION(BlueprintCallable)
	void process(int32 len, int32 nfx, float* fx[], int32 nout, float* out[]);

	void fluid_synth_write_float(
		int 	len,
		void*	lout,
		int 	loff,
		int 	lincr,
		void*	rout,
		int 	roff,
		int 	rincr
	);

	friend class USoundfontSubsystem;


	//~Begin IAudioProxyDataFactory Interface.
	virtual TSharedPtr<Audio::IProxyData> CreateProxyData(const Audio::FProxyDataInitParams& InitParams) override;
	//~ End IAudioProxyDataFactory Interface.
};

