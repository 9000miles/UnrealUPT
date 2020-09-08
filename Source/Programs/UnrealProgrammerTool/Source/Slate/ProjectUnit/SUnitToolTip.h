// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SToolTip.h"

class SProjectUnit;

class SUnitToolTip : public SToolTip
{
public:
	SLATE_BEGIN_ARGS(SUnitToolTip)
		: _ProjectUnit()
	{ }
	SLATE_ARGUMENT(TSharedPtr<SProjectUnit>, ProjectUnit)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	// IToolTip interface
	virtual bool IsEmpty() const override;

	virtual void OnOpening() override;

	virtual void OnClosed() override;

private:
	TSharedRef<SWidget> CreateToolTipWidget();

private:
	TWeakPtr<SProjectUnit> ProjectUnit;
};