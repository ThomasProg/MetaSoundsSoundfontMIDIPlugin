// Copyright Prog'z. All Rights Reserved.

#include "FluidsynthWrapper.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "MetasoundFrontendRegistries.h"

#define LOCTEXT_NAMESPACE "FFluidsynthWrapperModule"

void FFluidsynthWrapperModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("FluidsynthWrapper")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
#if PLATFORM_WINDOWS
	//FString s = "C:/Users/thoma/PandorasBox/Projects/ModularMusicGenerationModules/Modules/RuntimeModules/fluidsynth/Downloads/bin";
	FString s = FPaths::ProjectPluginsDir() + "/MetaSoundsSoundfontMIDIPlugin/Source/ThirdParty/FluidsynthWrapperLibrary/fluidsynth/bin";
	LibraryPath = FPaths::Combine(*s, TEXT("libfluidsynth-3.dll"));


	//LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/FluidsynthWrapperLibrary/Win64/ExampleLibrary.dll"));
#elif PLATFORM_MAC
    LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/FluidsynthWrapperLibrary/Mac/Release/libExampleLibrary.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/FluidsynthWrapperLibrary/Linux/x86_64-unknown-linux-gnu/libExampleLibrary.so"));
#endif // PLATFORM_WINDOWS

	ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;

	if (ExampleLibraryHandle)
	{
		// Call the test function in the third party library that opens a message box
		//ExampleLibraryFunction();
	}
	else
	{
		FText ErrorMessage = FText::Format(
			LOCTEXT("ThirdPartyLibraryError", "FluidsynthWrapper: Failed to load the library at path: {0}"),
			FText::FromString(LibraryPath)
		);
		FMessageDialog::Open(EAppMsgType::Ok, ErrorMessage);
	}

	FMetasoundFrontendRegistryContainer::Get()->RegisterPendingNodes();
}

void FFluidsynthWrapperModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFluidsynthWrapperModule, FluidsynthWrapper)
