#include "SUPTMainFrame.h"
#include "SBoxPanel.h"
#include "../Classes/ProjectInfo.h"
#include "SEngineVersionArea.h"
#include "SScrollBox.h"

void SUPTMainFrame::Construct(const FArguments& InArgs, TArray<TSharedPtr<FProjectInfo>>& AllProjects)
{
	TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

	CreateAllProjects(VerticalBox, AllProjects);

	ChildSlot
		[
			SNew(SScrollBox)
			+SScrollBox::Slot()
			[
				VerticalBox
			]
		];
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

	for (auto It = Map.CreateConstIterator(); It; ++It)
	{
		FString Version = (&It)->Key();
		TArray<TSharedPtr<FProjectInfo>> Infos = (&It)->Value();
		VerticalBox->AddSlot()
		.AutoHeight()
		.MaxHeight(500)
		.Padding(FMargin(5))
		[
			SNew(SEngineVersionArea, Version, Infos)
		];
	}
}