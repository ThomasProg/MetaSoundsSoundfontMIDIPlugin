// Fill out your copyright notice in the Description page of Project Settings.

using Microsoft.Extensions.Logging;
using System.IO;
using UnrealBuildTool;

public class FluidsynthWrapperLibrary : ModuleRules
{
	public string BinDir { get => "fluidsynth\\bin"; }
	public string IncludeDir { get => "fluidsynth\\include"; }
	public string LibDir { get => "fluidsynth\\lib"; }

	public FluidsynthWrapperLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, IncludeDir));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, LibDir, "libfluidsynth.a"));
		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, LibDir, "libfluidsynth.dll.a"));

		string[] str = {
			"fluidsynth.exe",
			"libfluidsynth-3.dll",
			"libgcc_s_sjlj-1.dll",
			"libglib-2.0-0.dll",
			"libgobject-2.0-0.dll",
			"libgomp-1.dll",
			"libgthread-2.0-0.dll",
			"libinstpatch-2.dll",
			"libinstpatch-2.pdb",
			"libintl-8.dll",
			"libsndfile-1.dll",
			"libstdc++-6.dll",
			"libwinpthread-1.dll"
		};

        string OutputBinariesDir = "";

        // Ensure the DLL is copied to the right binaries folder
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            OutputBinariesDir = Path.Combine(ModuleDirectory, "..", "..", "Binaries", "Win64");
        }

        System.Diagnostics.Debug.Assert(!string.IsNullOrEmpty(OutputBinariesDir), "BinariesDir should not be empty!");

        foreach (string s in str)
        {
            PublicDelayLoadDLLs.Add(s);

            string DestDLLPath = Path.Combine(ModuleDirectory, OutputBinariesDir, s);
            string SrcDLLPath = Path.Combine(ModuleDirectory, BinDir, s);
            RuntimeDependencies.Add(DestDLLPath, SrcDLLPath);

            // include it in packaged builds
            string PackagedDestDLLPath = Path.Combine("$(BinaryOutputDir)", s);
            RuntimeDependencies.Add(PackagedDestDLLPath, SrcDLLPath);

            Logger.LogTrace(Path.Combine(BinDir, s));
        }
    }
}
