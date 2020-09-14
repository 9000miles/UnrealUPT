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

#define LOCTEXT_NAMESPACE "SEngineProjects"
void SEngineProjects::Construct(const FArguments& InArgs)
{
	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			[
				CreateProjectsHeader()
			]
			+SVerticalBox::Slot()
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

void SEngineProjects::Init(TArray<TSharedPtr<FProjectInfo>> &Projects)
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
	.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.Padding(FMargin(5, 0))
		.HAlign(HAlign_Fill)
		[
			SNew(STextBlock)
			.Text(this, &SEngineProjects::GetProjectCount)
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(FMargin(5, 0))
		[
			SAssignNew(Hyperlink, SHyperlink)
			.OnNavigate_Lambda([this] { FPlatformProcess::ExploreFolder(*(EnginePath)); })
			.Text(this, &SEngineProjects::GetEnginePath)
		]
	];
}

TSharedRef<SWidget> SEngineProjects::CreateProjectsBody()
{
	TArray<TSharedPtr<FProjectInfo>> Projects;

	return
	SNew(SScrollBox)
	+SScrollBox::Slot()
	[
		SAssignNew(ProjectTileView, SProjectTileView, Projects)
	];
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