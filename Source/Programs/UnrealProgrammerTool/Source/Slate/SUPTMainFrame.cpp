#include "SUPTMainFrame.h"
#include "SBoxPanel.h"
#include "ProjectInfo.h"
#include "SEngineVersionArea.h"
#include "SScrollBox.h"
#include "UPTDefine.h"

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
		.Padding(FMargin(5))
		[
			SNew(SEngineVersionArea, Version, Infos)
		];
	}
}