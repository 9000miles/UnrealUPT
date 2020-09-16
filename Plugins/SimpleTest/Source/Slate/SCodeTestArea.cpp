#include "SCodeTestArea.h"
#include "TreeViewItem.h"
#include "SDockTab.h"
#include "SSimpleTestTreeView.h"
#include "SHeaderRow.h"
#include "SCheckBox.h"
#include "SButton.h"
#include "SimpleTestManager.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "SDisplayArea"

#pragma optimize("",off)
void SCodeTestArea::Construct(const FArguments& InArgs, FOnTestRowClick InOnTestItemClick, FOnRunTestItemClicked OnItemRunTest)
{
	OnTestItemClicked = InOnTestItemClick;
	OnRunTestButtonClicked = OnItemRunTest;

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			[
				SAssignNew(TreeView, SSimpleTestTreeView < TSharedPtr < ISimpleTestTreeViewItem >>)
				.SelectionMode(ESelectionMode::Multi)
				.TreeItemsSource(&CodeTestPaths)
				.OnGenerateRow(this, &SCodeTestArea::OnGenerateWidgetForTest)
				.OnGetChildren(this, &SCodeTestArea::OnGetChildren)
				.OnSelectionChanged(this, &SCodeTestArea::OnTestSelectionChanged)
				.ItemHeight(20.0f)
				.HeaderRow
				(
					SAssignNew(TestTableHeaderRow, SHeaderRow)
					+ SHeaderRow::Column(SimpleTestWindowConstants::Title)
					.HAlignHeader(HAlign_Center)
					.VAlignHeader(VAlign_Center)
					.HAlignCell(HAlign_Left)
					.VAlignCell(VAlign_Center)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("TestName_Header", "Test Name"))
					]

					+ SHeaderRow::Column(SimpleTestWindowConstants::Execute)
					.FixedWidth(80.0f)
					.HAlignHeader(HAlign_Center)
					.VAlignHeader(VAlign_Center)
					.HAlignCell(HAlign_Fill)
					.VAlignCell(VAlign_Fill)
					[
						SNew(STextBlock)
						.Text(LOCTEXT("ExecuteText", "Execute"))
						.ToolTipText(LOCTEXT("ExecuteText", "Execute"))
					]
				)
			]
		];
}

void SCodeTestArea::SetCodeTestPaths(TArray<TSharedPtr<FCodeTestData>> TestPaths)
{
	CodeTestPaths.Empty();

	for (TSharedPtr<FCodeTestData> Data : TestPaths)
	{
		CodeTestPaths.Add(MakeShareable(new FSimpleTestTreeViewItem(Data->DisplayName, Data->FileName, Data->LineNumber)));
	}

	TreeView->SetTreeItemsSource(&CodeTestPaths);
	TreeView->RequestListRefresh();
	TreeView->RequestTreeRefresh();
}

TSharedRef<ITableRow> SCodeTestArea::OnGenerateWidgetForTest(TSharedPtr<ISimpleTestTreeViewItem> InItem, const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SSimpleTestItem,OwnerTable, FOnTestRowClick::CreateSP(this, &SCodeTestArea::OnItemRunTest))
		.TestItem(InItem);
}

void SCodeTestArea::OnGetChildren(TSharedPtr<ISimpleTestTreeViewItem> InItem, TArray<TSharedPtr<ISimpleTestTreeViewItem> >& OutItems)
{
	OutItems.Add(InItem);
}

void SCodeTestArea::OnTestSelectionChanged(TSharedPtr<ISimpleTestTreeViewItem> Selection, ESelectInfo::Type SelectInfo)
{
	if (OnTestItemClicked.IsBound())
	{
		if (Selection.IsValid())
			OnTestItemClicked.ExecuteIfBound(Selection->GetTestPath());
	}
}

void SCodeTestArea::OnItemRunTest(const FString TestPath)
{
	if (OnRunTestButtonClicked.IsBound())
	{
		OnRunTestButtonClicked.ExecuteIfBound(TestPath);
	}

	if (OnTestItemClicked.IsBound())
	{
		OnTestItemClicked.ExecuteIfBound(TestPath);
	}
}

#pragma optimize("",on)

#undef LOCTEXT_NAMESPACE