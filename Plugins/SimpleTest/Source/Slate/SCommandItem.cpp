#include "SCommandItem.h"
#include "SBoxPanel.h"
#include "EditorStyleSet.h"
#include "SHyperlink.h"
#include "SlateApplication.h"
#include "SButton.h"
#include "SBorder.h"

#define LOCTEXT_NAMESPACE "SCommandItem"

void SCommandItem::Construct(const FArguments& InArgs, const FString InDisplayName, const FString InFileName, const int32 InLineNumber)
{
	OnCommandClick = InArgs._OnCommandClick;
	DisplayName = InDisplayName;
	FileName = InFileName;
	LineNumber = InLineNumber;

	ChildSlot
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(SHyperlink)
				.Style(FEditorStyle::Get(), "Common.GotoNativeCodeHyperlink")
				.OnNavigate_Lambda([this] { FSlateApplication::Get().GotoLineInSource(FileName, LineNumber); })
				.Text(FText::FromString(DisplayName))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(0.4f)
			.HAlign(HAlign_Center)
			[
				SNew(SButton)
				.Text(LOCTEXT("ExecuteText", "Execute"))
				.OnClicked(this, &SCommandItem::OnCommandClicked)
			]
		];
}

FReply SCommandItem::OnCommandClicked()
{
	if (OnCommandClick.IsBound())
	{
		OnCommandClick.ExecuteIfBound(DisplayName);
	}

	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE