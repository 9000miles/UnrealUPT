// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SDetailArea :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SDetailArea)
	{}
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void SetObject(UObject* Object);

private:
	TSharedPtr<class IDetailsView> DetailView;
};