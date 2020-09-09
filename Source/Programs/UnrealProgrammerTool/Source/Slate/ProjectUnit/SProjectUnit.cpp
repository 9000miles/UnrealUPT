#include "SProjectUnit.h"
#include "../Classes/ProjectInfo.h"
#include "SBorder.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "SImage.h"
#include "SlateBrush.h"
#include "../Manager/UPTDelegateCenter.h"
#include "SBox.h"
#include "UPTDefine.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "SUnitToolTip.h"

void SProjectThumbnail::Construct(const FArguments& InArgs, const FSlateBrush* Brush)
{
	OnDoubleClick = InArgs._OnDoubleClick;

	Image.Set(Brush);
}

FReply SProjectThumbnail::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return FReply::Handled();
}

void SProjectThumbnail::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
}

void SProjectThumbnail::OnMouseLeave(const FPointerEvent& MouseEvent)
{
}

FReply SProjectThumbnail::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	if (OnDoubleClick.IsBound())
		OnDoubleClick.ExecuteIfBound();

	return FReply::Handled();
}

void SProjectUnit::Construct(const FArguments& InArgs, const TSharedRef<FProjectInfo> Info)
{
	ProjectInfo = Info;

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("NoBorder"))
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				.Padding(FMargin(5))
				.AutoHeight()
				[
					SNew(SBox)
					.WidthOverride(PROJECT_THUMBNAIL_WIDTH)
					.HeightOverride(PROJECT_THUMBNAIL_HEIGHT)
					[
						SNew(SProjectThumbnail, GetThumbnail())
						.OnDoubleClick(this, &SProjectUnit::OnDoubleClicked)
						//SNew(SImage)
						//.Image(GetThumbnail())
					]
				]
				+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				.Padding(FMargin(5))
				[
					SNew(STextBlock)
					.Text(FText::FromString(ProjectInfo->GetProjectName()))
				]
			]
		];

	SetToolTip(SNew(SUnitToolTip).ProjectUnit(SharedThis(this)));
}

void SProjectUnit::OnDoubleClicked()
{
	if (!ProjectInfo.IsValid())
		return;

	if (FUPTDelegateCenter::OnOpenProject.IsBound())
	{
		FUPTDelegateCenter::OnOpenProject.ExecuteIfBound(ProjectInfo.ToSharedRef());
	}
}

FSlateBrush* SProjectUnit::GetThumbnail()
{
	FSlateBrush* Default = nullptr;

	if (!ProjectInfo.IsValid())
		return Default;

	TSharedPtr<FSlateBrush> Brush = ProjectInfo->GetThumbanil();
	return Brush.IsValid() ? Brush.Get() : Default;
}