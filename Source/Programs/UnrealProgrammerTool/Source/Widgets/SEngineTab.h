// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;
class SVerticalBox;

DECLARE_DELEGATE_OneParam(FOnTabActive, const FString& /*TabName*/);
DECLARE_DELEGATE_RetVal_OneParam(const FSlateBrush*, FOnGetTabBrush, const FString& /*TabName*/);
DECLARE_DELEGATE_RetVal_OneParam(const FText, FOnGetToolTipText, const FString& /*TabName*/);

class SEngineTab :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEngineTab)
	{	}
	SLATE_ARGUMENT(TArray<FString>, TabNames);
	SLATE_EVENT(FOnTabActive, OnTabActive);
	SLATE_EVENT(FOnGetTabBrush, OnGetTabBrush);
	SLATE_EVENT(FOnGetToolTipText, OnGetToolTipText);
	SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs);
	void Refresh(TArray<FString> Names);
	void SetActiveTab(const FString TabName);

private:
	TSharedRef<SWidget> CreateTab(FString Version);

	void OnEngineTabChanged(ECheckBoxState NewState, FString EngineVersion);
	ECheckBoxState GetEngineTabCheckedState(FString EngineVersion) const;
	const FSlateBrush* GetActiveTabIamge(FString EngineVersion) const;
	const FSlateBrush* GetSourceOrBinaryImage(FString EngineVersion) const;

	FText TabToolTipText(FString EngineVersion) const;

private:
	FString ActiveEngineTab;
	TArray<FString> TabNames;
	TSharedPtr<SVerticalBox> TabBox;

	FOnTabActive OnTabActive;
	FOnGetTabBrush OnGetTabBrush;
	FOnGetToolTipText OnGetToolTipText;
};