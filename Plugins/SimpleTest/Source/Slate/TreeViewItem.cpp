#include "TreeViewItem.h"
#include "STextBlock.h"
#include "SButton.h"
#include "SHyperlink.h"
#include "EditorStyleSet.h"
#include "SlateApplication.h"

#define LOCTEXT_NAMESPACE "SSimpleTestItem"

void SSimpleTestItem::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView, FOnRunTestItemClicked OnRunTestDelegate)
{
	TestItem = InArgs._TestItem;
	OnItemRunTest = OnRunTestDelegate;

	SMultiColumnTableRow< TSharedPtr< FString > >::Construct(SMultiColumnTableRow< TSharedPtr< FString > >::FArguments(), InOwnerTableView);
}

TSharedRef<SWidget> SSimpleTestItem::GenerateWidgetForColumn(const FName& ColumnName)
{
	if (ColumnName == SimpleTestWindowConstants::Title)
	{
		return SNew(SHyperlink)
			.Style(FEditorStyle::Get(), "Common.GotoNativeCodeHyperlink")
			.OnNavigate_Lambda([this] { FSlateApplication::Get().GotoLineInSource(TestItem->GetSourceFileName(), TestItem->GetSourceFileLine()); })
			.Text(FText::FromString(TestItem->GetTestPath()));
	}
	else if (ColumnName == SimpleTestWindowConstants::Execute)
	{
		return SNew(SButton)
			.HAlign(HAlign_Center)
			.Text(LOCTEXT("ExecuteText", "Execute"))
			.OnClicked(this, &SSimpleTestItem::OnRunTestClick);
	}

	return SNullWidget::NullWidget;
}

FReply SSimpleTestItem::OnRunTestClick()
{
	if (OnItemRunTest.IsBound())
	{
		if (TestItem.IsValid())
			OnItemRunTest.ExecuteIfBound(TestItem->GetTestPath());
	}

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPACE

FString FSimpleTestTreeViewItem::GetSourceFileName()
{
	return FileName;
}

int32 FSimpleTestTreeViewItem::GetSourceFileLine()
{
	return LineNumber;
}

FString FSimpleTestTreeViewItem::GetTestPath()
{
	return TestName;
}