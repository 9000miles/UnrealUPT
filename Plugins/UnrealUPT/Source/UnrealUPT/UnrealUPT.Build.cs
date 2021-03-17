// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealUPT : ModuleRules
{
    public UnrealUPT(ReadOnlyTargetRules Target) : base(Target)
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
                "InputCore",
                "Engine",
                "AppFramework",
                "ApplicationCore",
                "Projects",
                "Slate",
                "SlateCore",
                "SourceCodeAccess",
                "EditorStyle",
                "PrintHelper",
                "Json",
                "UMG",
				// ... add other public dependencies that you statically link with here ...
			}
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "SlateReflector",
                "EngineSettings",
                "DesktopPlatform",
                "DeveloperSettings",
				// ... add private dependencies that you statically link with here ...
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