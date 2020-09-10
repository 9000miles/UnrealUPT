// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;
class SProjectTileView;

class SEngineProjects :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEngineProjects)
	{	}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& Projects);
	void Refresh(TArray<TSharedPtr<FProjectInfo>>& Projects);


private:
	void Init(TArray<TSharedPtr<FProjectInfo>> &Projects);
	TSharedRef<SWidget> CreateAreaHeader();
	TSharedRef<SWidget> CreateAreaBody(TArray<TSharedPtr<FProjectInfo>>& Projects);

private:
	FString EnginePath;
	int32 ProjectCount;

	TSharedPtr<SProjectTileView> ProjectTileView;
};