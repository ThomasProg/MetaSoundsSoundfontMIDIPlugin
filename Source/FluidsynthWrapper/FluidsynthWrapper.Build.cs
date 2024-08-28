// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FluidsynthWrapper : ModuleRules
{
	public FluidsynthWrapper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		OptimizeCode = CodeOptimization.Never;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"FluidsynthWrapperLibrary",
				"Projects",
				"MetasoundEngine",
				"MetasoundFrontend",
				"MetasoundStandardNodes",
				//"AudioCodecEngine",
				"Serialization",
				"AudioPlatformConfiguration",
				"WaveTable"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				// ... add private dependencies that you statically link with here ...	
					"AudioExtensions",
					"AudioMixer",
					"MetasoundGraphCore",
					"MetasoundGenerator",
					"SignalProcessing"
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
