#include "SEngineVersionArea.h"
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

void SEngineVersionArea::Construct(const FArguments& InArgs, const FString& Version, TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	EngineVersion = Version;

	ProjectCount = Projects.Num();
	if (Projects.Num() > 0)
	{
		EnginePath = Projects[0]->GetEnginePath();
	}

	ChildSlot
		[
			SNew(SExpandableArea)
			.InitiallyCollapsed(false)
			.HeaderContent()
			[
				CreateAreaHeader(Version)
			]
			.BodyContent()
			[
				CreateAreaBody(Projects)
			]
		];
}

TSharedRef<SWidget> SEngineVersionArea::CreateAreaHeader(const FString& Version)
{
	return
	SNew(SBorder)
	.BorderImage(FEditorStyle::GetBrush("NoBorder"))
	[
		SNew(SHorizontalBox)
		+SHorizontalBox::Slot()
		[	
			SNew(STextBlock)
			.Text(FText::FromString(Version))
		]
		+ SHorizontalBox::Slot()
		.HAlign(HAlign_Right)
		.Padding(FMargin(5, 0))
		[
			SNew(SHyperlink)
			//.Style(FEditorStyle::Get(), "Common.GotoNativeCodeHyperlink")
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
	]
	;
}


TSharedRef<SWidget> SEngineVersionArea::CreateAreaBody(TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	return
	SNew(SScrollBox)
	+SScrollBox::Slot()
	[
		SNew(SProjectTileView, Projects)
	]
	;
}
