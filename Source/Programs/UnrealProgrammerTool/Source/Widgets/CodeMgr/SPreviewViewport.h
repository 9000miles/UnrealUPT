// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SPreviewViewport : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SPreviewViewport)
	{ }
	SLATE_END_ARGS()

	void Construct(const FArgumets& InArgs);

private:
};