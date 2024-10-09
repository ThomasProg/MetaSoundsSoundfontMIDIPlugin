// Fill out your copyright notice in the Description page of Project Settings.


#include "MetasoundSynthInstance.h"

namespace Metasound
{

FSynthInstance::FSynthInstance(const TSharedPtr<Audio::IProxyData>& InInitData)
{
	if (InInitData.IsValid())
	{
		if (InInitData->CheckTypeCast<FSynthInstanceProxy>())
		{
			SynthInstanceProxy = MakeShared<FSynthInstanceProxy, ESPMode::ThreadSafe>(InInitData->GetAs<FSynthInstanceProxy>());
		}
	}
}

}
