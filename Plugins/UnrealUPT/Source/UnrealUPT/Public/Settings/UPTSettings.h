// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DeveloperSettings.h"
#include "ProjectSettings.h"
#include "ApplicationSettings.h"
#include "UPTSettings.generated.h"

UCLASS(config = Game, defaultconfig, Category = "UPTSettings", transient)
class UUPTSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	static FProjectSettings GetProjectSetting(const FString Path);
	static FApplicationSettings GetApplicationSetting(const FString Path);

public:
	UPROPERTY(Category = "ProjectSettings", Config, EditDefaultsOnly)
		TArray<FProjectSettings> ProjectSettings;

	UPROPERTY(Category = "ApplicationSettings", Config, EditDefaultsOnly)
		TArray<FApplicationSettings> ApplicationSettings;
};
