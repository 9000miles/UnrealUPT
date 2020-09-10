#include "SUPTMainFrame.h"
#include "SBoxPanel.h"
#include "ProjectInfo.h"
#include "SEngineProjects.h"
#include "SScrollBox.h"
#include "UPTDefine.h"
#include "SSplitter.h"
#include "SCheckBox.h"
#include "EditorStyleSet.h"
#include "SOverlay.h"
#include "SSpacer.h"
#include "STextBlock.h"
#include "SImage.h"
#include "UPTStyle.h"

void SUPTMainFrame::Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& AllProjects)
{
	ChildSlot
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
			+SSplitter::Slot()
			.Value(0.3f)
			[
				SNew(SBorder)
				[
					SNew(SScrollBox)
					+SScrollBox::Slot()
					[
						SAssignNew(EngineBox, SVerticalBox)
					]
				]
			]
			+SSplitter::Slot()
			[
				SNew(SEngineProjects)
			]
		];

	CreateAllProjects(EngineBox->AsShared(), AllProjects);
}

void SUPTMainFrame::CreateAllProjects(TSharedRef<SVerticalBox>& VerticalBox, TArray<TSharedPtr<FProjectInfo>>& AllProjects)
{
	for (TSharedPtr<FProjectInfo> Info : AllProjects)
	{
		const FString Version = Info->GetEngineVersion();
		if (Map.Contains(Version))
			Map[Version].Add(Info);
		else
			Map.Add(Version, { Info });
	}

	//根据引擎版本排序
	Map.KeySort([](const FString& A, const FString& B) -> bool	{ return A < B; });

	//创建引擎版本对应的工程区块
	for (auto It = Map.CreateConstIterator(); It; ++It)
	{
		FString Version = (&It)->Key();
		TArray<TSharedPtr<FProjectInfo>> Infos = (&It)->Value();
		VerticalBox->AddSlot()
		.AutoHeight()
		.MaxHeight(ENGINE_VERSION_HEIGHT)
		.Padding(FMargin(2, 2))
		[
			CreateTab(Version)
		];
	}
}

TSharedRef<SWidget> SUPTMainFrame::CreateTab(FString Version)
{
	return 			
		SNew(SCheckBox)
		//.Style(FEditorStyle::Get(), "PlacementBrowser.Tab")
		.OnCheckStateChanged(this, &SUPTMainFrame::OnEngineTabChanged, Version)
		.IsChecked(this, &SUPTMainFrame::GetEngineTabCheckedState, Version)
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
			.VAlign(VAlign_Center)
			[
				SNew(SSpacer)
				.Size(FVector2D(1, 30))
			]
			+ SOverlay::Slot()
			.Padding(FMargin(6, 0, 15, 0))
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(Version)
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			[
				SNew(SImage)
				.Image(this, &SUPTMainFrame::GetActiveTabIamge, Version)
			]
			+ SOverlay::Slot()
			.VAlign(VAlign_Fill)
			.HAlign(HAlign_Left)
			[
				SNew(SImage)
				.Image(this, &SUPTMainFrame::GetSourceOrBinaryImage, Version)
			]
			+ SOverlay::Slot()
			.Padding(FMargin(6, 0, 15, 0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Right)
			[
				SNew(STextBlock)
				.Text(Version)
			]
		]
	;
}

void SUPTMainFrame::OnEngineTabChanged(ECheckBoxState NewState, const FString& EngineVersion)
{
	if (NewState == ECheckBoxState::Checked)
	{
		ActiveEngineTab = EngineVersion;
	}
}

ECheckBoxState SUPTMainFrame::GetEngineTabCheckedState(const FString& EngineVersion)
{
	return ActiveEngineTab == EngineVersion ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

const FSlateBrush* SUPTMainFrame::GetActiveTabIamge(const FString& EngineVersion)
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

const FSlateBrush* SUPTMainFrame::GetSourceOrBinaryImage(const FString& EngineVersion)
{
	const bool bEngineIsSource = true;
	static FName BrushName = bEngineIsSource ? "UPT.Source" : "UPT.Binary";
	return FUPTStyle::Get().GetBrush(BrushName);
}
