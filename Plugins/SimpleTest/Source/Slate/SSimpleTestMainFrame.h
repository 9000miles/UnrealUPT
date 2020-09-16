// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SCodeTestArea.h"

class SDockTab;
struct FCodeTestData;
struct FSlateTestData;

DECLARE_DELEGATE_RetVal_OneParam(UObject*, FOnGetDetailObject, const FString&);

DECLARE_DELEGATE_OneParam(FOnCommandClick, const FString);
DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<SWidget>, FOnGetCommandWidget, const FString&);

class SSimpleTestMainFrame :public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SSimpleTestMainFrame)
	{}
	SLATE_EVENT(FOnCommandClick, OnCommandClick);
	SLATE_EVENT(FOnGetDetailObject, OnGetDetailObject);
	SLATE_EVENT(FOnRunTestItemClicked, OnRunTestItemClicked);
	SLATE_EVENT(FOnGetCommandWidget, OnGetCommandWidget);
		SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs, const TSharedRef<SDockTab>& ConstructUnderMajorTab, const TSharedPtr<SWindow>& ConstructUnderWindow, TArray < TSharedPtr<FSlateTestData>> Commands, TArray<TSharedPtr<FCodeTestData>> TestPaths);
	void InitCommandArea(TArray<TSharedPtr<FSlateTestData>> CommandNames);
	void InitWorkArea(TArray<TSharedPtr<FCodeTestData>> TestPaths);

private:
	TSharedRef<SDockTab> HandleTabManagerSpawnTab(const FSpawnTabArgs& Args, FName TabIdentifier);
	void FillWindowMenu(FMenuBuilder& MenuBuilder, const TSharedPtr<class FTabManager> TabManager);

	void OnRefreshDetail(const FString TestPath);
	void OnRunTestButtonClick(const FString TestPath);

private:
	TSharedPtr<class SCommandArea> CommandArea;
	TSharedPtr<class SWorkArea> WorkArea;
	TSharedPtr<class SDetailArea> DetailArea;

	TSharedPtr<class FTabManager> TabManager;

	FOnCommandClick OnCommandClick;
	FOnGetDetailObject OnGetDetailObject;
	FOnRunTestItemClicked OnRunTestItemClicked;
	FOnGetCommandWidget OnGetCommandWidget;
};