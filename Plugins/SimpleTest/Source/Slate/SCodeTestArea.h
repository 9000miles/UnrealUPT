// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SSimpleTestTreeView.h"

class ISimpleTestTreeViewItem;
struct FCodeTestData;

DECLARE_DELEGATE_OneParam(FOnTestRowClick, const FString);
DECLARE_DELEGATE_OneParam(FOnRunTestItemClicked, const FString);

class SCodeTestArea : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SCodeTestArea)
	{ }
	SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, FOnTestRowClick InOnTestItemClick, FOnRunTestItemClicked OnItemRunTest);
	void SetCodeTestPaths(TArray<TSharedPtr<FCodeTestData>> TestPaths);

private:
	TSharedRef<ITableRow> OnGenerateWidgetForTest(TSharedPtr<ISimpleTestTreeViewItem> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	void OnGetChildren(TSharedPtr<ISimpleTestTreeViewItem> InItem, TArray<TSharedPtr<ISimpleTestTreeViewItem> >& OutItems);
	void OnTestSelectionChanged(TSharedPtr<ISimpleTestTreeViewItem> Selection, ESelectInfo::Type SelectInfo);
	void OnItemRunTest(const FString TestPath);

private:
	TSharedPtr<class SCheckBox> HeaderCheckbox;
	TSharedPtr<class SHeaderRow> TestTableHeaderRow;

	TSharedPtr<SSimpleTestTreeView<TSharedPtr<ISimpleTestTreeViewItem>>> TreeView;

	TArray<TSharedPtr<ISimpleTestTreeViewItem>> CodeTestPaths;

	FOnTestRowClick OnTestItemClicked;
	FOnRunTestItemClicked OnRunTestButtonClicked;
};