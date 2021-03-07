// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class UPTProject : ModuleRules
{
    public UPTProject(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        OptimizeCode = CodeOptimization.Never;
        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "UnrealUPT",
            });

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "PrintHelper",
            });

        // Uncomment if you are using Slate UI
        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Slate",
                "SlateCore",
                "UMG"
            });

        // Uncomment if you are using online features
        // PrivateDependencyModuleNames.Add("OnlineSubsystem");

        // To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
    }
}