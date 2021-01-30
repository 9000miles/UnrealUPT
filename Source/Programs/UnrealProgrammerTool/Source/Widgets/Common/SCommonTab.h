// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

struct FProjectInfo;
class SVerticalBox;

DECLARE_DELEGATE_OneParam(FOnTabActive, const FString& /*TabName*/);
DECLARE_DELEGATE_RetVal_TwoParams(const FSlateBrush*, FOnGetTabBrush, const FString& /*TabName*/, bool& /*bSourceEngien*/);
DECLARE_DELEGATE_RetVal_OneParam(const FText, FOnGetToolTipText, const FString& /*TabName*/);

class SCommonTab :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCommonTab)
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

protected:
	FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

private:
	TSharedRef<SWidget> CreateTab(FString Version);
	TSharedRef<SWidget> MakeMenuContent();

	void OnEngineTabChanged(ECheckBoxState NewState, FString EngineVersion);
	ECheckBoxState GetEngineTabCheckedState(FString EngineVersion) const;
	const FSlateBrush* GetActiveTabIamge(FString EngineVersion) const;
	const FSlateBrush* GetSourceOrBinaryImage(FString EngineVersion, bool& bSource) const;

	FText TabToolTipText(FString EngineVersion, bool bSource) const;

private:
	FString ActiveEngineTab;
	FString ActiveEngineVersion;
	TArray<FString> TabNames;
	TSharedPtr<SVerticalBox> TabBox;

	FOnTabActive OnTabActive;
	FOnGetTabBrush OnGetTabBrush;
	FOnGetToolTipText OnGetToolTipText;
};