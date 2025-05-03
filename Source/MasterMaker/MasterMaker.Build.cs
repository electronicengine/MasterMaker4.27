// Copyright Epic Games, Inc. All Rights Reserved.

using System;
using System.IO;
using UnrealBuildTool;

public class MasterMaker : ModuleRules
{
	public MasterMaker(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", 
                                                            "CoreUObject", 
                                                            "Engine", 
                                                            "InputCore", 
                                                            "UMG", 
                                                            "AnimGraphRuntime",
                                                            "Json", 
                                                            "JsonUtilities", 
                                                            "PhysXVehicles", 
                                                            "Slate", 
                                                            "SlateCore", 
                                                            "AIModule", 
                                                            "NavigationSystem", 
                                                            "CableComponent", 
                                                            "OnlineSubsystem", 
                                                            "HeadMountedDisplay",
                                                            "Http",
                                                            "RenderCore",
                                                            "RHI",
                                                            "ImageWrapper",
                                                            "Landscape"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

        string BasePath = Path.Combine(ModuleDirectory, "../MasterMaker/LlamaCore");
        string IncludePath = Path.Combine(BasePath, "Includes");
        PublicIncludePaths.Add(IncludePath);

        if (Target.Platform == UnrealTargetPlatform.Android)
        {
            // Define possible architectures
            string[] Architectures = { "armeabi-v7a", "arm64-v8a", "x86", "x86_64" };

            // Loop through each architecture
            foreach (string Arch in Architectures)
            {
                string LibPath = Path.Combine(BasePath, Arch);

                // List of libraries to link
                string[] SharedLibs = new string[]
                {
                    "libggml.so",
                    "libggml-base.so",
                    "libggml-cpu.so",
                    "libllama.so",
                    "libllama-android.so",
                    "libomp.so"
                };

                // Add libraries to PublicAdditionalLibraries
                foreach (string Lib in SharedLibs)
                {
                    PublicAdditionalLibraries.Add(Path.Combine(LibPath, Lib));
                    PublicDelayLoadDLLs.Add(Lib); // You can remove this line if delay load is not needed
                }
            }

            AdditionalPropertiesForReceipt.Add("AndroidPlugin", Path.Combine(ModuleDirectory, "LlamaCore/LlamaAndroid.xml"));

        }

        else if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            string LibPath = Path.Combine(BasePath, "Win64");

            string[] Dlls = new string[]
            {
                "llama.dll",
                "ggml.dll",
                "ggml-base.dll",
                "ggml-cpu.dll",
                "ggml-cuda.dll",
            };

            string[] Libs = new string[]
            {
                "llama.lib",
                "common.lib",
                "ggml.lib",
                "ggml-base.lib",
                "ggml-cpu.lib"
            };

            foreach (string Lib in Libs)
            {
                PublicAdditionalLibraries.Add(Path.Combine(LibPath, Lib));
            }

            foreach (string Dll in Dlls)
            {
                string DllPath = Path.Combine(LibPath, Dll);
                PublicDelayLoadDLLs.Add(Dll);
            }

            RuntimeDependencies.Add("$(TargetOutputDir)/llama.dll", Path.Combine(LibPath, "llama.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/ggml.dll", Path.Combine(LibPath, "ggml.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/ggml-base.dll", Path.Combine(LibPath, "ggml-base.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/ggml-cpu.dll", Path.Combine(LibPath, "ggml-cpu.dll"));
            RuntimeDependencies.Add("$(TargetOutputDir)/ggml-cuda.dll", Path.Combine(LibPath, "ggml-cuda.dll"));

            PublicSystemLibraryPaths.Add(LibPath);
        }
    }
}
