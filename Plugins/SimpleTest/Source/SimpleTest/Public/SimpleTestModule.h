// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class SIMPLETEST_API FSimpleTestModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	void AddMenuExtension(FMenuBuilder& Builder);
	TSharedRef<class SDockTab> OnSpawnSimpleTestPluginTab(const class FSpawnTabArgs& SpawnTabArgs);
	void OpenWindow();

	UObject* OnRefreshDetailObject(const FString& TestPath);
	void OnWorkAreaRunTestButtonClick(const FString TestPath);
	TSharedPtr<SWidget> OnGetCommandWidget(const FString& Command);

private:
	TSharedPtr< FUICommandList> PluginCommands;
};
