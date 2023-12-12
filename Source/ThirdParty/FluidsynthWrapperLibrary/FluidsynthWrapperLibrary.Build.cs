// Fill out your copyright notice in the Description page of Project Settings.

using Microsoft.Extensions.Logging;
using System.IO;
using UnrealBuildTool;

public class FluidsynthWrapperLibrary : ModuleRules
{
	public string BinDir { get => "C:\\Users\\thoma\\PandorasBox\\Projects\\ModularMusicGenerationModules\\Modules\\RuntimeModules\\fluidsynth\\Downloads\\bin"; }
	public string IncludeDir { get => "C:\\Users\\thoma\\PandorasBox\\Projects\\ModularMusicGenerationModules\\Modules\\RuntimeModules\\fluidsynth\\Downloads\\include"; }
	public string LibDir { get => "C:\\Users\\thoma\\PandorasBox\\Projects\\ModularMusicGenerationModules\\Modules\\RuntimeModules\\fluidsynth\\Downloads\\lib"; }

	public FluidsynthWrapperLibrary(ReadOnlyTargetRules Target) : base(Target)
	{
		Type = ModuleType.External;

		PublicIncludePaths.Add(IncludeDir);
		PublicAdditionalLibraries.Add(Path.Combine(LibDir,"libfluidsynth.a"));
		PublicAdditionalLibraries.Add(Path.Combine(LibDir, "libfluidsynth.dll.a"));

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

		foreach (string s in str)
		{
			PublicDelayLoadDLLs.Add(s);
			RuntimeDependencies.Add(Path.Combine(BinDir, s));
			Logger.LogTrace(Path.Combine(BinDir, s));
		}
	}
}
