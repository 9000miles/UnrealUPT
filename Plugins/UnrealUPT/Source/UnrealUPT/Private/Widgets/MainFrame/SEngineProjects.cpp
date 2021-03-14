#include "SEngineProjects.h"
#include "SExpandableArea.h"
#include "../Classes/ProjectInfo.h"
#include "SProjectUnit.h"
#include "SGridPanel.h"
#include "SBorder.h"
#include "SBoxPanel.h"
#include "STextBlock.h"
#include "ProjectUnit/SProjectTileView.h"
#include "Editor/EditorStyle/Public/EditorStyleSet.h"
#include "SScrollBox.h"
#include "SHyperlink.h"
#include "SlateApplication.h"
#include "PlatformFilemanager.h"
#include "PrintHelper.h"
#include "SNotificationList.h"
#include "UPTDelegateCenter.h"
#include "Widgets/UPTStyle.h"

#define LOCTEXT_NAMESPACE "SEngineProjects"
void SEngineProjects::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateProjectsHeader()
			]
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Fill)
			[
				CreateProjectsBody()
			]
		];
}

void SEngineProjects::Refresh(TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	Init(Projects);
	ProjectTileView->Refresh(Projects);
}

void SEngineProjects::Init(TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	ProjectCount = Projects.Num();
	if (Projects.Num() > 0)
	{
		EnginePath = Projects[0]->GetEnginePath();
	}
}

TSharedRef<SWidget> SEngineProjects::CreateProjectsHeader()
{
	return
		SNew(SBorder)
		.Padding(FMargin(5))
		.BorderImage(FUPTStyle::GetBrush("ToolPanel.GroupBorder"))
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.Padding(FMargin(5, 0))
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(this, &SEngineProjects::GetProjectCount)
			]
			
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(5, 0))
			[
				SAssignNew(Hyperlink, SHyperlink)
				.OnNavigate_Lambda([this] { FPlatformProcess::ExploreFolder(*(EnginePath)); })
				.Text(this, &SEngineProjects::GetEnginePath)
			]
			
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Center)
			.AutoWidth()
			.Padding(FMargin(5, 0))
			[
				SNew(SButton)
				.Text(LOCTEXT("StartEngienButton", "Start Engine"))
				.OnClicked(this, &SEngineProjects::OpenEngine)
			]
		];
}

TSharedRef<SWidget> SEngineProjects::CreateProjectsBody()
{
	TArray<TSharedPtr<FProjectInfo>> Projects;

	return
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SAssignNew(ProjectTileView, SProjectTileView, Projects)
		];
}

FReply SEngineProjects::OpenEngine()
{
	if (FPaths::DirectoryExists(EnginePath))
	{
		FString ExeFilename;
		ExeFilename = EnginePath + "/Engine/Binaries/Win64/UE4Editor.exe";

		//判断UE4Editor.exe是否存在
		if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ExeFilename))
		{
			PRINT_ERROR("Engine path is error");
			return FReply::Unhandled();
		}

#if PLATFORM_WINDOWS
		//使用CMD打开引擎
		FProcHandle Handle = FPlatformProcess::CreateProc(*ExeFilename, TEXT(""), true, false, false, NULL, 0, NULL, NULL);
		if (!Handle.IsValid())
		{
			PRINT_ERROR("Failed to create process");
			return FReply::Unhandled();
		}

		FPlatformProcess::CloseProc(Handle);

		FNotificationInfo Info(FText::Format(LOCTEXT("StartEngine", "{0} Engine started"), FText::FromString(EnginePath)));
		Info.ExpireDuration = 3;
		Info.bUseLargeFont = false;

		if (FUPTDelegateCenter::OnRequestAddNotification.IsBound())
			FUPTDelegateCenter::OnRequestAddNotification.Execute(Info);
#endif
	}
	return FReply::Unhandled();
}

FText SEngineProjects::GetEnginePath() const
{
	return EnginePath.IsEmpty() ? LOCTEXT("NotFound", "Not found engine directory") : FText::FromString(EnginePath);
}

FText SEngineProjects::GetProjectCount() const
{
	return FText::Format(LOCTEXT("ProjectCount", "Projects Count : {0}"), FText::AsNumber(ProjectCount));
}
#undef LOCTEXT_NAMESPACE