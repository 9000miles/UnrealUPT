#include "SWorkArea.h"
#include "SBoxPanel.h"
#include "SCodeTestArea.h"
#include "SSlateTestArea.h"
#include "SSimpleTestMainFrame.h"
#include "SimpleTestManager.h"

#define LOCTEXT_NAMESPACE "SWorkArea"
void SWorkArea::Construct(const FArguments& InArgs, FOnTestRowClick InOnTestItemClick, FOnRunTestItemClicked OnItemRunTest)
{
	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SAssignNew(CodeTestArea, SCodeTestArea, InOnTestItemClick, OnItemRunTest)
				.Visibility(EVisibility::Visible)
			]

			+ SVerticalBox::Slot()
			[
				SAssignNew(SlateTestArea, SSlateTestArea)
				.Visibility(EVisibility::Collapsed)
			]
		];
}

void SWorkArea::SwitchArea(bool bSwitchToCodeTest)
{
	CodeTestArea->SetVisibility(bSwitchToCodeTest ? EVisibility::Visible : EVisibility::Collapsed);
	SlateTestArea->SetVisibility(bSwitchToCodeTest ? EVisibility::Collapsed : EVisibility::Visible);
}

void SWorkArea::SetSlateTestWidget(TSharedPtr<SWidget> Widget)
{
	if (SlateTestArea.IsValid())
	{
		SlateTestArea->SetTestWidget(Widget->AsShared());
	}
}

void SWorkArea::SetCodeTestPaths(TArray<TSharedPtr<FCodeTestData>> TestPaths)
{
	if (CodeTestArea.IsValid())
	{
		CodeTestArea->SetCodeTestPaths(TestPaths);
	}
}

#undef LOCTEXT_NAMESPACE