// Copyright Progz. All Rights Reserved.

#pragma once

#include "MetasoundNodeInterface.h"

#include "HarmonixMetasound/Common.h"

#undef DECLARE_METASOUND_PARAM_EXTERN
#undef DECLARE_METASOUND_PARAM_ALIAS

// Redifinitation because of dll linkage
#define DECLARE_METASOUND_PARAM_EXTERN(NAME)      \
	FLUIDSYNTHWRAPPER_API extern const TCHAR* NAME##Name;       \
	FLUIDSYNTHWRAPPER_API extern const FText NAME##Tooltip;     \
	FLUIDSYNTHWRAPPER_API extern const FText NAME##DisplayName;

#define DECLARE_METASOUND_PARAM_ALIAS(NAME)      \
	FLUIDSYNTHWRAPPER_API extern const TCHAR* NAME##Name;       \
	FLUIDSYNTHWRAPPER_API extern const FText& NAME##Tooltip;     \
	FLUIDSYNTHWRAPPER_API extern const FText& NAME##DisplayName;


namespace SoundfontMIDIPlayer
{
	FLUIDSYNTHWRAPPER_API Metasound::FNodeClassName GetClassName();
	FLUIDSYNTHWRAPPER_API int32 GetCurrentMajorVersion();

	namespace Inputs
	{
		DECLARE_METASOUND_PARAM_ALIAS(MidiStream);
		DECLARE_METASOUND_PARAM_ALIAS(SynthInstance);
	}

	namespace Outputs
	{
		DECLARE_METASOUND_PARAM_ALIAS(AudioLeft);
		DECLARE_METASOUND_PARAM_ALIAS(AudioRight);
	}
}
