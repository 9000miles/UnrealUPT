// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SCodeMgr : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCodeMgr)
	{ }
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow);

private:
	TSharedRef<SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier);

private:
	TSharedPtr<class FTabManager> TabManager;
	TSharedPtr<class SDirectoryTree> DirectoryTree;
	TSharedPtr<class SFileViewport> FileViewport;
	TSharedPtr<class SCodeDetails> CodeDetails;
	TSharedPtr<class SCodeMgrState> CodeMgrState;
};