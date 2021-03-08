#include "SUnitToolTip.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "SBorder.h"
#include "SBoxPanel.h"
#include "ProjectInfo.h"
#include "SProjectUnit.h"
#include "STextBlock.h"
#include "SSeparator.h"
#include "Widgets/UPTStyle.h"

#define LOCTEXT_NAMESPACE "SUnitToolTip"

void SUnitToolTip::Construct(const FArguments& InArgs)
{
	ProjectUnit = InArgs._ProjectUnit;

	SToolTip::Construct(SToolTip::FArguments().TextMargin(1.0f));
}

bool SUnitToolTip::IsEmpty() const
{
	return !ProjectUnit.IsValid();
}

void SUnitToolTip::OnOpening()
{
	TSharedPtr<SProjectUnit> Unit = ProjectUnit.Pin();
	if (Unit.IsValid())
	{
		SetContentWidget(CreateToolTipWidget());
	}
	else
	{
		SetContentWidget(SNullWidget::NullWidget);
	}
}

void SUnitToolTip::OnClosed()
{
	SetContentWidget(SNullWidget::NullWidget);
}

TSharedRef<SWidget> SUnitToolTip::CreateToolTipWidget()
{
	TSharedRef<FProjectInfo> Info = ProjectUnit.Pin()->GetProjectInfo();

	return
		SNew(SBorder)
		.BorderImage(FUPTStyle::GetBrush("NoBorder"))
		.Padding(FMargin(10))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(Info->GetProjectName()))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0, 5))
			[
				SNew(SSeparator)
				.Thickness(3)
				.Orientation(Orient_Horizontal)
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(FText::FromString(Info->GetEngineVersion()))
			]
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(LOCTEXT("ProjectPath", "Project Path :"))
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(FMargin(0, 5))
			[
				SNew(SSeparator)
				.Thickness(3)
				.Orientation(Orient_Horizontal)
			]
			+ SVerticalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString(Info->GetProjectPath()))
			]
		];
}
#undef  LOCTEXT_NAMESPACE