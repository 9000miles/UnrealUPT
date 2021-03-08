// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealUPT.h"
#include "PrintHelper.h"
#include "Widgets/UPTStyle.h"
#include "Menu/UPTCommands.h"

#define LOCTEXT_NAMESPACE "FUnrealUPTModule"

void FUnrealUPTModule::StartupModule()
{
	FUPTStyle::Initialize();
	FUPTStyle::ReloadTextures();
	FUPTCommands::Register();

	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FUnrealUPTModule::ShutdownModule()
{
	FUPTStyle::Shutdown();
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUnrealUPTModule, UnrealUPT)