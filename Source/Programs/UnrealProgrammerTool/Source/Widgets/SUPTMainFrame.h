// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;
class SEngineTab;
class SEngineProjects;

class SUPTMainFrame :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SUPTMainFrame)
	{	}
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& AllProjects);
	void RequestRefresh(TArray<TSharedPtr<FProjectInfo>>& AllProjects);

private:
	void InitEngineProjects(TArray<TSharedPtr<FProjectInfo>>& AllProjects);

	void OnEngineTabChanged(const FString& EngineVersion);
	const FSlateBrush* GetSourceOrBinaryImage(const FString& EngineVersion);
	const FText OnGetEngineDir(const FString& EngineVersion);

private:
	TSharedPtr<SEngineTab> EngineTab;
	TSharedPtr<SEngineProjects> EngineProjects;

	//Key：引擎路径，Value：工程信息
	TMap<FString, TArray<TSharedPtr<FProjectInfo>>> Map;
};