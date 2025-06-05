// Copyright Prog'z. All Rights Reserved.

#pragma once

//#include "DSPProcessing/Volume.h"
#include "MetasoundEnumRegistrationMacro.h"
#include "MetasoundParamHelper.h"

#define IS_VERSION(MAJOR, MINOR) (ENGINE_MAJOR_VERSION == MAJOR) && (ENGINE_MINOR_VERSION == MINOR)
#define IS_VERSION_OR_PREV(MAJOR, MINOR) (ENGINE_MAJOR_VERSION == MAJOR) && (ENGINE_MINOR_VERSION <= MINOR)
#define IS_VERSION_OR_AFTER(MAJOR, MINOR) (ENGINE_MAJOR_VERSION == MAJOR) && (ENGINE_MINOR_VERSION >= MINOR)

namespace Metasound
{
	//------------------------------------------------------------------------------------
	// FVolumeOperator
	//------------------------------------------------------------------------------------
	class FVolumeOperator : public TExecutableOperator<FVolumeOperator>
	{
	public:
		static const FNodeClassMetadata& GetNodeInfo();
		static const FVertexInterface& GetVertexInterface();
#if IS_VERSION_OR_PREV(5, 4)
		static TUniquePtr<IOperator> CreateOperator(const FCreateOperatorParams& InParams, FBuildErrorArray& OutErrors);
#elif IS_VERSION_OR_AFTER(5, 6)
		static TUniquePtr<IOperator> CreateOperator(const FBuildOperatorParams& InParams, FBuildResults& OutResults);
#endif

		FVolumeOperator(const FOperatorSettings& InSettings, const FAudioBufferReadRef& InAudioInput, const FFloatReadRef& InAmplitude);

		//virtual FDataReferenceCollection GetInputs()  const override;
		//virtual FDataReferenceCollection GetOutputs() const override;
		virtual void BindInputs(FInputVertexInterfaceData& InVertexData) override;
		virtual void BindOutputs(FOutputVertexInterfaceData& InVertexData) override;

		void Execute();

	private:
		FAudioBufferReadRef	 AudioInput;
		FAudioBufferWriteRef AudioOutput;

		//DSPProcessing::FVolume VolumeDSPProcessor;

		FFloatReadRef Amplitude;
	};

	//------------------------------------------------------------------------------------
	// FVolumeNode
	//------------------------------------------------------------------------------------
	class FVolumeNode : public FNodeFacade
	{
	public:
		// Constructor used by the Metasound Frontend.
		FVolumeNode(const FNodeInitData& InitData)
			: FNodeFacade(InitData.InstanceName, InitData.InstanceID, TFacadeOperatorClass<FVolumeOperator>())
		{

		}
	};
}
