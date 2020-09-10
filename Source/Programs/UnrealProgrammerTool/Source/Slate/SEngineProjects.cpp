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

void SEngineProjects::Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	Init(Projects);

	ChildSlot
		[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			[
				CreateAreaHeader()
			]
			+SVerticalBox::Slot()
			[
				CreateAreaBody(Projects)
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

TSharedRef<SWidget> SEngineProjects::CreateAreaHeader()
{
	return
	SNew(SBorder)
	.BorderImage(FEditorStyle::GetBrush("NoBorder"))
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(FMargin(5, 0))
		[
			SNew(SHyperlink)
			.OnNavigate_Lambda([this] { FPlatformProcess::ExploreFolder(*(EnginePath)); })
			.Text(FText::FromString(EnginePath))
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(FMargin(5, 0))
		.AutoWidth()
		[
			SNew(STextBlock)
			.Text(FText::FromString(FString::FromInt(ProjectCount)))
		]
	];
}

TSharedRef<SWidget> SEngineProjects::CreateAreaBody(TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	return
	SNew(SScrollBox)
	+SScrollBox::Slot()
	[
		SAssignNew(ProjectTileView, SProjectTileView, Projects)
	];
}
