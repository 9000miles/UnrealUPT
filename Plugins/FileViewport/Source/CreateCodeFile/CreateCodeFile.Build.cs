// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class CreateCodeFile : ModuleRules
{
    public CreateCodeFile(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicIncludePaths.AddRange(
            new string[]
            {
                Path.Combine(ModuleDirectory, "Public")
            }
            );

        PrivateIncludePaths.AddRange(
            new string[]
            {
            }
            );

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "GameProjectGeneration",
                "EngineSettings",
                "Projects",
                "UnrealEd",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "ToolMenus",
                "AppFramework",
                "DesktopPlatform",
            }
            );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
            }
            );
    }
}