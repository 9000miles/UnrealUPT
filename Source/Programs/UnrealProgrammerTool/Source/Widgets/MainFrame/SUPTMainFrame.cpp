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
#include "SCommonTab.h"
#include "UPTManager.h"
#include "SNotificationList.h"

#define LOCTEXT_NAMESPACE "SUPTMainFrame"

void SUPTMainFrame::Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& AllProjects)
{
	TArray<FString> Tabs;

	SAssignNew(EngineTab, SCommonTab)
		.TabNames(Tabs)
		.OnTabActive(this, &SUPTMainFrame::OnEngineTabChanged)
		.OnGetTabBrush(this, &SUPTMainFrame::GetSourceOrBinaryImage)
		.OnGetToolTipText(this, &SUPTMainFrame::OnGetEngineDir);

	SAssignNew(EngineProjects, SEngineProjects);

	ChildSlot
		.Padding(FMargin(2))
		[
			SNew(SOverlay)
			+ SOverlay::Slot()
		[
			SNew(SSplitter)
			.Orientation(Orient_Horizontal)
		+ SSplitter::Slot()
		.Value(0.3f)
		[
			EngineTab->AsShared()
		]
	+ SSplitter::Slot()
		[
			EngineProjects.ToSharedRef()
		]
		]

	+ SOverlay::Slot()
		.HAlign(HAlign_Right)
		.VAlign(VAlign_Bottom)
		.Padding(15)
		[
			SAssignNew(NotificationListPtr, SNotificationList)
			.Visibility(EVisibility::SelfHitTestInvisible)
		]
		];

	RequestRefresh(AllProjects);

	FUPTManager::Get()->SetNotificationList(NotificationListPtr.ToSharedRef());
}

void SUPTMainFrame::RequestRefresh(TArray<TSharedPtr<FProjectInfo>>& AllProjects)
{
	Map.Empty();

	InitEngineProjects(AllProjects);

	TArray<FString> EngineVersions;
	Map.GenerateKeyArray(EngineVersions);

	EngineTab->Refresh(EngineVersions);

	if (EngineVersions.Num() > 0)
	{
		EngineTab->SetActiveTab(EngineVersions[0]);
	}
}

void SUPTMainFrame::InitEngineProjects(TArray<TSharedPtr<FProjectInfo>>& AllProjects)
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
	Map.KeySort([](const FString& A, const FString& B) -> bool { return A < B; });
}

void SUPTMainFrame::OnEngineTabChanged(const FString& EngineVersion)
{
	TArray<TSharedPtr<FProjectInfo>> ProjectInfos;
	if (Map.Contains(EngineVersion))
	{
		ProjectInfos = Map[EngineVersion];
	}

	EngineProjects->Refresh(ProjectInfos);
}

const FSlateBrush* SUPTMainFrame::GetSourceOrBinaryImage(const FString& EngineVersion, bool& bSource)
{
	FString RootDir;
	if (Map.Contains(EngineVersion))
	{
		if (Map[EngineVersion].Num() > 0)
		{
			TSharedPtr<FProjectInfo> Info = Map[EngineVersion][0];
			if (Info.IsValid())
			{
				RootDir = Info->GetEnginePath();
				if (RootDir.IsEmpty()) return FUPTStyle::Get().GetBrush(FName("UPT.Tab.NotFound"));
			}
		}
	}

	bSource = FUPTManager::Get()->EngineIsDistributionByRootDir(RootDir);
	FName BrushName = bSource ? FName("UPT.Tab.Source") : FName("UPT.Tab.Binary");
	return FUPTStyle::Get().GetBrush(BrushName);
}

const FText SUPTMainFrame::OnGetEngineDir(const FString& EngineVersion)
{
	if (Map.Contains(EngineVersion))
	{
		if (Map[EngineVersion].Num() > 0)
		{
			return FText::FromString(Map[EngineVersion][0]->GetEnginePath());
		}
	}
	return LOCTEXT("not found engine", "not found engine");
}
#undef LOCTEXT_NAMESPACE