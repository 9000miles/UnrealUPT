// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SVerticalBox;

class SCommandArea :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCommandArea)
	{	}
	SLATE_EVENT(FSimpleDelegate, OnSwitchedToCodeTestClicked)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void AddCommandWidget(TSharedRef<SWidget> Widget);

private:
	FReply OnSwitchCodeTestClick();

private:
	TSharedPtr<class SVerticalBox> CommandBox;
	FSimpleDelegate OnSwitchedToCodeTestClicked;
};