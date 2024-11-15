// Copyright Epic Games, Inc. All Rights Reserved.

#include "Metasound/MetasoundSoundfontMIDIPlayerNode.h"
#include "HarmonixMetasound/DataTypes/MidiStream.h"
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

#include "MetasoundExecutableOperator.h"     // TExecutableOperator class
#include "MetasoundPrimitives.h"             // ReadRef and WriteRef descriptions for bool, int32, float, and string
#include "MetasoundNodeRegistrationMacro.h"  // METASOUND_LOCTEXT and METASOUND_REGISTER_NODE macros
#include "MetasoundStandardNodesNames.h"     // StandardNodes namespace
#include "MetasoundFacade.h"				         // FNodeFacade class, eliminates the need for a fair amount of boilerplate code
#include "MetasoundParamHelper.h"            // METASOUND_PARAM and METASOUND_GET_PARAM family of macros

#include "SoundfontInstrument.h"

// Required for ensuring the node is supported by all languages in engine. Must be unique per MetaSound.
#define LOCTEXT_NAMESPACE "MetasoundStandardNodes_MetaSoundSoundfontMIDIStreamPlayerNode"

namespace Metasound
{
	// Vertex Names - define your node's inputs and outputs here
	namespace SoundfontMIDIStreamPlayerNodeNames
	{
		METASOUND_PARAM(InputMidiStream, "MidiStream", "MidiStream that is synthesized.");
		METASOUND_PARAM(InputSynthInstance, "SynthInstance", "The synth synthesizing the midi stream");

		METASOUND_PARAM(OutputAudioLeft, "OutputAudioLeft", "The synthesized left audio channel.");
		METASOUND_PARAM(OutputAudioRight, "OutputAudioRight", "The synthesized right audio channel.");
	}


	// Operator Class - defines the way your node is described, created and executed
	class FSoundfontMIDIStreamPlayerOperator : public TExecutableOperator<FSoundfontMIDIStreamPlayerOperator>, public FSoundfontInstrument
	{
	public:
		// Constructor
		FSoundfontMIDIStreamPlayerOperator(const Metasound::FBuildOperatorParams& InParams,
			const HarmonixMetasound::FMidiStreamReadRef& InMidiStream,
			Metasound::FSynthInstanceReadRef InSynthInstance,
			const FTriggerReadRef& InPlayReadRef)
			: Inputs{ InMidiStream, InSynthInstance, InPlayReadRef }
			, Outputs{ TDataWriteReferenceFactory<FAudioBuffer>::CreateAny(InParams.OperatorSettings), TDataWriteReferenceFactory<FAudioBuffer>::CreateAny(InParams.OperatorSettings) }
			//: InputA(InAValue)
			//, InputB(InBValue)
			//, SoundfontMIDIStreamPlayerNodeOutput(FFloatWriteRef::CreateNew(*InputA + *InputB))
		{
			Reset(InParams);
			Init();
		}

		// Helper function for constructing vertex interface
		static const FVertexInterface& DeclareVertexInterface()
		{
			using namespace SoundfontMIDIStreamPlayerNodeNames;

			static const FVertexInterface Interface(
				FInputVertexInterface(
					TInputDataVertex<HarmonixMetasound::FMidiStream>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputMidiStream)),
					TInputDataVertex<Metasound::FSynthInstance>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSynthInstance)),
					TInputDataVertex<FTrigger>("Play", FDataVertexMetadata{ LOCTEXT("MetaSoundSoundfontPlayerNode_InputPlayDesc", "Plays the given note") })
				),
				FOutputVertexInterface(
					TOutputDataVertex<Metasound::FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputAudioLeft)),
					TOutputDataVertex<Metasound::FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputAudioRight))
				)
			);

			return Interface;
		}

		// Retrieves necessary metadata about your node
		static const FNodeClassMetadata& GetNodeInfo()
		{
			auto CreateNodeClassMetadata = []() -> FNodeClassMetadata
				{
					FVertexInterface NodeInterface = DeclareVertexInterface();

					FNodeClassMetadata Metadata
					{
						FNodeClassName { StandardNodes::Namespace, "SoundfontMIDIStreamPlayer Node", StandardNodes::AudioVariant },
						1, // Major Version
						0, // Minor Version
						METASOUND_LOCTEXT("SoundfontMIDIStreamPlayerNodeDisplayName", "SoundfontMIDIStreamPlayer Node"),
						METASOUND_LOCTEXT("SoundfontMIDIStreamPlayerNodeDesc", "A simple node to demonstrate how to create new MetaSound nodes in C++. Adds two floats together"),
						PluginAuthor,
						PluginNodeMissingPrompt,
						NodeInterface,
						{ }, // Category Hierarchy 
						{ }, // Keywords for searching
						FNodeDisplayStyle{}
					};

					return Metadata;
				};

			static const FNodeClassMetadata Metadata = CreateNodeClassMetadata();
			return Metadata;
		}

		struct FInputs
		{
			//Metasound::FBoolReadRef Enabled;
			HarmonixMetasound::FMidiStreamReadRef MidiStream;
			Metasound::FSynthInstanceReadRef SynthInstance;

			FTriggerReadRef PlayReadRef;
		};
		
		struct FOutputs
		{
			Metasound::FAudioBufferWriteRef AudioLeft;
			Metasound::FAudioBufferWriteRef AudioRight;
		};


		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override
		{
			InOutVertexData.BindReadVertex("MidiStream", Inputs.MidiStream);
			InOutVertexData.BindReadVertex("SynthInstance", Inputs.SynthInstance);
			InOutVertexData.BindReadVertex("Play", Inputs.PlayReadRef);
		}

		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override
		{
			InOutVertexData.BindWriteVertex("OutputAudioLeft", TDataWriteReference<FAudioBuffer>(Outputs.AudioLeft));
			InOutVertexData.BindWriteVertex("OutputAudioRight", TDataWriteReference<FAudioBuffer>(Outputs.AudioRight));
		}



		// Allows MetaSound graph to interact with your node's inputs
		virtual FDataReferenceCollection GetInputs() const override
		{
			using namespace SoundfontMIDIStreamPlayerNodeNames;

			FDataReferenceCollection InputDataReferences;

			InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputMidiStream), Inputs.MidiStream);
			InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSynthInstance), Inputs.SynthInstance);

			return InputDataReferences;
		}

		// Allows MetaSound graph to interact with your node's outputs
		virtual FDataReferenceCollection GetOutputs() const override
		{
			using namespace SoundfontMIDIStreamPlayerNodeNames;

			FDataReferenceCollection OutputDataReferences;

			//OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputAudioLeft), SoundfontMIDIStreamPlayerNodeOutput);
			OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputAudioLeft), Outputs.AudioLeft);
			OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputAudioRight), Outputs.AudioRight);

			return OutputDataReferences;
		}

		static TUniquePtr<Metasound::IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults)
		{
			const FOperatorSettings& Settings = InParams.OperatorSettings;
			const FInputVertexInterfaceData& InputData = InParams.InputData;
			HarmonixMetasound::FMidiStreamReadRef InMidiStream = InputData.GetOrConstructDataReadReference<HarmonixMetasound::FMidiStream>("MidiStream");
			FSynthInstanceReadRef InSynthInstance = InputData.GetOrConstructDataReadReference<FSynthInstance>("SynthInstance");
			FTriggerReadRef InPlayReadRef = InputData.GetOrConstructDataReadReference<FTrigger>( "Play", InParams.OperatorSettings);
			return MakeUnique<FSoundfontMIDIStreamPlayerOperator>(InParams, InMidiStream, InSynthInstance, InPlayReadRef);
		}

		void Init()
		{
			SetTicksPerQuarterNote(Inputs.MidiStream->GetTicksPerQuarterNote());
			//SetPatchAndOverrides(PatchInPin->GetRenderable());
			//SetRawTransposition(*TranspositionInPin);
		}

		void Reset(const FResetParams& Params)
		{
			//SyncLinkOutPin->Reset();

			//Prepare(Params.OperatorSettings.GetSampleRate(), OutputChannelLayout, AudioRendering::kFramesPerRenderBuffer, true);
			BusBuffer.SetNumValidFrames(0);
			//ResidualBuffer.SetNumValidFrames(0);
			FramesPerBlock = Params.OperatorSettings.GetNumFramesPerBlock();
			ResetInstrumentState();
			SetSampleRate(Params.OperatorSettings.GetSampleRate());

			//SetVoicePool(FFusionVoicePool::GetDefault(Params.OperatorSettings.GetSampleRate()), false);
		}

		virtual void Process(uint32 InSliceIndex, uint32 InSubsliceIndex, TAudioBuffer<float>& OutBuffer) override
		{

		}

public:
		// Primary node functionality
		void Execute()
		{
			//ZeroOutput();

			// Create a synth if not
			if (Inputs.SynthInstance.Get() != nullptr && Inputs.SynthInstance->GetSynthInstanceProxy() && Inputs.SynthInstance->GetSynthInstanceProxy()->SynthInstance)
			{
				SynthInstance = Inputs.SynthInstance->GetSynthInstanceProxy()->SynthInstance;
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






			int32 FramesRequired = FramesPerBlock;
			int32 CurrentBlockFrameIndex = 0;

			// create an iterator for midi events in the block
			const TArray<HarmonixMetasound::FMidiStreamEvent>& MidiEvents = Inputs.MidiStream->GetEventsInBlock();
			auto MidiEventIterator = MidiEvents.begin();

			// create an iterator for the midi clock 
			//const TSharedPtr<const FMidiClock, ESPMode::NotThreadSafe> MidiClock = MidiStreamInPin->GetClock();

			while (MidiEventIterator != MidiEvents.end())
			{
				//if ((*MidiEventIterator).BlockSampleFrameIndex <= CurrentBlockFrameIndex)
				//{
					const FMidiMsg& MidiMessage = (*MidiEventIterator).MidiMessage;
					if (MidiMessage.IsStd())// && (*MidiEventIterator).TrackIndex == CurrentTrackNumber)
					{
						HandleMidiMessage(
							(*MidiEventIterator).GetVoiceId(),
							MidiMessage.GetStdStatus(),
							MidiMessage.GetStdData1(),
							MidiMessage.GetStdData2(),
							(*MidiEventIterator).AuthoredMidiTick,
							(*MidiEventIterator).CurrentMidiTick,
							0.0f);
					}
					else if (MidiMessage.IsAllNotesOff())
					{
						AllNotesOff();
					}
					else if (MidiMessage.IsAllNotesKill())
					{
						KillAllVoices();
					}
					++MidiEventIterator;
				//}
				//else
				//{
				//	break;
				//}
			}


			Inputs.PlayReadRef->ExecuteBlock(
				[&](int32 StartFrame, int32 EndFrame)
				{


				},
				[this](int32 StartFrame, int32 EndFrame)
				{
					SynthInstance->noteon(0, 60, 100);
				});

			//StopReadRef->ExecuteBlock(
			//	[&](int32 StartFrame, int32 EndFrame)
			//	{


			//	},
			//	[this](int32 StartFrame, int32 EndFrame)
			//	{
			//		if (SynthInstance.IsValid())
			//		{
			//			SynthInstance->noteoff(*ChannelReadRef, *KeyReadRef);
			//		}
			//	});





			// Synthesize from fluidsynth to the output
			float* LAudio = Outputs.AudioLeft->GetData();
			float* RAudio = Outputs.AudioRight->GetData();

			const int32 NumSamples = Outputs.AudioLeft->Num();

			memset(LAudio, 0, NumSamples * sizeof(float));
			memset(RAudio, 0, NumSamples * sizeof(float));

			float* arrays[] = { LAudio, RAudio };

			if (SynthInstance.IsValid())
			{
				SynthInstance->process(NumSamples, 0, NULL, 2, arrays);
			}



		}

		///**********************************************************************************************************
		//***********************************************************************************************************
		//* FFusionSamplerOperatorBase::Execute()
		//***********************************************************************************************************
		//*********************************************************************************************************/
		//void Execute()
		//{
		//	StuckNoteGuard.UnstickNotes(*MidiStreamInPin, [this](const FMidiStreamEvent& Event)
		//		{
		//			NoteOff(Event.GetVoiceId(), Event.MidiMessage.GetStdData1(), Event.MidiMessage.GetStdChannel());
		//		});

		//	if (EnableMTRenderingInPin)
		//	{
		//		if (!SyncLinkOutPin->GetTask().IsCompleted())
		//		{
		//			UE_LOG(LogFusionSamplerPlayer, Warning, TEXT("FusionSamplerOperator is running multi-threaded, and the Execute method was called again before the last render was complete!"));
		//		}
		//		SyncLinkOutPin->Reset();
		//	}

		//	if (!*EnableInPin)
		//	{
		//		if (MadeAudioLastFrame)
		//		{
		//			KillAllVoices();
		//			MadeAudioLastFrame = false;
		//		}
		//		ZeroOutput();
		//		return;
		//	}

		//	int32 TrackIndex = *TrackNumberInPin;
		//	if (TrackIndex != CurrentTrackNumber)
		//	{
		//		AllNotesOff();
		//		CurrentTrackNumber = TrackIndex;
		//	}

		//	CheckForUpdatedFusionPatchData();
		//	UpdatePatchOverrides();

		//	SetRawTransposition(*TranspositionInPin);

		//	if (EnableMTRenderingInPin && !bDisableMultithreadedRender)
		//	{
		//		SyncLinkOutPin->KickAsyncRender([this]()
		//			{
		//				TRACE_CPUPROFILER_EVENT_SCOPE_STR("FusionAsyncRender");
		//				DoRender();
		//			});
		//	}
		//	else
		//	{
		//		DoRender();
		//	}
		//}

		//void DoRender()
		//{
		//	int32 FramesRequired = FramesPerBlock;
		//	int32 CurrentBlockFrameIndex = 0;

		//	MadeAudioLastFrame = false;

		//	if (ResidualBuffer.GetNumValidFrames() > 0)
		//	{
		//		// we have some samples from the last render we need to use...
		//		int32 NumFramesCopied = CopyFramesToOutput(CurrentBlockFrameIndex, ResidualBuffer, FramesRequired);
		//		ResidualBuffer.AdvanceAliasedDataPointers(NumFramesCopied);

		//		FramesRequired -= NumFramesCopied;
		//		CurrentBlockFrameIndex += NumFramesCopied;
		//		MadeAudioLastFrame = true;
		//	}

		//	TAudioBuffer<float> AliasedOutputBuffer;
		//	SetupOutputAlias(AliasedOutputBuffer, CurrentBlockFrameIndex);

		//	// create an iterator for midi events in the block
		//	const TArray<FMidiStreamEvent>& MidiEvents = MidiStreamInPin->GetEventsInBlock();
		//	auto MidiEventIterator = MidiEvents.begin();

		//	// create an iterator for the midi clock 
		//	const TSharedPtr<const FMidiClock, ESPMode::NotThreadSafe> MidiClock = MidiStreamInPin->GetClock();

		//	while (FramesRequired > 0)
		//	{
		//		while (MidiEventIterator != MidiEvents.end())
		//		{
		//			if ((*MidiEventIterator).BlockSampleFrameIndex <= CurrentBlockFrameIndex)
		//			{
		//				const FMidiMsg& MidiMessage = (*MidiEventIterator).MidiMessage;
		//				if (MidiMessage.IsStd() && (*MidiEventIterator).TrackIndex == CurrentTrackNumber)
		//				{
		//					HandleMidiMessage(
		//						(*MidiEventIterator).GetVoiceId(),
		//						MidiMessage.GetStdStatus(),
		//						MidiMessage.GetStdData1(),
		//						MidiMessage.GetStdData2(),
		//						(*MidiEventIterator).AuthoredMidiTick,
		//						(*MidiEventIterator).CurrentMidiTick,
		//						0.0f);
		//				}
		//				else if (MidiMessage.IsAllNotesOff())
		//				{
		//					AllNotesOff();
		//				}
		//				else if (MidiMessage.IsAllNotesKill())
		//				{
		//					KillAllVoices();
		//				}
		//				++MidiEventIterator;
		//			}
		//			else
		//			{
		//				break;
		//			}
		//		}

		//		if (MidiClock.IsValid())
		//		{
		//			const float ClockSpeed = MidiClock->GetSpeedAtBlockSampleFrame(CurrentBlockFrameIndex);
		//			SetSpeed(ClockSpeed, !(*ClockSpeedAffectsPitchInPin));
		//			const float ClockTempo = MidiClock->GetTempoAtBlockSampleFrame(CurrentBlockFrameIndex);
		//			SetTempo(ClockTempo);
		//			const float Beat = MidiClock->GetQuarterNoteIncludingCountIn();
		//			SetBeat(Beat);
		//		}

		//		if (FramesRequired > AudioRendering::kFramesPerRenderBuffer)
		//		{
		//			// we can render this block in place and avoid a copy
		//			Process(SliceIndex++, 0, AliasedOutputBuffer);
		//			MadeAudioLastFrame = MadeAudioLastFrame || !AliasedOutputBuffer.GetIsSilent();
		//			AliasedOutputBuffer.IncrementChannelDataPointers(AudioRendering::kFramesPerRenderBuffer);
		//			FramesRequired -= AudioRendering::kFramesPerRenderBuffer;
		//			CurrentBlockFrameIndex += AudioRendering::kFramesPerRenderBuffer;
		//		}
		//		else
		//		{
		//			// we only have room for a portion of the block we are about to render, so render to our scratch buffer...
		//			BusBuffer.SetNumValidFrames(AudioRendering::kFramesPerRenderBuffer);
		//			Process(SliceIndex++, 0, BusBuffer);
		//			MadeAudioLastFrame = MadeAudioLastFrame || !BusBuffer.GetIsSilent();

		//			int32 CopiedFrames = CopyFramesToOutput(CurrentBlockFrameIndex, BusBuffer, FramesRequired);
		//			// Alias the BusBuffer and initialize it with the offset of the num frames copied
		//			// this allows us to advance through the frames as we copy them from the residual buffer
		//			ResidualBuffer.Alias(BusBuffer, CopiedFrames);

		//			FramesRequired -= CopiedFrames;
		//			CurrentBlockFrameIndex += CopiedFrames;
		//		}
		//	}

		//	// there may be some remaining midi at the end of the block that we need to render next block
		//	// so pass them to the fusion sampler now...
		//	while (MidiEventIterator != MidiEvents.end())
		//	{
		//		const FMidiMsg& MidiMessage = (*MidiEventIterator).MidiMessage;
		//		if (MidiMessage.IsStd() && (*MidiEventIterator).TrackIndex == CurrentTrackNumber)
		//		{
		//			HandleMidiMessage(
		//				(*MidiEventIterator).GetVoiceId(),
		//				MidiMessage.GetStdStatus(),
		//				MidiMessage.GetStdData1(),
		//				MidiMessage.GetStdData2(),
		//				(*MidiEventIterator).AuthoredMidiTick,
		//				(*MidiEventIterator).CurrentMidiTick,
		//				0.0f);
		//		}
		//		++MidiEventIterator;
		//	}
		//}
		//void PostExecute()
		//{
		//	if (EnableMTRenderingInPin)
		//	{
		//		// This will ensure that our deferred renderer HAS, in fact, completed,
		//		// and will clear out the task handle. 
		//		SyncLinkOutPin->PostExecute();
		//	}
		//}


		void ZeroOutput()
		{
			Outputs.AudioLeft->Zero();
			Outputs.AudioRight->Zero();
		}


	private:
		FInputs Inputs;
		FOutputs Outputs;

		//** DATA
		int32 FramesPerBlock = 0;
		int32 CurrentTrackNumber = 0;

		int32 SliceIndex = 0;
		bool MadeAudioLastFrame = false;
	};

	// Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
	class FSoundfontMIDIStreamPlayerNode : public FNodeFacade
	{
	public:
		FSoundfontMIDIStreamPlayerNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FSoundfontMIDIStreamPlayerOperator>())
		{
		}
	};

     




	// Register node
	METASOUND_REGISTER_NODE(FSoundfontMIDIStreamPlayerNode);
}
















#undef LOCTEXT_NAMESPACE