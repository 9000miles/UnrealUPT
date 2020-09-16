// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SSimpleTestMainFrame.h"
#include "SCodeTestArea.h"

/** Columns for the test tree view */
namespace SimpleTestWindowConstants
{
	const FName Title(TEXT("Name"));
	const FName Execute(TEXT("Execute"));
}

class ISimpleTestTreeViewItem :public TSharedFromThis<ISimpleTestTreeViewItem>
{
public:
	virtual FString GetTestPath() = 0;
	virtual FString GetSourceFileName() = 0;
	virtual int32 GetSourceFileLine() = 0;

	virtual ~ISimpleTestTreeViewItem() {}

protected:
	FString TestName;
	FString FileName;
	int32 LineNumber;
};

class FSimpleTestTreeViewItem :public ISimpleTestTreeViewItem
{
public:
	FSimpleTestTreeViewItem(const FString Name, const FString InFileName, const int32 InLineNumber)
	{
		TestName = Name;
		FileName = InFileName;
		LineNumber = InLineNumber;
	}

public:
	FString GetSourceFileName() override;
	int32 GetSourceFileLine() override;
	FString GetTestPath() override;
};

class SSimpleTestItem : public SMultiColumnTableRow< TSharedPtr<FString> >
{
public:
	SLATE_BEGIN_ARGS(SSimpleTestItem)
	{}
	SLATE_ARGUMENT(TSharedPtr<ISimpleTestTreeViewItem>, TestItem)
		SLATE_END_ARGS()

public:
	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FOnRunTestItemClicked OnRunTest);

private:
	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;
	FReply OnRunTestClick();

private:
	TSharedPtr<ISimpleTestTreeViewItem> TestItem;
	FOnRunTestItemClicked OnItemRunTest;
};