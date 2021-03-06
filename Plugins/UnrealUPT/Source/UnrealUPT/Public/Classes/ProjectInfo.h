// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FSlateBrush;

struct FProjectInfo
{
public:
	FProjectInfo(const FString& InProjectPath, const TSharedPtr<FSlateBrush>& InThumbnail);

	const FString GetProjectName() const { return ProjectName; }
	const FString GetProjectPath() const { return ProjectPath; }
	const FString GetEnginePath() const { return EnginePath; }
	const FString GetEngineVersion() const { return EngineVersion; }
	const TSharedPtr<FSlateBrush> GetThumbanil() const { return Thumbnail; }

private:
	FString ProjectName;
	FString ProjectPath;

	FString EnginePath;
	FString EngineVersion;

	TSharedPtr<FSlateBrush> Thumbnail;
};