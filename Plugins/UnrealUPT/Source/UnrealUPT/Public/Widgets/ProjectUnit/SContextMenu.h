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

private:
	TSharedPtr<FProjectInfo> ProjectInfo;
};