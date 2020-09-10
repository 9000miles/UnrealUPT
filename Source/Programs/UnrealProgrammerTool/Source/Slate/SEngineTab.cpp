#include "SEngineTab.h"
#include "SCheckBox.h"
#include "SBorder.h"
#include "SScrollBox.h"
#include "SBoxPanel.h"
#include "SSpacer.h"
#include "STextBlock.h"
#include "SImage.h"
#include "UPTStyle.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "SEngineTab"
void SEngineTab::Construct(const FArguments& InArgs)
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

void SEngineTab::Refresh(TArray<FString> Names)
{
	TabBox->ClearChildren();

	for (const FString TabName : TabNames)
	{
		TabBox->AddSlot()
		.HAlign(HAlign_Fill)
			[
				CreateTab(TabName)
			];
	}
}

TSharedRef<SWidget> SEngineTab::CreateTab(FString TabName)
{
	return
		SNew(SCheckBox)
		.Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
		.OnCheckStateChanged(this, &SEngineTab::OnEngineTabChanged, TabName)
		.IsChecked(this, &SEngineTab::GetEngineTabCheckedState, TabName)
		.ToolTipText(this, &SEngineTab::TabToolTipText, TabName)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			.AutoWidth()
			[
				SNew(SImage)
				.Image(this, &SEngineTab::GetActiveTabIamge, TabName)
			]
			+ SHorizontalBox::Slot()
			.Padding(FMargin(5, 0))
			.AutoWidth()
			[
				SNew(SImage)
				.Image(this, &SEngineTab::GetSourceOrBinaryImage, TabName)
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

void SEngineTab::OnEngineTabChanged(ECheckBoxState NewState, FString EngineVersion)
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

ECheckBoxState SEngineTab::GetEngineTabCheckedState(FString EngineVersion) const
{
	return ActiveEngineTab == EngineVersion ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

const FSlateBrush* SEngineTab::GetActiveTabIamge(FString EngineVersion) const
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

const FSlateBrush* SEngineTab::GetSourceOrBinaryImage(FString EngineVersion) const
{
	if (OnGetTabBrush.IsBound())
	{
		return OnGetTabBrush.Execute(EngineVersion);
	}

	return nullptr;
}

FText SEngineTab::TabToolTipText(FString EngineVersion) const
{
	if (OnGetToolTipText.IsBound())
	{
		return OnGetToolTipText.Execute(EngineVersion);
	}

	return LOCTEXT("Tab Tool Tip Text", "Tab Tool Tip Text");
}
#undef LOCTEXT_NAMESPACE

