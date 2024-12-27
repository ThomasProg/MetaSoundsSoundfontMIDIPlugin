// Copyright Prog'z. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "MetasoundDataReference.h"
#include "MetasoundDataTypeRegistrationMacro.h"
#include "IAudioProxyInitializer.h"
#include "SynthInstance.h"

namespace Metasound
{
	class FSynthInstance;
	using FSynthInstanceReadRef = TDataReadReference<FSynthInstance>;

	// Metasound data type that holds onto a weak ptr. Mostly used as a placeholder until we have a proper proxy type.
	class FLUIDSYNTHWRAPPER_API FSynthInstance
	{
		FSynthInstanceProxyPtr SynthInstanceProxy = MakeShared<FSynthInstanceProxy, ESPMode::ThreadSafe>(nullptr);

	public:

		FSynthInstance() = default;
		FSynthInstance(const FSynthInstance&) = default;
		FSynthInstance& operator=(const FSynthInstance& Other) = default;

		FSynthInstance(const TSharedPtr<Audio::IProxyData>& InInitData);

		const FSynthInstanceProxyPtr& GetSynthInstanceProxy() const
		{
			return SynthInstanceProxy;
		}

	};

	DECLARE_METASOUND_DATA_REFERENCE_TYPES(FSynthInstance, FLUIDSYNTHWRAPPER_API, FSynthInstanceTypeInfo, FSynthInstanceReadRef, FSynthInstanceWriteRef)
}

