// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FOnCommandClick, const FString);

class SCommandItem :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCommandItem)
	{	}
	SLATE_EVENT(FOnCommandClick, OnCommandClick)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const FString InDisplayName, const FString InFileName, const int32 InLineNumber);

private:
	FReply OnCommandClicked();

private:
	FString DisplayName;
	FString FileName;
	int32 LineNumber;

	FOnCommandClick OnCommandClick;
};