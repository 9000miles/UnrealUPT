// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SDockTab.h"
#include "TabManager.h"
#include "SCodeTestArea.h"
#include "SSimpleTestMainFrame.h"

struct FCodeTestData;

class SWorkArea :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SWorkArea)
	{}
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, FOnTestRowClick InOnTestItemClick, FOnRunTestItemClicked OnItemRunTest);
	void SwitchArea(bool bSwitchToCodeTest);

	void SetSlateTestWidget(TSharedPtr<SWidget> Widget);
	void SetCodeTestPaths(TArray<TSharedPtr<FCodeTestData>> TestPaths);

private:
	TSharedPtr<class SCodeTestArea> CodeTestArea;
	TSharedPtr<class SSlateTestArea> SlateTestArea;
};