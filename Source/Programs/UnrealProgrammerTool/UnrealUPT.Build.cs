// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class UnrealUPT : ModuleRules
{
    public UnrealUPT(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        OptimizeCode = CodeOptimization.InShippingBuildsOnly;

        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(EngineDirectory, "Source/Runtime/Launch/Public"),

                Path.Combine(ModuleDirectory, "Source/Menu"),
                Path.Combine(ModuleDirectory, "Source/Classes"),
                Path.Combine(ModuleDirectory, "Source/Launcher"),
                Path.Combine(ModuleDirectory, "Source/Manager"),
                Path.Combine(ModuleDirectory, "Source/Utility"),

                Path.Combine(ModuleDirectory, "Source/Widgets"),
                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr"),
                Path.Combine(ModuleDirectory, "Source/Widgets/Common"),
                Path.Combine(ModuleDirectory, "Source/Widgets/MainFrame"),
                Path.Combine(ModuleDirectory, "Source/Widgets/ProjectUnit"),
                Path.Combine(ModuleDirectory, "Source/Widgets/Settings"),

                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr/CodeBrowser/Public"),
                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr/CodeBrowser/Private"),

                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr/CreateCodeFile/Public"),
                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr/CreateCodeFile/Private"),

                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr/FileViewport/Public"),
                Path.Combine(ModuleDirectory, "Source/Widgets/CodeMgr/FileViewport/Private"),

                Path.Combine(ModuleDirectory, "Source/PrintHelper/Public"),
                Path.Combine(ModuleDirectory, "Source/PrintHelper/Private"),
            }
            );

        System.Console.WriteLine("----------" + Path.Combine(ModuleDirectory, "Source/PrintHelper/Private"));
        PrivateIncludePaths.Add(System.IO.Path.Combine(EngineDirectory, "Source/Runtime/Launch/Private"));      // For LaunchEngineLoop.cpp include

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "InputCore",
            //"Engine",
            "AppFramework",
            "ApplicationCore",
            "Projects",
            "Slate",
            "SlateCore",
            "StandaloneRenderer",
            "SourceCodeAccess",
            "EditorStyle",
            //"PrintHelper",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "SlateReflector",
            "EngineSettings",
            "DesktopPlatform",
        });

        DynamicallyLoadedModuleNames.AddRange(new string[]
        {
            //"SlateReflector",
        });

        if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PrivateDependencyModuleNames.Add("XCodeSourceCodeAccess");
            AddEngineThirdPartyPrivateStaticDependencies(Target, "CEF3");
        }
        else if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            PrivateDependencyModuleNames.Add("VisualStudioSourceCodeAccess");
        }
    }
}