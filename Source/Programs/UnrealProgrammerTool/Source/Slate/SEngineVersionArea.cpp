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

void SEngineVersionArea::Construct(const FArguments& InArgs, const FString& Version, TArray<TSharedPtr<FProjectInfo>>& Projects)
{
	EngineVersion = Version;

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
