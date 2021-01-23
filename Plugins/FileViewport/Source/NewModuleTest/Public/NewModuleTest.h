// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "NewModuleTest.generated.h"

class FNewModuleTest : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	void StartupModule() override;
	void ShutdownModule() override;
};

UCLASS()
class UCreateCodeFilesBPLirary :public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		static void CreateCodeFile();
};
