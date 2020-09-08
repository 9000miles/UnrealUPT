// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Desktop)]
public class UnrealProgrammerToolTarget : TargetRules
{
    public UnrealProgrammerToolTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Program;
        LinkType = TargetLinkType.Monolithic;
        LaunchModuleName = "UnrealProgrammerTool";
        ExtraModuleNames.Add("EditorStyle");

        // Lean and mean
        bBuildDeveloperTools = true;

        // Never use malloc profiling in Unreal Header Tool.  We set this because often UHT is compiled right before the engine
        // automatically by Unreal Build Tool, but if bUseMallocProfiler is defined, UHT can operate incorrectly.
        bUseMallocProfiler = false;

        // Editor-only data, however, is needed
        bBuildWithEditorOnlyData = true;

        // Currently this app is not linking against the engine, so we'll compile out references from Core to the rest of the engine
        bCompileAgainstEngine = false;
        bCompileAgainstCoreUObject = true;
        bCompileAgainstApplicationCore = true;

        // UnrealHeaderTool is a console application, not a Windows app (sets entry point to main(), instead of WinMain())
        //bIsBuildingConsoleApplication = true;
    }
}