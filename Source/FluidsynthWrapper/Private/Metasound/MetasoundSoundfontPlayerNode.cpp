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
#include "SoundfontSubsystem.h"
#include "Engine/Engine.h"
#include "SynthInstance.h"
#include "MetasoundSynthInstance.h"

#define LOCTEXT_NAMESPACE "MetasoundMGF"

using namespace Metasound;

class FMetaSoundSoundfontPlayerNodeOperator : public TExecutableOperator<FMetaSoundSoundfontPlayerNodeOperator>
{
public:
	// Maximum absolute pitch shift in semitones. 
	static const FVertexInterface& GetDefaultInterface()
	{
		static const FVertexInterface DefaultInterface(
			FInputVertexInterface(
				TInputDataVertex<FTrigger>("Play", FDataVertexMetadata{ LOCTEXT("MetaSoundSoundfontPlayerNode_InputPlayDesc", "Plays the given note") }),
				TInputDataVertex<FTrigger>("Stop", FDataVertexMetadata{ LOCTEXT("MetaSoundSoundfontPlayerNode_InputStopDesc", "Stops any note from playing") }),
				//TInputDataVertex<FWaveTableBankAsset>("Soundfont", FDataVertexMetadata
				//	{
				//		LOCTEXT("MetaSoundSoundfontPlayerNode_InputBankDesc", "WaveTable Bank to playback from"),
				//		LOCTEXT("MetaSoundSoundfontPlayerNode_InputBankName", "Bank")
				//	}),
				TInputDataVertex<int32>("Key", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputKeyDesc", "Pitch of the played note"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputKeyName", "Key")
					}),
				TInputDataVertex<int32>("Velocity", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputVelocityDesc", "Intensity of the played note"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputVelocityName", "Velocity")
					}),
				TInputDataVertex<int32>("Channel", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputChannelDesc", "Channel of the played note"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputChannelName", "Channel")
					}),
				TInputDataVertex<FSynthInstance>("SynthInstance", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputSynthInstanceDesc", "SynthInstance that is playing the note"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_InputSynthInstanceName", "SynthInstance")
					})
			),
			FOutputVertexInterface(
				TOutputDataVertex<FAudioBuffer>("LOut", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_OutputMonoOutDesc", "L output audio from player"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_OutputMonoOutName", "LOut")
					}),
				TOutputDataVertex<FAudioBuffer>("ROut", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_OutputMonoOutDesc", "R output audio from player"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_OutputMonoOutName", "ROut")
					}),
				TOutputDataVertex<FTrigger>("OnFinished", FDataVertexMetadata
					{
						LOCTEXT("MetaSoundSoundfontPlayerNode_OnFinished", "Trigger executed when player is complete (if not looping) or stop is triggered"),
						LOCTEXT("MetaSoundSoundfontPlayerNode_OutputOnFinishedName", "On Finished")
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
		FSynthInstanceReadRef InSynthInstanceReadRef = InputCollection.GetDataReadReferenceOrConstruct<FSynthInstance>("SynthInstance");
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

		return MakeUnique<FMetaSoundSoundfontPlayerNodeOperator>(InParams, /*InWaveTableBankReadRef,*/ InKeyReadRef, InVelocityReadRef, InChannelReadRef, InSynthInstanceReadRef, InPlayReadRef, InStopReadRef/*, MoveTemp(InPhaseModReadRef)*/);
	}

	FMetaSoundSoundfontPlayerNodeOperator(
		const FCreateOperatorParams& InParams,
		//const FWaveTableBankAssetReadRef& InWaveTableBankReadRef,
		const FInt32ReadRef& InKeyReadRef,
		const FInt32ReadRef& InVelocityReadRef,
		const FInt32ReadRef& InChannelReadRef,
		const FSynthInstanceReadRef& InSynthInstanceReadRef,
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
		, SynthInstanceReadRef(InSynthInstanceReadRef)
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
	}

	virtual ~FMetaSoundSoundfontPlayerNodeOperator()
	{
		// Shut down
		//if (SynthInstance)
		//{
		//	for (int32 i = 0; i < SynthInstance->count_midi_channels(); i++)
		//	{
		//		SynthInstance->all_notes_off(i);
		//	}
		//}
	}

	virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override
	{
		InOutVertexData.BindReadVertex("Key", KeyReadRef);
		InOutVertexData.BindReadVertex("Velocity", VelocityReadRef);
		InOutVertexData.BindReadVertex("Channel", ChannelReadRef);
		InOutVertexData.BindReadVertex("SynthInstance", SynthInstanceReadRef);
		InOutVertexData.BindReadVertex("Play", PlayReadRef);
		InOutVertexData.BindReadVertex("Stop", StopReadRef);
	}

	virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override
	{
		InOutVertexData.BindWriteVertex("LOut", TDataWriteReference<FAudioBuffer>(OutBufferLWriteRef));
		InOutVertexData.BindWriteVertex("ROut", TDataWriteReference<FAudioBuffer>(OutBufferRWriteRef));
		InOutVertexData.BindWriteVertex("OnFinished", TDataWriteReference<FTrigger>(OutOnFinishedRef));
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

	TWeakObjectPtr<USynthInstance> SynthInstance = nullptr;
	void Execute()
	{
		if (SynthInstanceReadRef.Get() != nullptr && SynthInstanceReadRef->GetSynthInstanceProxy() && SynthInstanceReadRef->GetSynthInstanceProxy()->SynthInstance)
		{
			SynthInstance = SynthInstanceReadRef->GetSynthInstanceProxy()->SynthInstance;
		}
		else if (SynthInstance == nullptr) // @TODO : optimize for runtime? put in constructor if not bound?
		{
			TObjectPtr<USoundfontSubsystem> SfSubsys = GEngine->GetEngineSubsystem<USoundfontSubsystem>();
			ensure(SfSubsys);
			if (SfSubsys)
			{
				SynthInstance = SfSubsys->CreateNewSynthInstance(NAME_None);
			}
		}


		PlayReadRef->ExecuteBlock(
			[&](int32 StartFrame, int32 EndFrame)
			{


			},
			[this](int32 StartFrame, int32 EndFrame)
			{
				SynthInstance->noteon(*ChannelReadRef, *KeyReadRef, *VelocityReadRef);
			});

		StopReadRef->ExecuteBlock(
			[&](int32 StartFrame, int32 EndFrame)
			{


			},
			[this](int32 StartFrame, int32 EndFrame)
			{
				if (SynthInstance.IsValid())
				{
					SynthInstance->noteoff(*ChannelReadRef, *KeyReadRef);
				}
			});

		


		//const float* InputAudio = AudioInput->GetData();
		float* LAudio = OutBufferLWriteRef->GetData();
		float* RAudio = OutBufferRWriteRef->GetData();

		const int32 NumSamples = OutBufferLWriteRef->Num();

		memset(LAudio, 0, NumSamples * sizeof(float));
		memset(RAudio, 0, NumSamples * sizeof(float));

		float* arrays[] = {LAudio, RAudio};

		if (SynthInstance.IsValid())
		{
			SynthInstance->process(NumSamples, 0, NULL, 2, arrays);
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

	FSynthInstanceReadRef SynthInstanceReadRef;

	//FTriggerReadRef SyncReadRef;
	//FFloatReadRef PitchShiftReadRef;
	//FBoolReadRef LoopReadRef;

	//TOptional<FAudioBufferReadRef> PhaseModReadRef;

	//WaveTable::FWaveTableSampler Sampler;

	TDataWriteReference<FTrigger> OutOnFinishedRef;
	TDataWriteReference<FAudioBuffer> OutBufferLWriteRef;
	TDataWriteReference<FAudioBuffer> OutBufferRWriteRef;

	//USynthInstance* SynthInstance;
};

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
