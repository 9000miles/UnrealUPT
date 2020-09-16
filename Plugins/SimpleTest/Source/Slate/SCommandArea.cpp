#include "SCommandArea.h"
#include "SBoxPanel.h"
#include "SSimpleTestMainFrame.h"
#include "SBorder.h"
#include "SButton.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "SCommandArea"

void SCommandArea::Construct(const FArguments& InArgs)
{
	OnSwitchedToCodeTestClicked = InArgs._OnSwitchedToCodeTestClicked;

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.FillHeight(0.2f)
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				.Padding(3)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.Text(LOCTEXT("Test Code Text", "Switch Code Test"))
					.ToolTipText(LOCTEXT("Test Code Text", "Test Code"))
					.OnClicked(this, &SCommandArea::OnSwitchCodeTestClick)
				]
			]

			+ SVerticalBox::Slot()
			.Padding(0, 2)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
				[
					SAssignNew(CommandBox, SVerticalBox)
				]
			]
		];
}

void SCommandArea::AddCommandWidget(TSharedRef<SWidget> Widget)
{
	CommandBox->AddSlot()
		.MaxHeight(20)
		.Padding(2, 2)
		[
			Widget
		];
}

FReply SCommandArea::OnSwitchCodeTestClick()
{
	if (OnSwitchedToCodeTestClicked.IsBound())
		OnSwitchedToCodeTestClicked.ExecuteIfBound();

	return FReply::Handled();
}

#undef LOCTEXT_NAMESPCE