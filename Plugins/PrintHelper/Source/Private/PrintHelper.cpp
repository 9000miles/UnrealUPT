// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PrintHelper.h"
#if WITH_EDITOR
#include "PrintHelperSettings.h"
#include "Developer/Settings/Public/ISettingsModule.h"
#include "Developer/Settings/Public/ISettingsSection.h"
#endif

#define LOCTEXT_NAMESPACE "FPrintHelperModule"

void FPrintHelperModule::StartupModule()
{
#if WITH_EDITOR
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");
	if (SettingsModule == nullptr)
		return;

	ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", "PrintHelper",
		LOCTEXT("PrintHelperSettingsName", "Print Helper"),
		LOCTEXT("PrintHelperSettingsDescription", "Printout Settings"),
		GetMutableDefault<UPrintHelperSettings>());

	if (SettingsSection.IsValid())
	{
		SettingsSection->OnModified().BindRaw(this, &FPrintHelperModule::HandleSettingsSaved);
	}

	const auto& Settings = GetMutableDefault<UPrintHelperSettings>();
	FPrintTool::Get()->SetEnable(Settings->bEnablePrintOutput);
	FPrintTool::Get()->SetSettings(Settings->bEnablePrintFile, Settings->bEnablePrintLine, Settings->bEnableCallStack);
#endif
}

bool FPrintHelperModule::HandleSettingsSaved()
{
#if WITH_EDITOR
	const auto& Settings = GetMutableDefault<UPrintHelperSettings>();
	Settings->SaveConfig();
	FPrintTool::Get()->SetEnable(Settings->bEnablePrintOutput);
	FPrintTool::Get()->SetSettings(Settings->bEnablePrintFile, Settings->bEnablePrintLine, Settings->bEnableCallStack);

	return true;
#endif
	return false;
}

void FPrintHelperModule::ShutdownModule()
{
#if WITH_EDITOR
	ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule != nullptr)
	{
		SettingsModule->UnregisterSettings("Editor", "Plugins", "PrintHelper");
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPrintHelperModule, PrintHelper)