// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "CreateCodeFileModule.generated.h"

class FCreateCodeFileModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
	void OpenNewFile();
private:
	TSharedPtr<class FUICommandList> PluginCommands;
};

UCLASS()
class UCreateCodeFilesBPLirary :public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		static void CreateCodeFile();
};
