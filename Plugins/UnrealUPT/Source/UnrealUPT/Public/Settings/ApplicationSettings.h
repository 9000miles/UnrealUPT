// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ApplicationSettings.generated.h"

/**
 *
 */
USTRUCT()
struct FApplicationSettings
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(Category = "ApplicationSettings", Config, EditDefaultsOnly)
		FString ExePath;
	UPROPERTY(Category = "ApplicationSettings", Config, EditDefaultsOnly)
		FString IconPath;
};
