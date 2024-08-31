// Copyright Epic Games, Inc. All Rights Reserved.

#include "Interfaces/MetasoundFrontendSourceInterface.h"
#include "MetasoundDataFactory.h"
#include "MetasoundEngineNodesNames.h"
#include "MetasoundExecutableOperator.h"
#include "MetasoundFacade.h"
#include "MetasoundNodeRegistrationMacro.h"
#include "MetasoundPrimitives.h"
#include "MetasoundStandardNodesCategories.h"
#include "MetasoundStandardNodesNames.h"
#include "MetasoundTrace.h"
#include "MetasoundTrigger.h"
#include <fluidsynth.h>
#include <DSP/MultichannelBuffer.h>

#define LOCTEXT_NAMESPACE "MetasoundMGF"

using namespace Metasound;

int fluidsynth_callback(void* data, int len,
	int nfx, float* fx[],
	int nout, float* out[]);

class FMetaSoundSoundfontPlayerNodeOperator : public TExecutableOperator<FMetaSoundSoundfontPlayerNodeOperator>
{
public:
	// Maximum absolute pitch shift in semitones. 
	static const FVertexInterface& GetDefaultInterface()
	{
		static const FVertexInterface DefaultInterface(
			FInputVertexInterface(
				TInputDataVertex<FTrigger>("Play", FDataVertexMetadata{ LOCTEXT("MetaSoundSoundfontPlayerNode_InputPlayDesc", "Plays the wavetable") }),
				TInputDataVertex<FTrigger>("Stop", FDataVertexMetadata{ LOCTEXT("MetaSoundSoundfontPlayerNode_InputStopDesc", "Stops the wavetable") }),
				//TInputDataVertex<FWaveTableBankAsset>("Soundfont", FDataVertexMetadata
				//	{
				//		LOCTEXT("MetaSoundSoundfontPlayerNode_InputBankDesc", "WaveTable Bank to playback from"),
				//		LOCTEXT("MetaSoundSoundfontPlayerNode_InputBankName", "Bank")
				//	}),
				TInputDataVertex<int32>("Key", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputKeyDesc", "Key of WaveTable Bank entry to play"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputKeyName", "Key")
					}),
				TInputDataVertex<int32>("Velocity", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputVelocityDesc", "Velocity of WaveTable Bank entry to play"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputVelocityName", "Velocity")
					}),
				TInputDataVertex<int32>("Channel", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputChannelDesc", "Channel of WaveTable Bank entry to play"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputChannelName", "Channel")
					})
			),
			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>("LOut", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundWaveTablePlayer_OutputMonoOutDesc", "L output audio from player"),
						LOCTEXT("MetaSoundWaveTablePlayer_OutputMonoOutName", "LOut")
					}),
				TOutputDataVertex<FAudioBuffer>("ROut", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundWaveTablePlayer_OutputMonoOutDesc", "R output audio from player"),
						LOCTEXT("MetaSoundWaveTablePlayer_OutputMonoOutName", "ROut")
					}),
				TOutputDataVertex<FTrigger>("OnFinished", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_OnFinished", "Trigger executed when player is complete (if not looping) or stop is triggered"),
						LOCTEXT("MetaSoundWaveTablePlayer_OutputOnFinishedName", "On Finished")
					})
			)
		);

		return DefaultInterface;
	}

	static const FNodeClassMetadata& GetNodeInfo()
	{
		auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
		{
			FNodeClassMetadata Metadata
			{
				{ /*EngineNodes::Namespace*/ "MGF", "Player", ""},
				1, // Major Version
				0, // Minor Version
				LOCTEXT("MetaSoundSoundfontPlayerNode_Name", "Soundfont Player"),
				LOCTEXT("MetaSoundSoundfontPlayerNode_Description", "Reads through the given SoundfontBank's entry at the given index."),
				PluginAuthor,
				PluginNodeMissingPrompt,
				GetDefaultInterface(),
				{ NodeCategories::WaveTables },
				{ NodeCategories::Generators, METASOUND_LOCTEXT("WaveTablePlayerSynthesisKeyword", "Synthesis")},
				{ }
			};

			return Metadata;
		};

		static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
		return Metadata;
	}

	static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, TArray<TUniquePtr<IOperatorBuildError>>& OutErrors)
	{
		const FInputVertexInterface& InputInterface = InParams.Node.GetVertexInterface().GetInputInterface();
		const FDataReferenceCollection& InputCollection = InParams.InputDataReferences;

		//FWaveTableBankAssetReadRef InWaveTableBankReadRef = InputCollection.GetDataReadReferenceOrConstruct<FWaveTableBankAsset>("WaveTableBank");
		FInt32ReadRef InKeyReadRef = InputCollection.GetDataReadReferenceOrConstruct<int32>("Key");
		FInt32ReadRef InVelocityReadRef = InputCollection.GetDataReadReferenceOrConstruct<int32>("Velocity");
		FInt32ReadRef InChannelReadRef = InputCollection.GetDataReadReferenceOrConstruct<int32>("Channel");
		FTriggerReadRef InPlayReadRef = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FTrigger>(InputInterface, "Play", InParams.OperatorSettings);
		FTriggerReadRef InStopReadRef = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FTrigger>(InputInterface, "Stop", InParams.OperatorSettings);
		//FTriggerReadRef InSyncReadRef = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<FTrigger>(InputInterface, "Sync", InParams.OperatorSettings);
		//FFloatReadRef InPitchShiftReadRef = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<float>(InputInterface, "PitchShift", InParams.OperatorSettings);
		//FBoolReadRef InLoopReadRef = InputCollection.GetDataReadReferenceOrConstructWithVertexDefault<bool>(InputInterface, "Loop", InParams.OperatorSettings);

		//TOptional<FAudioBufferReadRef> InPhaseModReadRef;
		//if (InputCollection.ContainsDataReadReference<FAudioBuffer>("PhaseMod"))
		//{
		//	InPhaseModReadRef = InputCollection.GetDataReadReference<FAudioBuffer>("PhaseMod");
		//}

		return MakeUnique<FMetaSoundSoundfontPlayerNodeOperator>(InParams, /*InWaveTableBankReadRef,*/ InKeyReadRef, InVelocityReadRef, InChannelReadRef, InPlayReadRef, InStopReadRef/*, MoveTemp(InPhaseModReadRef)*/);
	}

	FMetaSoundSoundfontPlayerNodeOperator(
		const FCreateOperatorParams& InParams,
		//const FWaveTableBankAssetReadRef& InWaveTableBankReadRef,
		const FInt32ReadRef& InKeyReadRef,
		const FInt32ReadRef& InVelocityReadRef,
		const FInt32ReadRef& InChannelReadRef,
		const FTriggerReadRef& InPlayReadRef,
		const FTriggerReadRef& InStopReadRef
		//const FTriggerReadRef& InSyncReadRef,
		//const FFloatReadRef& InPitchShiftReadRef,
		//const FBoolReadRef& InLoopReadRef,
		//TOptional<FAudioBufferReadRef>&& InPhaseModReadRef
	)
		: SampleRate(InParams.OperatorSettings.GetSampleRate())
		//, WaveTableBankReadRef(InWaveTableBankReadRef)
		, KeyReadRef(InKeyReadRef)
		, VelocityReadRef(InVelocityReadRef)
		, ChannelReadRef(InChannelReadRef)
		, PlayReadRef(InPlayReadRef)
		, StopReadRef(InStopReadRef)
		//, SyncReadRef(InSyncReadRef)
		//, PitchShiftReadRef(InPitchShiftReadRef)
		//, LoopReadRef(InLoopReadRef)
		//, PhaseModReadRef(MoveTemp(InPhaseModReadRef))
		, OutBufferLWriteRef(TDataWriteReferenceFactory<FAudioBuffer>::CreateAny(InParams.OperatorSettings))
		, OutBufferRWriteRef(TDataWriteReferenceFactory<FAudioBuffer>::CreateAny(InParams.OperatorSettings))
		, OutOnFinishedRef(TDataWriteReferenceFactory<FTrigger>::CreateExplicitArgs(InParams.OperatorSettings))
	{
		const float BlockRate = InParams.OperatorSettings.GetActualBlockRate();
		if (BlockRate > 0.0f)
		{
			BlockPeriod = 1.0f / BlockRate;
		}

		// Create settings and synth
		settings = new_fluid_settings();
		synth = new_fluid_synth(settings);

		// Create an audio driver
		//driverCallback = new_fluid_audio_driver(settings, synth);

		// Load SoundFont
		//fluid_synth_sfload(synth, "C:/Users/thoma/PandorasBox/Projects/ModularMusicGenerationModules/Assets/Soundfonts/Touhou/Touhou.sf2", 1);
		FString RelativePath = FPaths::ProjectContentDir();

		FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
		fluid_synth_sfload(synth, TCHAR_TO_UTF8 (*(FullPath + "/" + "Touhou.sf2")), 1);
	}

	virtual ~FMetaSoundSoundfontPlayerNodeOperator()
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
	}

	virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override
	{
		//InOutVertexData.BindReadVertex("WaveTableBank", WaveTableBankReadRef);
		InOutVertexData.BindReadVertex("Key", KeyReadRef);
		InOutVertexData.BindReadVertex("Velocity", VelocityReadRef);
		InOutVertexData.BindReadVertex("Channel", ChannelReadRef);
		InOutVertexData.BindReadVertex("Play", PlayReadRef);
		InOutVertexData.BindReadVertex("Stop", StopReadRef);
		//InOutVertexData.BindReadVertex("Sync", SyncReadRef);
		//InOutVertexData.BindReadVertex("PitchShift", PitchShiftReadRef);
		//InOutVertexData.BindReadVertex("Loop", LoopReadRef);

		//if (PhaseModReadRef.IsSet())
		//{
		//	InOutVertexData.BindReadVertex("PhaseMod", *PhaseModReadRef);
		//}
	}

	virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override
	{
		InOutVertexData.BindReadVertex("LOut", TDataReadReference<FAudioBuffer>(OutBufferLWriteRef));
		InOutVertexData.BindReadVertex("ROut", TDataReadReference<FAudioBuffer>(OutBufferRWriteRef));
		InOutVertexData.BindReadVertex("OnFinished", TDataReadReference<FTrigger>(OutOnFinishedRef));
	}

	virtual FDataReferenceCollection GetInputs() const override
	{
		// This should never be called. Bind(...) is called instead. This method
		// exists as a stop-gap until the API can be deprecated and removed.
		checkNoEntry();
		return {};
	}

	virtual FDataReferenceCollection GetOutputs() const override
	{
		// This should never be called. Bind(...) is called instead. This method
		// exists as a stop-gap until the API can be deprecated and removed.
		checkNoEntry();
		return {};
	}

	void Execute()
	{
		PlayReadRef->ExecuteBlock(
			[&](int32 StartFrame, int32 EndFrame)
			{


			},
			[this](int32 StartFrame, int32 EndFrame)
			{
				// Start playing a note (adjust the parameters as needed)
				//fluid_synth_program_change(synth, 0, 0);
				fluid_synth_noteon(synth, *ChannelReadRef, *KeyReadRef, *VelocityReadRef);

			});

		StopReadRef->ExecuteBlock(
			[&](int32 StartFrame, int32 EndFrame)
			{


			},
			[this](int32 StartFrame, int32 EndFrame)
			{
				// Start playing a note (adjust the parameters as needed)
				//fluid_synth_program_change(synth, 0, 0);
				//fluid_synth_noteon(synth, *ChannelReadRef, *KeyReadRef, *VelocityReadRef);
				fluid_synth_noteoff(synth, *ChannelReadRef, *KeyReadRef);

			});


		//const float* InputAudio = AudioInput->GetData();
		float* LAudio = OutBufferLWriteRef->GetData();
		float* RAudio = OutBufferRWriteRef->GetData();

		const int32 NumSamples = OutBufferLWriteRef->Num();

		//for (int32 Index = 0; Index < NumSamples; ++Index)
		//{
		//	OutputAudio[Index] = (*Amplitude) * InputAudio[Index];
		//}

		//// Create audio buffer
		//int buffer_size = 1024;
		//int num_channels = 2; // Stereo output
		//float left_buffer[1024];
		//float right_buffer[1024];

		//// Create pointers to the buffers
		//float* left_buffer_ptr = left_buffer;
		//float* right_buffer_ptr = right_buffer;

		memset(LAudio, 0, NumSamples * sizeof(float));
		memset(RAudio, 0, NumSamples * sizeof(float));

		float* arrays[] = {LAudio, RAudio};

		int result = fluid_synth_process(synth, NumSamples, 0, NULL, 2, arrays);

		if (result != FLUID_OK) {
			//fprintf(stderr, "fluid_synth_process() failed\n");
			UE_LOG(LogTemp, Error, TEXT("fluid_synth_process() failed"));
		}

		//for (int i = 0; i < buffer_size; i++)
		//{
		//	float value = left_buffer[i];

		//	LAudio[i] = left_buffer[i];
		//	RAudio[i] = right_buffer[i];

		//	//if (value > 0.01)
		//	//{
		//	//	int v = 2;
		//	//}
		//}
	}


	void Reset(const IOperator::FResetParams& InParams)
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
	}

private:
	float BlockPeriod = 0.0f;
	float SampleRate = 48000.0f;
	bool bPlaying = false;

	//FWaveTableBankAssetReadRef WaveTableBankReadRef;
	FInt32ReadRef KeyReadRef;
	FInt32ReadRef VelocityReadRef;
	FInt32ReadRef ChannelReadRef;
	FTriggerReadRef PlayReadRef;
	FTriggerReadRef StopReadRef;
	//FTriggerReadRef SyncReadRef;
	//FFloatReadRef PitchShiftReadRef;
	//FBoolReadRef LoopReadRef;

	//TOptional<FAudioBufferReadRef> PhaseModReadRef;

	//WaveTable::FWaveTableSampler Sampler;

	TDataWriteReference<FTrigger> OutOnFinishedRef;
	TDataWriteReference<FAudioBuffer> OutBufferLWriteRef;
	TDataWriteReference<FAudioBuffer> OutBufferRWriteRef;


	fluid_settings_t* settings = nullptr;
	fluid_synth_t* synth = nullptr;
	fluid_audio_driver_t* driverCallback = nullptr;

	friend int fluidsynth_callback(void* data, int len,
		int nfx, float* fx[],
		int nout, float* out[]);
};

//int b = 0;

// Callback function to process audio data
int fluidsynth_callback(void* data, int len,
	int nfx, float* fx[],
	int nout, float* out[])
{
	FMetaSoundSoundfontPlayerNodeOperator* d = (FMetaSoundSoundfontPlayerNodeOperator*)data;

	// Thank you a lot https://github.com/ensemblesaw/ensembles-app/blob/b16631a36882b6df9253a77d992eb78e9e0eedd2/src/Core/Synthesizer/providers/synthesizer_instance.c#L95
	// for giving me an example of how new_fluid_audio_driver2 works
	if (fx == 0)
	{
		/* Note that some audio drivers may not provide buffers for effects like
		 * reverb and chorus. In this case it's your decision what to do. If you
		 * had called fluid_synth_process() like in the else branch below, no
		 * effects would have been rendered. Instead, you may mix the effects
		 * directly into the out buffers. */
		if (fluid_synth_process(d->synth, len, nout, out, nout, out) != FLUID_OK)
		{
			/* Some error occurred. Very unlikely to happen, though. */
			return FLUID_FAILED;
		}
	}
	else
	{
		/* Call the synthesizer to fill the output buffers with its
		 * audio output. */
		if (fluid_synth_process(d->synth, len, nfx, fx, nout, out) != FLUID_OK)
		{
			/* Some error occurred. Very unlikely to happen, though. */
			return FLUID_FAILED;
		}
	}


	//TArray<float> t1(out[0], len);
	//TArray<float> t2(out[1], len);

	//for (float f : t2)
	//{
	//	if (f > 0.05)
	//	{
	//		b += 2;
	//	}
	//}

	//int32 bufferLength = d->OutBufferLWriteRef->Num();
	//for (int i = 0; i < FMath::Min(bufferLength, len); i++)
	//{
	//	d->OutBufferLWriteRef->GetData()[i] = t2[i];
	//}

	//FAudioBuffer& OutBuffer = *d->OutBufferLWriteRef;
	//OutBuffer.Zero();

	//auto GetLastIndex = [](const FTriggerReadRef& Trigger)
	//{
	//	int32 LastIndex = -1;
	//	Trigger->ExecuteBlock([](int32, int32) {}, [&LastIndex](int32 StartFrame, int32 EndFrame)
	//		{
	//			LastIndex = FMath::Max(LastIndex, StartFrame);
	//		});
	//	return LastIndex;
	//};

	//const int32 LastPlayIndex = GetLastIndex(d->PlayReadRef);
	//const int32 LastStopIndex = GetLastIndex(d->StopReadRef);
	//if (LastPlayIndex >= 0 || LastStopIndex >= 0)
	//{
	//	d->bPlaying = LastPlayIndex > LastStopIndex;
	//}



	return FLUID_OK;
}

class FMetaSoundSoundfontPlayerNode : public FNodeFacade
{
public:
	FMetaSoundSoundfontPlayerNode(const FNodeInitData& InInitData)
		: FNodeFacade(InInitData.InstanceName, InInitData.InstanceID, TFacadeOperatorClass<FMetaSoundSoundfontPlayerNodeOperator>())
	{
	}

	virtual ~FMetaSoundSoundfontPlayerNode() = default;
};

METASOUND_REGISTER_NODE(FMetaSoundSoundfontPlayerNode)

#undef LOCTEXT_NAMESPACE // MetasoundStandardNodes
