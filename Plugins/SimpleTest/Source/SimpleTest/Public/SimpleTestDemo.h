// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimpleTestDemo.generated.h"

UCLASS(BlueprintType, Blueprintable)
class UTestDemo : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Hell)
		TArray<int32> HelloAArray;

	UPROPERTY(EditAnywhere, Category = Hell)
		int32 HelloI;

	UPROPERTY(EditAnywhere, Category = Hell)
		float HelloF;
};

UCLASS(BlueprintType, Blueprintable)
class UTestDemo_New : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = Hell)
		TSoftObjectPtr<UTexture2D> Texture;

	UPROPERTY(EditAnywhere, Category = Hell)
		int32 Key;
};

UCLASS(BlueprintType, Blueprintable)
class UProjectTestSettings : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "TestSettings")
		FString ProjectPath;
};