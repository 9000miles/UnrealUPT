// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;


class SUPTMainFrame :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUPTMainFrame)
	{	}
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& AllProjects);

private:
	void CreateAllProjects(TSharedRef<SVerticalBox>& VerticalBox, TArray<TSharedPtr<FProjectInfo>>& AllProjects);

private:
	//Key：引擎路径，Value：工程信息
	TMap<FString, TArray<TSharedPtr<FProjectInfo>>> Map;
};