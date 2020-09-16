// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SSlateTestArea :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSlateTestArea)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void SetTestWidget(TSharedRef<SWidget> Widget);

private:
};