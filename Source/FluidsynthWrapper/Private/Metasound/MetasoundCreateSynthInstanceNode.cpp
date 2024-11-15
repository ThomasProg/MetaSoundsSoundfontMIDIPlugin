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
	namespace CreateSynthInstanceNodeNames
	{
		METASOUND_PARAM(InSynthInstanceName, "SynthInstanceName", "The name of the synth");
		METASOUND_PARAM(InSoundfontPath, "SoundfontPath", "The path of the soundfont used by the synth\nExample: \"/soundfonts/\" + \"Touhou.sf2\"");

		METASOUND_PARAM(OutSynthInstance, "SynthInstance", "The synth created");
	}


	// Operator Class - defines the way your node is described, created and executed
	class FCreateSynthInstanceOperator : public TExecutableOperator<FCreateSynthInstanceOperator>
	{
	public:
		// Constructor
		FCreateSynthInstanceOperator(const Metasound::FBuildOperatorParams& InParams,
			FStringReadRef SynthInstanceName,
			FStringReadRef SoundfontPath)
			:  Inputs{ SynthInstanceName, SoundfontPath }
			, Outputs{ TDataWriteReferenceFactory<FSynthInstance>::CreateAny(InParams.OperatorSettings) }
		{

		}

		// Helper function for constructing vertex interface
		static const FVertexInterface& DeclareVertexInterface()
		{
			using namespace CreateSynthInstanceNodeNames;

			static const FVertexInterface Interface(
				FInputVertexInterface(
					TInputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(InSynthInstanceName)),
					TInputDataVertex<FString>(METASOUND_GET_PARAM_NAME_AND_METADATA(InSoundfontPath))
					//TInputDataVertex<HarmonixMetasound::FMidiStream>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputMidiStream)),
					//TInputDataVertex<Metasound::FSynthInstance>(METASOUND_GET_PARAM_NAME_AND_METADATA(InputSynthInstance)),
					//TInputDataVertex<FTrigger>("Play", FDataVertexMetadata{ LOCTEXT("MetaSoundSoundfontPlayerNode_InputPlayDesc", "Plays the given note") })
				),
				FOutputVertexInterface(
					//TOutputDataVertex<Metasound::FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputAudioLeft)),
					//TOutputDataVertex<Metasound::FAudioBuffer>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutputAudioRight))
					TOutputDataVertex<Metasound::FSynthInstance>(METASOUND_GET_PARAM_NAME_AND_METADATA(OutSynthInstance))
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
						FNodeClassName { StandardNodes::Namespace, "CreateSynthInstance Node", StandardNodes::AudioVariant },
						1, // Major Version
						0, // Minor Version
						METASOUND_LOCTEXT("CreateSynthInstanceNodeDisplayName", "CreateSynthInstance Node"),
						METASOUND_LOCTEXT("CreateSynthInstanceNodeDesc", "Creates a Synth Instance to be used by SoundfontMIDIPlayer"),
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
			////Metasound::FBoolReadRef Enabled;
			//HarmonixMetasound::FMidiStreamReadRef MidiStream;
			//Metasound::FSynthInstanceReadRef SynthInstance;

			//FTriggerReadRef PlayReadRef;
			FStringReadRef SynthInstanceName;
			FStringReadRef SoundfontPath;

		};
		
		struct FOutputs
		{
			//Metasound::FAudioBufferWriteRef AudioLeft;
			//Metasound::FAudioBufferWriteRef AudioRight;

			Metasound::FSynthInstanceWriteRef SynthInstance;
		};


		virtual void BindInputs(FInputVertexInterfaceData& InOutVertexData) override
		{
			InOutVertexData.BindReadVertex("SynthInstanceName", Inputs.SynthInstanceName);
			InOutVertexData.BindReadVertex("SoundfontPath", Inputs.SoundfontPath);

			//InOutVertexData.BindReadVertex("MidiStream", Inputs.MidiStream);
			//InOutVertexData.BindReadVertex("SynthInstance", Inputs.SynthInstance);
			//InOutVertexData.BindReadVertex("Play", Inputs.PlayReadRef);
		}

		virtual void BindOutputs(FOutputVertexInterfaceData& InOutVertexData) override
		{
			//InOutVertexData.BindReadVertex("OutputAudioLeft", TDataReadReference<FAudioBuffer>(Outputs.AudioLeft));
			//InOutVertexData.BindReadVertex("OutputAudioRight", TDataReadReference<FAudioBuffer>(Outputs.AudioRight));
			InOutVertexData.BindWriteVertex("SynthInstance", TDataWriteReference<FSynthInstance>(Outputs.SynthInstance));
		}



		//// Allows MetaSound graph to interact with your node's inputs
		//virtual FDataReferenceCollection GetInputs() const override
		//{
		//	using namespace CreateSynthInstanceNodeNames;

		//	FDataReferenceCollection InputDataReferences;

		//	//InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputMidiStream), Inputs.MidiStream);
		//	//InputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(InputSynthInstance), Inputs.SynthInstance);

		//	return InputDataReferences;
		//}

		//// Allows MetaSound graph to interact with your node's outputs
		//virtual FDataReferenceCollection GetOutputs() const override
		//{
		//	using namespace CreateSynthInstanceNodeNames;

		//	FDataReferenceCollection OutputDataReferences;

		//	//OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputAudioLeft), SoundfontMIDIStreamPlayerNodeOutput);
		//	OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputAudioLeft), Outputs.AudioLeft);
		//	OutputDataReferences.AddDataReadReference(METASOUND_GET_PARAM_NAME(OutputAudioRight), Outputs.AudioRight);

		//	return OutputDataReferences;
		//}

		static TUniquePtr<Metasound::IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults)
		{
			const FOperatorSettings& Settings = InParams.OperatorSettings;
			const FInputVertexInterfaceData& InputData = InParams.InputData;
			//HarmonixMetasound::FMidiStreamReadRef InMidiStream = InputData.GetOrConstructDataReadReference<HarmonixMetasound::FMidiStream>("MidiStream");
			//FSynthInstanceReadRef InSynthInstance = InputData.GetOrConstructDataReadReference<FSynthInstance>("SynthInstance");
			//FTriggerReadRef InPlayReadRef = InputData.GetOrConstructDataReadReference<FTrigger>( "Play", InParams.OperatorSettings);

			FStringReadRef SynthInstanceName = InputData.GetOrConstructDataReadReference<FString>("SynthInstanceName");
			FStringReadRef SoundfontPath = InputData.GetOrConstructDataReadReference<FString>("SoundfontPath");

			return MakeUnique<FCreateSynthInstanceOperator>(InParams, SynthInstanceName, SoundfontPath);
		}

public:
		// Primary node functionality
		void Execute()
		{
			//TObjectPtr<USoundfontSubsystem> SfSubsys = GEngine->GetEngineSubsystem<USoundfontSubsystem>();
			//ensure(SfSubsys);
			//if (SfSubsys && !SynthInstance.IsValid())
			//{
			//	SynthInstance = MakeShared<FSynthInstanceProxy, ESPMode::ThreadSafe>(SfSubsys->CreateSynthInstance(FName(*Inputs.SynthInstanceName)));

			//	FString RelativePath = FPaths::ProjectContentDir();
			//	FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
			//	FString Path = *(FullPath + *Inputs.SoundfontPath);
			//	SynthInstance->SynthInstance->sfload(Path, 1);
			//}

			//Outputs.SynthInstance->SetSynthInstanceProxy(SynthInstance);




			if (SynthInstance == nullptr)
			{
				TObjectPtr<USoundfontSubsystem> SfSubsys = GEngine->GetEngineSubsystem<USoundfontSubsystem>();
				ensure(SfSubsys);
				if (SfSubsys)
				{
					SynthInstance = SfSubsys->CreateNewSynthInstance(FName(*Inputs.SynthInstanceName));

					FString FilePath;
					if (Inputs.SoundfontPath->Contains(":"))
					{
						FilePath = *Inputs.SoundfontPath;
					}
					else
					{
						FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*FPaths::ProjectContentDir());
						FilePath = *(FullPath + *Inputs.SoundfontPath);
					}

					SynthInstance->sfload(FilePath, 1);
				}
			}
			const FSynthInstance& Inst = *Outputs.SynthInstance;
			FSynthInstanceProxyPtr Proxy = Inst.GetSynthInstanceProxy();
			Proxy->SynthInstance = SynthInstance;






			//{
			//	if (SynthInstance != nullptr)

			//	if (Outputs.SynthInstance.Get())
			//	{
			//		if (Outputs.SynthInstance->GetSynthInstanceProxy().IsValid())
			//		{
			//			USynthInstance*& SynthInstance = Outputs.SynthInstance->GetSynthInstanceProxy()->SynthInstance;
			//			if (SynthInstance == nullptr)
			//			{
			//				SynthInstance = SfSubsys->CreateSynthInstance(FName(*Inputs.SynthInstanceName));

			//				FString RelativePath = FPaths::ProjectContentDir();
			//				FString FullPath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*RelativePath);
			//				FString Path = *(FullPath + *Inputs.SoundfontPath);
			//				SynthInstance->sfload(Path, 1);
			//			}
			//		}
			//	}
			//}

		}

	private:
		FInputs Inputs;
		FOutputs Outputs;

		USynthInstance* SynthInstance = nullptr;
	};

	// Node Class - Inheriting from FNodeFacade is recommended for nodes that have a static FVertexInterface
	class FCreateSynthInstanceNode : public FNodeFacade
	{
	public:
		FCreateSynthInstanceNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FCreateSynthInstanceOperator>())
		{
		}
	};






	// Register node
	METASOUND_REGISTER_NODE(FCreateSynthInstanceNode);
}
















#undef LOCTEXT_NAMESPACE