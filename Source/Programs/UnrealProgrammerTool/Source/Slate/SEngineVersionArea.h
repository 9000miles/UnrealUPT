// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;

class SEngineVersionArea :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEngineVersionArea)
	{	}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const FString& Version, TArray<TSharedPtr<FProjectInfo>>& Projects);

private:
	TSharedRef<SWidget> CreateAreaHeader(const FString& Version);
	TSharedRef<SWidget> CreateAreaBody(TArray<TSharedPtr<FProjectInfo>>& Projects);

private:
	FString EngineVersion;
};