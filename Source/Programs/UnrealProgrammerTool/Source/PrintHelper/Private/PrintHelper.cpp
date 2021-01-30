// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "PrintHelper.h"
#include "PrintHelperSettings.h"
#if WITH_EDITOR
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
		LOCTEXT("PrintHelperSettingsDescription", "Print Helper"),
		GetMutableDefault<UPrintHelperSettings>());

	if (SettingsSection.IsValid())
	{
		SettingsSection->OnModified().BindRaw(this, &FPrintHelperModule::HandleSettingsSaved);
	}

	const UPrintHelperSettings* Settings = GetDefault<UPrintHelperSettings>();
	FPrintTool::Get()->SetEnable(Settings->bEnablePrintOutput);
	FPrintTool::Get()->SetCanPrintFileLine(Settings->bEnablePrintFileLine);
#endif
}

bool FPrintHelperModule::HandleSettingsSaved()
{
#if WITH_EDITOR
	const auto& Settings = GetMutableDefault<UPrintHelperSettings>();
	Settings->SaveConfig();
	FPrintTool::Get()->SetEnable(Settings->bEnablePrintOutput);
	FPrintTool::Get()->SetCanPrintFileLine(Settings->bEnablePrintFileLine);

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
		SettingsModule->UnregisterSettings("Project", "Plugins", "Print Helper");
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FPrintHelperModule, PrintHelper)