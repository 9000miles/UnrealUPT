#include "SCommonTab.h"
#include "SCheckBox.h"
#include "SBorder.h"
#include "SScrollBox.h"
#include "SBoxPanel.h"
#include "SSpacer.h"
#include "STextBlock.h"
#include "SImage.h"
#include "UPTStyle.h"
#include "EditorStyleSet.h"
#include "SEngineContextMenu.h"
#include "PrintHelper.h"

#define LOCTEXT_NAMESPACE "SEngineTab"

FReply SCommonTab::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		//FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
		//FSlateApplication::Get().PushMenu(AsShared(), WidgetPath, MenuContent.ToSharedRef(), SummonLocation, FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu));
		FReply Reply = FReply::Handled().ReleaseMouseCapture();
		return Reply;
	}
	return FReply::Unhandled();
}

void SCommonTab::Construct(const FArguments& InArgs)
{
	TabNames = InArgs._TabNames;
	OnTabActive = InArgs._OnTabActive;
	OnGetTabBrush = InArgs._OnGetTabBrush;
	OnGetToolTipText = InArgs._OnGetToolTipText;

	ChildSlot
		[
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
		.Padding(FMargin(5))
		[
			SNew(SScrollBox)
			+ SScrollBox::Slot()
		[
			SAssignNew(TabBox, SVerticalBox)
		]
		]
		];

	Refresh(TabNames);
}

void SCommonTab::Refresh(TArray<FString> Names)
{
	TabBox->ClearChildren();

	for (const FString TabName : Names)
	{
		TabBox->AddSlot()
			.HAlign(HAlign_Fill)
			[
				CreateTab(TabName)
			];
	}
}

void SCommonTab::SetActiveTab(const FString TabName)
{
	ActiveEngineTab = TabName;

	if (OnTabActive.IsBound())
		OnTabActive.ExecuteIfBound(ActiveEngineTab);
}

TSharedRef<SWidget> SCommonTab::CreateTab(FString TabName)
{
	bool bSourceEngine;
	const FSlateBrush* SourceBinaryBrush = GetSourceOrBinaryImage(TabName, bSourceEngine);
	return
		SNew(SCheckBox)
		.Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
		.OnCheckStateChanged(this, &SCommonTab::OnEngineTabChanged, TabName)
		.IsChecked(this, &SCommonTab::GetEngineTabCheckedState, TabName)
		.ToolTipText(this, &SCommonTab::TabToolTipText, TabName, bSourceEngine)
		.OnGetMenuContent(this, &SCommonTab::MakeMenuContent)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Left)
		.AutoWidth()
		[
			SNew(SImage)
			.Image(this, &SCommonTab::GetActiveTabIamge, TabName)
		]
	+ SHorizontalBox::Slot()
		.Padding(FMargin(5, 0))
		.AutoWidth()
		[
			SNew(SImage)
			.Image(SourceBinaryBrush)
		]
	+ SHorizontalBox::Slot()
		.Padding(FMargin(6, 0, 15, 0))
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TabName))
		]
		]
	;
}

TSharedRef<SWidget> SCommonTab::MakeMenuContent()
{
	return SNew(SSEngineContextMenu, ActiveEngineVersion);
}

void SCommonTab::OnEngineTabChanged(ECheckBoxState NewState, FString EngineVersion)
{
	if (NewState == ECheckBoxState::Checked)
	{
		ActiveEngineTab = EngineVersion;
		if (OnTabActive.IsBound())
		{
			OnTabActive.ExecuteIfBound(EngineVersion);
		}
	}
}

ECheckBoxState SCommonTab::GetEngineTabCheckedState(FString EngineVersion) const
{
	return ActiveEngineTab == EngineVersion ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

const FSlateBrush* SCommonTab::GetActiveTabIamge(FString EngineVersion) const
{
	if (ActiveEngineTab == EngineVersion)
	{
		static FName PlacementBrowserActiveTabBarBrush("PlacementBrowser.ActiveTabBar");
		return FEditorStyle::GetBrush(PlacementBrowserActiveTabBarBrush);
	}
	else
	{
		return nullptr;
	}
}

const FSlateBrush* SCommonTab::GetSourceOrBinaryImage(FString EngineVersion, bool& bSource) const
{
	if (OnGetTabBrush.IsBound())
	{
		return OnGetTabBrush.Execute(EngineVersion, bSource);
	}

	return nullptr;
}

FText SCommonTab::TabToolTipText(FString EngineVersion, bool bSource) const
{
	FText TipText;
	if (OnGetToolTipText.IsBound())
	{
		FText EngienType = bSource ? LOCTEXT("Source Engine", "Source Engine  -  {0}") : LOCTEXT("Binary Engine", "Binary Engine  -  {0}");
		TipText = OnGetToolTipText.Execute(EngineVersion);

		if (!TipText.IsEmpty())
			return FText::Format(EngienType, TipText);
	}

	return TipText.IsEmpty() ? LOCTEXT("NotFound", "Not found engine directory") : TipText;
}
#undef LOCTEXT_NAMESPACE