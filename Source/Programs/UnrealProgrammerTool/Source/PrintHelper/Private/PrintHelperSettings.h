// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrintHelperSettings.generated.h"

UCLASS(config = Game, defaultconfig, Category = "PrintHelper", transient)
class UPrintHelperSettings : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(Category = "PrintHelperSettings", config, EditDefaultsOnly)
		bool bEnablePrintOutput = true;

	UPROPERTY(Category = "PrintHelperSettings", config, EditDefaultsOnly)
		bool bEnablePrintFileLine = true;
};