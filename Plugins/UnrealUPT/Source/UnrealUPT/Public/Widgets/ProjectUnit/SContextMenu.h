// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;

class SContextMenu :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SContextMenu)
	{	}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, TSharedPtr<FProjectInfo> Info);
	void LaunchGame(TSharedPtr<FProjectInfo> Info);
	void PackageProject(TSharedPtr<FProjectInfo> Info);
	void CopyPackageFiles(TSharedPtr<FProjectInfo> Info);

private:
	void HandleUatProcessCanceled();
	void HandleUatProcessCompleted(int32 Result);
	static void HandleUatProcessOutput(FString Message);

private:
	TSharedPtr<FProjectInfo> ProjectInfo;
};