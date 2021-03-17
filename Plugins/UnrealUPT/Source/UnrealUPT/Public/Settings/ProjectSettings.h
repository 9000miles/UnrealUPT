// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectSettings.generated.h"

/**
 *
 */
USTRUCT()
struct FProjectSettings
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(Category = "ProjectSettings", config, EditDefaultsOnly)
		FString ProjectPath;
	UPROPERTY(Category = "ProjectSettings", config, EditDefaultsOnly)
		FString LaunchParameters;
	UPROPERTY(Category = "ProjectSettings", config, EditDefaultsOnly)
		FString PackageDir;
	UPROPERTY(Category = "ProjectSettings", config, EditDefaultsOnly)
		FString PackingParameters;
};