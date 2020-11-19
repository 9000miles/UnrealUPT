// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CodeBrowserDelegates.h"
#include "CodeBrowserUtils.h"

FMovedContentFolder::FMovedContentFolder(const FString& InOldPath, const FString& InNewPath)
	: OldPath(InOldPath)
	, NewPath(InNewPath)
	, Flags(CodeBrowserUtils::IsFavoriteFolder(OldPath) ? EMovedContentFolderFlags::Favorite : EMovedContentFolderFlags::None)
{
	OldPath = InOldPath;
	NewPath = InNewPath;
}